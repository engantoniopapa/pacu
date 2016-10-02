#include "header.h"



//  Avvio fase di boot del client, ritorna: 
//	0 se il boot è fallito, 
//	1 se il boot è andato a buon fine e la versione del client era aggiornata, 
//  2 se il boot è andato a buon fine e la versione server del client è aggiornata, ma non la versione domini
//  4 se il boot è andato a buon fine e la versione domini del client è da aggiornare, la versione server è stata aggiornata
//	Parametri -> descrittore del socket di connessione 
int init_boot_fase(int socksd)
{
	int state = 0;

	BootMessage* msg;
	set_timeout(socksd  , timeout_boot  , 0  , timeout_boot  , 0 );	
	msg = init_bootmessage(_BOOT_PING);

	if( send_ping(socksd , msg ) )
	{
		if(recv_pong(socksd , msg ) && msg->pay_desc == _BOOT_PONG )
		{	
			state = check_state_boot( msg->versione_dominio , msg->versione_primay);
			
			server_v_primary = msg->versione_primay ;
			server_v_dom = msg->versione_dominio ;

			if( state == 4 )
			{
				if(send_req_lilst(socksd , msg ))
				{
					if( recv_server_list(socksd) )
					{
						if(!update_version(server_v_primary , 0))
						{
							printf("Errore nell'aggiornamento della versione primary \n");
						}
					}
					else
					{
						printf("Errore nella ricezione della lista server \n");
					}
				}
			}		
		}
		else
		{
			if( msg->pay_desc == _BOOT_ERROR )
				printf("Errore nella fase di Boot\n");
		}
	
	}

	free(msg);

	close(socksd);
	return state;
}



//	Invio messaggio di Bootping, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, messaggio ping da inviare
int send_ping(int connsd , BootMessage* msg )
{
	int rc;
	int b;

	if( (rc = send(connsd,msg,sizeof(BootMessage ),MSG_NOSIGNAL) 	)   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	
	return b;
}



//	Ricezione messaggio di Bootpong, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al messaggio da ricevere
int  recv_pong( int connsd , BootMessage * buffer_pong )
{
	int rc;
	int b;
	memset(buffer_pong, 0 , sizeof(BootMessage ) );

	if (   (rc = recv(connsd,buffer_pong,sizeof(BootMessage),0) )   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella recv: ");
			b = 0;
		}
	} 

	if( buffer_pong->pay_desc == _BOOT_PONG)
	{
		b = 1 ;
	}
	else
	{
		b = 0 ;
	}
	return b;
}



//  Verifica la versione del client, ritorna:
//	1 se il boot è andato a buon fine e la versione del client è quella + aggiornata, 
//  2 se il boot è andato a buon fine e la versione server del client è aggiornata, ma non la versione domini
//  3 se il boot è andato a buon fine e la versione domini del client è aggiornata, ma non la versione server
//  4 se il boot è andato a buon fine e la versione del client è da aggiornare
//  Parametri -> la versione del server boot dei domini , la versione del server boot dei server.
int check_state_boot( double boot_dom , double boot_server)
{
	int i ;

	if(version_dom != boot_dom  || version_serv != boot_server )
	{
		if(version_dom != boot_dom  && version_serv != boot_server)
		{
			i = 4;
		}
		else
		{
			
			if(version_dom != boot_dom)
				i= 2 ;
		}
	}
	else
	{
		i = 1;
	}

	return i ;

}



//	Invio messaggio di BootList, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, messaggio ping da inviare
int send_req_lilst(int connsd , BootMessage* msg )
{
	int rc;
	int b;

	set_bootmessage(msg , _BOOT_LIST);

	if( (rc = send(connsd,msg,sizeof(BootMessage ),MSG_NOSIGNAL) 	)   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	
	return b;
}



//	ricevo la lista PrimaryServer, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione
int recv_server_list(int connsd)
{
	int b = 0;

	char *list;
 
	list =  writeFile_on_buffer(connsd , max_line);

	if( list == NULL)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
		}
		else
		{
			perror("errore nella send: ");
		}
	} 
	else
	{
		b = parsing_server_list(list) ;
		if(b)
		{
			char *stringa_errore;
			stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
			if(!insert_servlist_DB(list  , stringa_errore) )
			{
				b = 0 ;
				fprintf(stderr, "%s\n", stringa_errore);
			}
			free(stringa_errore);
		}	
	}
	free(list);
	return b ;
}


//  analizza la lista server, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//  Parametri -> lista server
int parsing_server_list(char *list)
{
	printf("lista :\n");
	printf("%s \n" , list);

	if( !anal_sint_list(list) )
		return 0 ;
	int rc;
	char *stringa_errore;

	DB *db_server; // descrittore del DB Primary
	
	db_server = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );

	rc = open_db_rw( db_server , PATH_DB_PRIMARY , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	char *string_query = "delete from ListServer";
	rc = delete_db(db_server , string_query , stringa_errore);
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	rc = close_db(db_server, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(db_server);
	free(stringa_errore);
	return 1 ;
}



// 	Inserisci nel Db la serv list ricevuta, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri->la lista di server da inserire, un puntatore char per "restituire" l'eventuale errore
int insert_servlist_DB( char *list   , char* string_error)
{
	DB *db_server; // descrittore del DB Primary
	char *stringa_errore;
	int rc;

	int i =0 ;
	int count = 0;
	char *string_query = "insert into ListServer values('";
	char *query;
	
	db_server = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );

	rc = open_db_rw( db_server , PATH_DB_PRIMARY , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	count = strlen(string_query) + 1;

	while( i < strlen(list) )
	{
		if(list[i] == '^' )
		{
			++count;
			query = realloc(query , count * sizeof(char) );
			query[count-1] = '\0' ;
			strncat(query , ")" , sizeof(char));

			if(insert_db(db_server , query , string_error))
			{
					free(query);
					i++;
			}
			else
			{
				free(query);
				return 0;
			}
	
		}
		else
		{		
			query = (char *)calloc ( count , sizeof(char) );
			strcpy( query , string_query );		
			while(list[i] != '|')
			{ 
				++count;
				query = realloc(query , count * sizeof(char) );
				query[count-1] = '\0' ;
				strncat(query , &list[i] , 1);
				++i;
			}

			count += 3;

			query = realloc(query , count * sizeof(char) );
			memset(&query[count-4] , '\0' , 3);
			strncat(query , "','" , 3);

			++i;
			while(list[i] != '|')
			{
				++count;
				query = realloc(query , count * sizeof(char) );
				query[count-1] = '\0';
				strncat(query , &list[i] , 1);

				++i;
			}

			count +=3 ;
			query = realloc(query , count * sizeof(char) );
			memset(&query[count-4] , '\0' , 3);
			strncat(query , "', " , 3);
			++i;

			while(list[i] != '^')
			{
				++count; 
				query = realloc(query , count * sizeof(char) );
				query[count-1] = '\0' ;
				strncat(query , &list[i] , 1);
				++i;
			}
		}
	}	

	rc = close_db(db_server, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(db_server);
	free(stringa_errore);
	return 1;
}


