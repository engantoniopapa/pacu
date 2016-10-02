#include "header.h"


int send_update_dom(int  , InitMessage*  );


//aggiorna la tabella domini
int update_domini()
{
	int index_server;
	int			sockfd;
	int rc;
	
	int tentativi = len_list_server * 3;
	int count_prove = 0 ;
	
	while(count_prove < tentativi)
	{
		index_server = generatoreRandom(len_list_server);
		
		if( ( sockfd = connessioneClient(list_server[index_server].ip ,  list_server[index_server].port , 3) ) <= 0 ) 
		{
			count_prove++;
		}
		else
		{
			count_prove = tentativi;
		}
	}
	InitMessage *msg ;
	msg = init_InitMessage(_INIT_UPDATE_DOM, NULL , 0 , 0 , 0);

	set_timeout(sockfd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );	

	if(send_update_dom(sockfd ,  msg ) )
	{
		free(msg);
		DiagMessage *msgD;
		msgD = (DiagMessage*) calloc (  1 , sizeof(DiagMessage)  );
		rc = recv_DiagMessage( sockfd , msgD) ;
		if( rc == 1  &&  msgD->pay_desc == _DIAG_ACK)
		{
			free(msgD);
			if( recv_dom_list(sockfd) )
			{
				if(!update_version(0 , server_v_dom))
				{
					printf("Errore nell'aggiornamento della versione domini \n");
					return 0;
				}
			}
			else
			{
				printf("Errore nella ricezione della lista domini \n");
				return 0;
			}
		}
		else
		{
			printf("Errore nella ricezione dell' Ack \n");
			free(msgD);
			return 0;
		}
	}
	else
	{
		free(msg);
		return 0;
	}

	return 1;

}



// richiedi la creazione della lista server
// percorso del db;
void get_list_server(char *path)
{
	int rc;
	char *stringa_errore;
	DB *db_primary; // descrittore del DB Server


	db_primary = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );

	rc = open_db_ro( db_primary , path , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }


	list_server = create_list_server(db_primary ,  &rc , "ListServer" , "" , stringa_errore );
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	rc = close_db(db_primary, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(stringa_errore);
	free(db_primary);
}


// richiedi la creazione della lista domini
void get_list_domini(char *path)
{
	int rc;
	char *stringa_errore;
	DB *db_dom; // descrittore del DB Server


	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );

	rc = open_db_ro( db_dom , path , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	list_domini = create_list_domini(db_dom , &rc , "Dom" , "", stringa_errore );
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

		rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(stringa_errore);
	free(db_dom);
}



//	Invio messaggio di Update Dom, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, messaggio da inviare
int send_update_dom(int connsd , InitMessage* msg )
{
	int rc;
	int b;

	if( (rc = send(connsd,msg,sizeof(InitMessage ),MSG_NOSIGNAL) 	)   ==  0)
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



//	ricevo la lista Domini, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione
int recv_dom_list(int connsd)
{
	int b = 0;

	char *list;
 						
	set_timeout(connsd  , timeout_domlist , 0  , timeout_domlist , 0 );
 
	list =  writeFile_on_buffer_noblock(connsd , max_line);

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
		b = parsing_dom_list(list) ;
		if(b)
		{
			char *stringa_errore;
			stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
			if(!insert_domlist_DB(list  , stringa_errore) )
			{
				b = 0 ;
				fprintf(stderr, "%s\n", stringa_errore);
			}
			free(stringa_errore);
		}	
	}
	
	printf(" %s \n " , list ); 
	free(list);
	
	set_timeout(connsd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );
	
	return b ;
}


//  analizza la lista domini, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//  Parametri -> lista domini
int parsing_dom_list(char *list)
{

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

	char *string_query = "delete from Dom";
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



// 	Inserisci nel Db la dom list ricevuta, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri->la lista di server da inserire, un puntatore char per "restituire" l'eventuale errore
int insert_domlist_DB( char *list   , char* string_error)
{
	DB *db_server; // descrittore del DB Primary
	
	char *stringa_errore;
	int rc;

	int i =0 ;
	int count = 0;
	char *string_query = "insert into Dom values('";
	char *query;
	
	db_server = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );

	rc = open_db_rw( db_server , PATH_DB_DOMINI , stringa_errore);
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
			strncat(query , "')" ,2 * sizeof(char));
			
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
