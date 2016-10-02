#include "header.h"



// fase amministratore, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
int init_am_fase()
{

	int sockfd;

	char ip_fase[_DIM_IPV4];
	char port_fase_str[_DIM_IPV4];
	uint16_t port_fase;

	memset(ip_fase , '\0' , _DIM_IPV4 );
	memset(port_fase_str , '\0' , _DIM_PORT );
	memset(nick_am , '\0' , _DIM_NICK_PSW);
	l_nick_am = 0;
	
	printf("Inserisci l'IP del server: ");
	scanf("%s", ip_fase);
	printf("\n");

	printf("Inserisci la porta del server: ");
	scanf("%s", port_fase_str);
	printf("\n");	
	
	port_fase = atoi(port_fase_str);

	sockfd = connessioneClient(ip_fase , port_fase , tentativi_boot) ;
	if( sockfd <= 0 )
	{
		printf("Errore nella connessione \n");
		return 0;
	}

	 set_timeout(sockfd , timeout_am , 0 , timeout_am , 0 );	

	if(!login(sockfd) )
	{
		printf("LOGIN FALLITO !!!!!!!!!!! \n");
		exit(0);
	} 

	system("clear");
	printf("LOGIN RIUSCITO :) \n");
	stampa_comandi_am();

	operation_am(sockfd);

	return 1;
}



// fase login, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione
int login( int sockfd)
{

	int rc;
	BootAM * msg_login;
	
	char nick[_DIM_NICK_PSW];
	int dim_nick;
	char psw[_DIM_NICK_PSW];
	int dim_psw;

	printf("USERNAME: ");
	scanf("%s", nick);
	printf("\n");
	dim_nick = strlen(nick);

	printf("PASSWORD: ");
	scanf("%s", psw);
	printf("\n");
	dim_psw = strlen(psw);

	msg_login = init_boot_am( _BOOT_LOGIN_AM , nick , dim_nick , psw , dim_psw , NULL , 0);

	if( (rc = send(sockfd,msg_login,sizeof(BootAM ),MSG_NOSIGNAL) 	)   ==  0)
	{
		free(msg_login);
		return 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			free(msg_login);
			return 0;
		}
		else
		{
			perror("errore nella send: ");
			free(msg_login);
			return 0;
		}
	} 
	
	memset(msg_login , 0 , sizeof(BootAM) );
	
	if( (rc = recv(sockfd,msg_login,sizeof(BootAM ),0) 	)   ==  0)
	{
		free(msg_login);
		return 0;
	}
	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			free(msg_login);
			return 0;
		}
		else
		{
			perror("errore nella recv: ");
			free(msg_login);
			return 0;
		}
	}

	if(msg_login->pay_desc == _BOOT_COMMIT_AM )
	{
		free(msg_login);
		strcpy(nick_am , nick);
		l_nick_am = strlen(nick);
		return 1;
	}
	
	if(msg_login->pay_desc == _BOOT_ERROR_AM )
	{
		printf("%s \n" , msg_login->query);
		free(msg_login);
		return 0;
	}

	return 0;

}



// stampa i comandi amministratore
void stampa_comandi_am()
{
	printf("\n-------- COMANDI CONSOLE AMMINISTRATORE --------\n");
	printf(" %s.exit \033[m terminazione del server di Boot \n" , color); // 1
	printf(" %s.logout \033[m logout amministratore \n" , color); // 2
	printf(" %s.use \033[m seleziona il DB del server di Boot su cui operare \n" , color); // 3
	printf(" %s.chpsw \033[m cambia la psw amministratore \n" , color); // 4
	printf(" %s.select \033[m select sul DB del server Boot \n" , color); // 5
	printf(" %s.op \033[m update, insert, delete sul DB del server Boot \n" , color); // 6
	printf(" %s.help \033[m legenda dei comandi \n" , color); // 5
	printf("\n");
}



