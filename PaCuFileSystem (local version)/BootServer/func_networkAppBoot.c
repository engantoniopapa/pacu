#include "header.h"


//	Inizializza un messaggio di tipo BootMessage, ritorna: il puntatore al messaggio creato
//	Parametri-> decsrittore di payload del messaggio
BootMessage* init_bootmessage( unsigned char c)
{
	BootMessage *b;
	b = (BootMessage *) malloc( sizeof(BootMessage) );

	b->pay_desc = c ;
	
	if( c == _BOOT_PING )
	{
		b->versione_dominio = 0.0 ;
		b->versione_primay = 0.0 ;
	}

	if( c == _BOOT_PONG )
	{
		b->versione_dominio = version_dom ;
		b->versione_primay = version_serv ;
	}

	if( c == _BOOT_LIST)
	{
		b->versione_dominio = version_dom ;
		b->versione_primay = version_serv ;
	}
	
	if( c == _BOOT_ERROR )
	{
		b->versione_dominio = 0.0 ;
		b->versione_primay = 0.0 ;
	}
	return b;
}



//	Inizializza un messaggio di tipo BootAm, ritorna: il puntatore al messaggio creato
//	Parametri-> decsrittore di payload del messaggio
BootAM* init_boot_am( unsigned char c)
{
	BootAM *b;
	b = (BootAM *) calloc( 1, sizeof(BootAM) );

	b->pay_desc = c ;
	
	if( c == _BOOT_LOGIN_AM )
	{;}

	if( c == _BOOT_EXIT_AM )
	{;}

	if( c == _BOOT_USE_DB_AM)
	{;}
	
	if( c == _BOOT_SELECT_AM )
	{;}

	if( c == _BOOT_GE_OP_AM )
	{;}
	
	if( c == _BOOT_ERROR_AM )
	{
		b->dim_nick = 0;
		b->dim_psw = 0;
		b->dim_query = 0;
	}
	
	if( c == _BOOT_CHANGE_PSW_AM )
	{;}

	if( c == _BOOT_LOGOUT_AM )
	{;}

	if( c == _BOOT_COMMIT_AM )
	{
		b->dim_nick = 0;
		b->dim_psw = 0;
		b->dim_query = 0;
		memset( b->nick , 0 , _DIM_NICK_PSW);
		memset( b->psw , 0 , _DIM_NICK_PSW);
		memset( b->query , 0 , _DIM_QUERY_AM); 
	}

	return b;
}


// Bootstrap della connessione lato server, ritorna: il descrittore socket per mettersi in ascolto sulla porta
//	Parametri-> puntatore alla porta , e lunghezza coda di backlog
int bootServer( in_port_t *port , int backlog) 
{
	int listensd;
	struct sockaddr_in servaddr;
	
	if((listensd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("errore in socket");
		exit(-1);
	}

	memset((void*)&servaddr,0,sizeof(servaddr));

	addr_init(&servaddr, port ,INADDR_ANY);

	if(bind(listensd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		perror("errore in bind");
		exit(-1);
	}
	
	puts("bind effettuato con successo");
	
	if(listen(listensd,backlog)<0)
	{
		perror("errore in listen");
		exit(-1);
	}

	puts("listen effettuata con successo");
	return listensd;
}



//	Ricezione messaggio di Bootping, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al messaggio da ricevere
int  recv_ping( int connsd , BootMessage * buffer_ping )
{
	int rc;
	memset(buffer_ping, 0 , sizeof(BootMessage ) );

	set_timeout(connsd  , timeout_ping  , 0  , timeout_ping  , 0 );

	if (   (rc = recv(connsd,buffer_ping,sizeof(BootMessage),0) )   ==  0)
	{
		return 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			return 0;
		}
		else
		{
			perror("errore nella recv: ");
			return 0;
		}
	} 
	return 1;
}



//	Invio messaggio di Bootpong, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al parametro version_serv
int send_pong(int connsd , double *serv)
{
	int rc;
	BootMessage * msg;
	msg = init_bootmessage(_BOOT_PONG) ;
	int b;
	
	set_timeout(connsd  , timeout_pong  , 0  , timeout_pong , 0 );

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
		*serv = msg->versione_primay ;
	}
	
	free(msg); 
	return b;
}