// operazioni amministratore, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione 
int operation_am(int sockfd)
{
	int b = 1;
	int scelta;
	char comando[_DIM_COM_CONSOLE_AM];	
	
	while(b)
	{
		memset( comando, '\0' , _DIM_COM_CONSOLE_AM );
		printf("Inserisci comando: ");
		scanf("%s", comando);
		while(getchar() != '\n'); // per svuotare l'stdin
		printf("\n");
		scelta = parsing_am(comando, _DIM_COM_CONSOLE_AM );

		switch (scelta) {
		
		case 1: {
			if(op_exit_am(sockfd))
			{
				return 1;
			}
			printf("Errore nel comamdo .exit \n");
			return 0;}

		case 2:{
			if(op_logout_am(sockfd))
			{
				return 1;
			}
			printf("Errore nel comamdo .logout \n");
			return 0;}

		case 3:
					{
						char string_db[50];
						int length;
			
						memset( string_db , '\0' , 50);			
						printf("Inserisci il nome del DB: ");
						scanf("%s", string_db);
						printf("\n");
						length = strlen(string_db);				

						if(!op_use_am(sockfd , string_db , length))
							printf("Errore nel comamdo .use \n");
					} break;
		
		case 4:
					{
						char old_psw[_DIM_NICK_PSW+1];
						char new_psw[_DIM_NICK_PSW+1];
			
						memset( old_psw , '\0' , _DIM_NICK_PSW+1);			
						printf("Inserisci la vecchia password: ");
						scanf("%s", old_psw);
						printf("\n");
						
						memset( new_psw , '\0' , _DIM_NICK_PSW+1);			
						printf("Inserisci la nuova password: ");
						scanf("%s", new_psw);
						printf("\n");				

						if(!op_chpsw_am(sockfd , old_psw , new_psw))
							printf("Errore nel comamdo .chpsw \n");
					} break;
	
		case 5:
					{
						char str_query[_DIM_QUERY_AM];
						int l_str_query;					
	
						memset( str_query , '\0' , _DIM_QUERY_AM);			
						printf("Inserisci la query di tipo select seguita dal carattere ';' \nquery:> ");

						scanf("%[^;]", str_query);
						while(getchar() != '\n'); // per svuotare l'stdin
						printf("\n");
						l_str_query =	strlen(str_query);
					
						if(!op_select_am(sockfd , str_query , l_str_query))
							printf("Errore nel comamdo .select \n");
					} break;

		case 6:
					{
						char str_query[_DIM_QUERY_AM];
						int l_str_query;					
	
						memset( str_query , '\0' , _DIM_QUERY_AM);			
						printf("Inserisci la query  seguita dal carattere ';' \nquery:> ");

						scanf("%[^;]", str_query);
						while(getchar() != '\n'); // per svuotare l'stdin
						printf("\n");
						l_str_query =	strlen(str_query);
					

						if(!op_geop_am(sockfd , str_query , l_str_query))
							printf("Errore nel comamdo .op \n");
					} break;
	
		case 7:
					{
						stampa_comandi_am();
					} break;

		default:
		printf("Comando errato !!!\n");
		break; }

	}
	return 1;
}



//  parsing dei comnadi dell'amministratore, ritorna: 0 se c'e' un errore, n !=0 che identifica il comando
//  Parametri -> comando, lunghezza comando 
int parsing_am(char *comando, int l_com )
{
	int b =  0;

	if(comando[0] == '.')
	{
		if(strncmp("exit" , &comando[1] , 4) == 0)
			return 1;
		
		if(strncmp("logout" , &comando[1] , 6) == 0 )
			return 2;
	
		if(strncmp("use" , &comando[1] , 3) == 0)
			return 3;

		if(strncmp("chpsw" , &comando[1] , 5) == 0)
			return 4;

		if(strncmp("select" , &comando[1] , 6) == 0 )
			return 5;
		
		if(strncmp("op" , &comando[1] , 2) == 0 )
			return 6;

		if(strncmp("help" , &comando[1] , 4) == 0 )
			return 7;
	}

	return b;
}