//	Invio messaggio di errore
//	Parametri-> descrittore della connessione
void send_error(int connsd)
{
	int rc;
	BootMessage * msg_error;
	msg_error = init_bootmessage(_BOOT_ERROR) ;
	set_timeout(connsd  , timeout_error  , 0  , timeout_error , 0 );
	rc = send(connsd,msg_error,sizeof(BootMessage ),MSG_NOSIGNAL) ;
	free(msg_error);
}



//	Ricezione messaggio di BootstrapListPrimary, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al parametro version_serv
int wait_req_client(int connsd)
{
	int rc;
	int b;
	BootMessage * msg;
	msg = (BootMessage *) malloc (sizeof(BootMessage)) ;

	set_timeout(connsd  , timeout_req_list , 0  , timeout_req_list , 0 );

	if( (rc = recv(connsd,msg,sizeof(BootMessage ),0) 	)   ==  0)
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
	else
	{
		if( msg->pay_desc == _BOOT_LIST)
		{
			b = 1 ;
		}
		else
		{
			b = 0;
		}
	}

	free(msg); 
	return b;
}



//	Invio lista PrimaryServer
//	Parametri-> descrittore della connessione, stringa contenente la lista Primary da inviare al client
int send_server_list(int connsd , char *list)
{
	int rc;

	set_timeout(connsd  , timeout_send_servlist , 0  , timeout_send_servlist , 0 );
	rc = writeBuffer_on_file(connsd , max_line, list) ;


	if( rc <= 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			return 0;
		}
		else
		{
			perror("errore nella send: ");
			return 0;
		}
	} 
	else
	{
		return 1;
	}
}



//	Invio risultato select (Amministratore)
//	Parametri-> descrittore della connessione, stringa contenente il risultato da inviare all'amministratore
int send_result_query(int connsd , char *list)
{
	int rc;

	rc = writeBuffer_on_file(connsd, max_line , list);

	if( rc <= 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			return 0;
		}
		else
		{
			perror("errore nella send: ");
			return 0;
		}
	} 
	else
	{
		return 1;
	}
}



//	Ricezione messaggio di BootLogin, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al messaggio da ricevere
int  recv_login( int connsd , BootAM * buffer )
{
	int rc;
	memset(buffer, 0 , sizeof(BootAM ) );

	if (   (rc = recv(connsd,buffer,sizeof(BootAM),0) )   ==  0)
	{
		return 0 ;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			return 0;
		}
		else
		{
			perror("errore nella recv: ");
			return 0;
		}
	} 
	return 1;
}



//	Verifica password per il login (Amministratore), restituisce: 0 login errato 1 login effettuato con successo
//	Parametri->	descrittore del db, password dell'utente, lunghezza password, nickname dell'utente, lunghezza nickname
int check_psw(DB *db ,char* nick , int dim_nick ,char *psw , int dim_psw )
{
	char hash[_DIM_MD5]; // hash password da calcolare
	char psw_am[_DIM_MD5]; // hash password del db
	int b = 0;

	if( (b = select_password_am(db , nick , psw_am , _DIM_MD5 )) ==  1 )
	{
		  create_password(hash , psw , dim_psw);

			if( memcmp( hash , psw_am , _DIM_MD5 ) == 0 )
			{		
				b = 1 ;
			}
			else
			{			
				b = 0;
			}
	}
	return b;
}



//	invia un messaggio di BootCommit (Amministratore), restituisce: 1 se l'operazione è riuscita, 0 se l'operazione non è riuscita
//	Parametri-> descrittore della connessione socket
int send_commit_am( int connsd)
{
	int rc;
	BootAM * msg;
	msg =  init_boot_am(_BOOT_COMMIT_AM) ;
	int b;

	if( (rc = send(connsd,msg,sizeof(BootAM),MSG_NOSIGNAL) 	)   ==  0)
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
			perror("errore nella send commit: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	
	free(msg); 
	return b;	
}



//	Invio messaggio di errore (Amministratore), ritorna: 1 se l'operazione è riuscita, 0 se l'operazione è non riuscita
//	Parametri-> descrittore della connessione
int send_error_am(int connsd , char *str_error)
{
	int rc;
	BootAM * msg_error;
	msg_error = init_boot_am(_BOOT_ERROR_AM);
	int b = 0;

	strncpy(msg_error->query , str_error , _DIM_QUERY_AM);
	rc = send(connsd,msg_error,sizeof(BootAM),MSG_NOSIGNAL);

	

	if( rc <= 0)
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
	free(msg_error);
	return rc;
}