// implementazione comando .exit, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione 
int op_exit_am(int sockfd)
{	
	int rc;
	int b;
	BootAM *msg;

	msg = init_boot_am(_BOOT_EXIT_AM , NULL , 0 , NULL , 0 ,NULL , 0 );



	if( (rc = send(sockfd,msg,sizeof(BootAM ),MSG_NOSIGNAL) 	)   ==  0)
	{
		free(msg);
		return 0;
	}

	if( rc < 0)
	{
		if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
		{
			printf("Errore nel socket di connessione \n");
			exit(0);
		}

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
		memset(msg , 0 , sizeof(BootAM) );

		if( (rc = recv(sockfd,msg,sizeof(BootAM ),0) 	)   ==  0)
		{
			free(msg);
			return 0;
		}

		if( rc < 0)
		{
			if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK  || errno == EPIPE )
			{
				printf("Errore nel socket di connessione \n");
				exit(0);
			}

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
			if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
			{
				printf("Errore nel socket di connessione \n");
				exit(0);
			}

			if(msg->pay_desc == _BOOT_COMMIT_AM)
			{
				b = 1 ;
			}
			
			if(msg->pay_desc == _BOOT_ERROR_AM)
			{
				printf("%s \n", msg->query);
				b = 0 ;
			}
		}
	}
	
	free(msg);
	return b; 
}



// implementazione comando .logout, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione 
int op_logout_am(int sockfd)
{	
	int rc;
	int b;
	BootAM *msg;

	msg = init_boot_am(_BOOT_LOGOUT_AM , NULL , 0 , NULL , 0 ,NULL , 0 );



	if( (rc = send(sockfd,msg,sizeof(BootAM ),MSG_NOSIGNAL) 	)   ==  0)
	{
		free(msg);
		return 0;
	}

	if( rc < 0)
	{
		if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
		{
			printf("Errore nel socket di connessione \n");
			exit(0);
		}

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
		memset(msg , 0 , sizeof(BootAM) );

		if( (rc = recv(sockfd,msg,sizeof(BootAM ),0) 	)   ==  0)
		{
			free(msg);
			return 0;
		}

		if( rc < 0)
		{
			if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
			{
				printf("Errore nel socket di connessione \n");
				exit(0);
			}

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
			if(msg->pay_desc == _BOOT_COMMIT_AM)
			{
				b = 1 ;
			}
			
			if(msg->pay_desc == _BOOT_ERROR_AM)
			{
				printf("%s \n", msg->query);
				b = 0 ;
			}
		}
	}
	
	free(msg);
	return b; 
}



// implementazione comando .use, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione 
int op_use_am(int sockfd , char *string_db , int l)
{	
	int rc;
	int b;
	BootAM *msg;

	msg = init_boot_am(_BOOT_USE_DB_AM , NULL , 0 , NULL , 0 ,string_db , l );

	if( (rc = send(sockfd,msg,sizeof(BootAM ),MSG_NOSIGNAL) 	)   ==  0)
	{
		free(msg);
		return 0;
	}

	if( rc < 0)
	{
		if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
		{
			printf("Errore nel socket di connessione \n");
			exit(0);
		}

		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send \n");
			b = 0;
		}
	} 
	else
	{
		memset(msg , 0 , sizeof(BootAM) );

		if( (rc = recv(sockfd,msg,sizeof(BootAM ),0) 	)   ==  0)
		{
			free(msg);
			return 0;
		}

		if( rc < 0)
		{
			if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
			{
				printf("Errore nel socket di connessione \n");
				exit(0);
			}

			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				printf("timeout !!!!!!!\n");
				b = 0;
			}
			else
			{
				perror("errore nella recv \n ");
				b = 0;
			}
		} 
		else
		{
			if(msg->pay_desc == _BOOT_COMMIT_AM)
			{
				b = 1 ;
			}
			
			if(msg->pay_desc == _BOOT_ERROR_AM)
			{
				printf("errore: %s \n", msg->query);
				b = 0 ;
			}
		}
	}
	free(msg);
	return b; 
}



// implementazione comando .chpsw, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione, la vecchia password , la nuova password 
int op_chpsw_am(int sockfd , char *old_psw , char *new_psw)
{
	int rc;
	int b;
	BootAM *msg;

	msg = init_boot_am(_BOOT_CHANGE_PSW_AM , nick_am , l_nick_am , old_psw , strlen(old_psw) , new_psw , strlen(new_psw) );

	if( (rc = send(sockfd,msg,sizeof(BootAM ),MSG_NOSIGNAL) 	)   ==  0)
	{
		free(msg);
		return 0;
	}

	if( rc < 0)
	{
		if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
		{
			printf("Errore nel socket di connessione \n");
			exit(0);
		}

		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send \n");
			b = 0;
		}
	} 
	else
	{
		memset(msg , 0 , sizeof(BootAM) );

		if( (rc = recv(sockfd,msg,sizeof(BootAM ),0) 	)   ==  0)
		{
			free(msg);
			return 0;
		}

		if( rc < 0)
		{
			if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
			{
				printf("Errore nel socket di connessione \n");
				exit(0);
			}

			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				printf("timeout !!!!!!!\n");
				b = 0;
			}
			else
			{
				perror("errore nella recv \n ");
				b = 0;
			}
		} 
		else
		{
			if(msg->pay_desc == _BOOT_COMMIT_AM)
			{
				b = 1 ;
			}
			
			if(msg->pay_desc == _BOOT_ERROR_AM)
			{
				printf("errore: %s \n", msg->query);
				b = 0 ;
			}
		}
	}
	
	free(msg);
	return b; 
}



// implementazione comando .select, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione, la query di tipo select , la dimensione della query
int op_select_am(int sockfd , char *str_query , int l_str_query)
{
	int rc;
	int b;
	BootAM *msg;

	msg = init_boot_am(_BOOT_SELECT_AM , NULL , 0 , NULL , 0 , str_query , l_str_query);

	if( (rc = send(sockfd,msg,sizeof(BootAM ),MSG_NOSIGNAL) 	)   ==  0)
	{
		free(msg);
		return 0;
	}

	if( rc < 0)
	{
		if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
		{
			printf("Errore nel socket di connessione \n");
			exit(0);
		}

		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send \n");
			b = 0;
		}
	} 
	else
	{
		char *risultato;
		risultato = writeFile_on_buffer_noblock(sockfd, max_line);
		printf("Risultato Query:\n");
		printf("%s\n", risultato);
		free(risultato); 
	}
	
	free(msg);
	return b; 
}



// implementazione comando .op, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri -> descrittore del socket di connessione, la query di tipo select , la dimensione della query
int op_geop_am(int sockfd , char *str_query , int l_str_query)
{
	int rc;
	int b;
	BootAM *msg;

	msg = init_boot_am(_BOOT_GE_OP_AM , NULL , 0 , NULL , 0 , str_query , l_str_query);

	if( (rc = send(sockfd,msg,sizeof(BootAM ),MSG_NOSIGNAL) 	)   ==  0)
	{
		free(msg);
		return 0;
	}

	if( rc < 0)
	{
		if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
		{
			printf("Errore nel socket di connessione \n");
			exit(0);
		}

		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send \n");
			b = 0;
		}
	} 
	else
	{
		memset(msg , 0 , sizeof(BootAM) );

		if( (rc = recv(sockfd,msg,sizeof(BootAM ),0) 	)   ==  0)
		{
			free(msg);
			return 0;
		}

		if( rc < 0)
		{
			if( errno == EBADF || errno == ECONNRESET || errno == ENOTCONN  || errno == ENOTSOCK || errno == EPIPE )
			{
				printf("Errore nel socket di connessione \n");
				exit(0);
			}

			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				printf("timeout !!!!!!!\n");
				b = 0;
			}
			else
			{
				perror("errore nella recv \n ");
				b = 0;
			}
		} 
		else
		{
			if(msg->pay_desc == _BOOT_COMMIT_AM)
			{
				b = 1 ;
			}
			
			if(msg->pay_desc == _BOOT_ERROR_AM)
			{
				printf("errore: %s \n", msg->query);
				b = 0 ;
			}
		}
	}
	
	free(msg);
	return b; 

}
