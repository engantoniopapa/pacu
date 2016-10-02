#include "header.h"



//	porzione di codice del dispatcher che rimane in ascolto su una porta 
void *func_dispatcher()
{
	int listensd ;
  int t ;	
	
	listensd = bootServer(port_client , backlog_client);

	printf("Creating disp_DNS %lu\n", (unsigned long)pthread_self());		
	pthread_t threads[thread_helper];

	

	for(t=0;t < thread_helper;t++)
	{
		int rc ;

  	rc = pthread_create (&threads[t], NULL, func_helper, (void *)&listensd);
  	if (rc)
		{
     	printf("ERROR; return code from pthread_create() is %d\n",rc);
     	exit(-1);
  	}
	}



	sem_wait(sem_exit_disp);

		exit_helper = 1;
	

	sleep(time_helper);
	for(t=0;t < thread_helper;t++)
	{	
  		pthread_cancel(threads[t] );
	}			
	close(listensd);

	pthread_exit(NULL);

}



//	porzione di codice degli helper che si contendono le richieste di connessione client
void *func_helper(void *l)
{	
	int listensd = * (int *)l ;
	int connsd;
	BootMessage *buffer_ping;
	buffer_ping = (BootMessage *) malloc (sizeof(BootMessage));

	printf("Creating thread_DNS %lu\n", (unsigned long)pthread_self());	

	for ( ; ; ) 
	{

		if(exit_helper)
		{
				pthread_exit(NULL);	
		}

		sem_wait(sem_accept);

			connsd = connessioneServer(listensd);

		sem_post(sem_accept);	

		if( recv_ping(connsd, buffer_ping) )
		{
			if( buffer_ping->pay_desc == _BOOT_PING)
			{
				double temp_v_serv;					

				if(send_pong(connsd , &temp_v_serv))
				{
					if(temp_v_serv == buffer_ping->versione_primay)
					{;}
					else
					{
						if(wait_req_client(connsd))
						{
							send_server_list(connsd , list_primary);
						}
					}
				}	
				else
				{
					send_error(connsd);
				}					
				
			}
			else
			{
				printf("messaggio non formattato\n");
				send_error(connsd);
			}

		}
		printf("close\n");
		close(connsd);
	}

	free(buffer_ping);
	pthread_exit(NULL);
}



// porzione di codice del thread responsabile della connessione con l'amministratore
void *func_thread_am()
{
	int listensd ;
	int connsd;
	short int uscita = 0; // terminazione sever di Boot
	
	listensd = bootServer(port_am , 1);
	printf("Creating thread AM %lu\n", (unsigned long)pthread_self());		

	DB *db_conf; // descrittore del DB Conf
	DB *temp_db; // descrittore DB usato dall'amministratore
	int rc;
	char *psw;
	char *stringa_errore;
	BootAM *buffer;

	psw = (char *) calloc (  _DIM_MD5 , sizeof(char) );
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char));
	db_conf = ( DB *) malloc (sizeof(DB) ) ;	
	temp_db = ( DB *) malloc (sizeof(DB) ) ;	
	buffer = (BootAM *) calloc ( 1 , sizeof(BootAM) );

	rc = open_db_rw( db_conf , PATH_DB_CONF , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	while(!uscita)
	{
		connsd = connessioneServer(listensd);

		set_timeout(connsd , timeout_am , 0 , timeout_am , 0);

		if( control_login(connsd , db_conf , buffer) )
		{
			printf("Login effettuato con successo \n");
			short int logout = 0 ;
			short int use_temp_db = 0;

			while(!logout)
			{
				memset(buffer, 0 , sizeof(BootAM) ); 
				rc = recv(connsd,buffer,sizeof(BootAM),0);

				if( rc <= 0)
				{
					char *str_error = "Errore in ricezione, sulla porta amministratore";
					printf("%s \n" , str_error);
					if(errno == EAGAIN || errno == EWOULDBLOCK)
					{;}
					else
					{
						send_error_am(connsd , str_error);
					}

					logout = 1;
				}	

				else			
				{	
						int bizantino = 1;
						if( buffer->pay_desc == _BOOT_EXIT_AM )
						{	
							if(send_commit_am(connsd))
							{
								bizantino = 0;
								logout = 1;
								uscita = 1; 
							}
						}

						if( buffer->pay_desc == _BOOT_LOGOUT_AM )
						{
							if(send_commit_am(connsd))
							{
								bizantino = 0;
								logout = 1;
							}
						}

						if(buffer->pay_desc == _BOOT_USE_DB_AM)
						{	
								bizantino = 0;
							
								if( scelta_db(temp_db , buffer ,stringa_errore)  )
								{
									use_temp_db = 1;
									send_commit_am(connsd);
								}
								else
								{	
									send_error_am(connsd , stringa_errore );
								}
						}

						if(buffer->pay_desc == _BOOT_CHANGE_PSW_AM)
						{
							bizantino = 0;

							if(open_db_rw( temp_db , PATH_DB_CONF , stringa_errore) )
							{
								use_temp_db = 1;
								char *str_query;
								int count = 0;
								char new_password_md5[_DIM_MD5];
								char old_password_md5[_DIM_MD5];
								char *temp_query_1 = "update Accesso set psw = '";
								char *temp_query_2 = " where nick = '";
								char *temp_query_3 = " and psw = '";  
								
								create_password( new_password_md5 , buffer->query , buffer->dim_query);
								create_password( old_password_md5 , buffer->psw , buffer->dim_psw);

								count =  strlen(temp_query_1) + strlen(temp_query_2) + strlen(temp_query_3) + _DIM_MD5 + 40 + buffer->dim_nick + _DIM_MD5;
								str_query = (char*) calloc (count , sizeof(char) );
								strcpy(str_query , temp_query_1);
								strncat(str_query , new_password_md5 , _DIM_MD5);
								strcat(str_query , "'");
								strcat(str_query , temp_query_2);
								strncat(str_query, buffer->nick , buffer->dim_nick);
								strcat(str_query , "'");
								strcat(str_query , temp_query_3);
								strncat(str_query, old_password_md5 ,_DIM_MD5);
								strcat(str_query , "'");

								if(generic_op_db(temp_db ,  str_query , stringa_errore) )
								{	
									send_commit_am(connsd);
								}
								else
								{
									send_error_am(connsd , stringa_errore );
								}			
							}
							else
							{	
								send_error_am(connsd , stringa_errore );
							}
						}

						if( buffer->pay_desc == _BOOT_SELECT_AM )
						{
							bizantino = 0;
							char *str_select;
							char *string_query;
							int no_error ;
							if(use_temp_db)
							{
									string_query = (char*) calloc ( buffer->dim_query+1 , sizeof(char) );
									strcpy(string_query , buffer->query  );
									str_select = select_db_am(temp_db , string_query , stringa_errore , &no_error ); 
							}
							else
							{
								no_error = 0 ;
								memset(stringa_errore , '\0' , _MAX_STRING_ERROR);
								strcpy(stringa_errore , "DB non selezionato, utilizza il comndo .use per selezionarlo \n");
							}

							if(no_error)
							{
								send_result_query(connsd , str_select);
							}
							else
							{	
								send_result_query(connsd , stringa_errore);
							}

							if(use_temp_db)
							{
								free(string_query);
								free(str_select);
							}
						}

						if( buffer->pay_desc == _BOOT_GE_OP_AM )
						{	
							bizantino = 0;
							char *string_query;
							int no_error ;

							if(use_temp_db)
							{
								string_query = (char*) calloc ( buffer->dim_query+1 , sizeof(char) );
								memcpy(string_query , buffer->query , buffer->dim_query * sizeof(char) );
								string_query[buffer->dim_query] ='\0';
								no_error = generic_op_db(temp_db , string_query , stringa_errore);
							}
							else
							{
								no_error = 0 ;
								memset(stringa_errore , '\0' , _MAX_STRING_ERROR);
								strcpy(stringa_errore , "DB non selezionato, utilizza il comndo .op per selezionarlo \n");
							}

							if(no_error)
							{
								send_commit_am(connsd);
							}
							else
							{
								send_error_am(connsd , stringa_errore);
							}

							if(use_temp_db)
							{
								free(string_query);
							}
						}

						if(bizantino)
						{
							send_error_am(connsd , "Errore messaggio non formattato ");
						}
				}
			}

			if(use_temp_db)
			{
				rc = close_db(temp_db, stringa_errore);
 			  if( !rc )
				{
    			fprintf(stderr, "%s\n", stringa_errore);
    			exit(1);
  			}
			}
		}
		
		else
		{
			send_error_am(connsd , "Errore nel login");
		}
		close(connsd);
	}


	rc = close_db(db_conf, stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(stringa_errore);
	free(db_conf);
	free(psw);
	free(buffer);

	close(listensd);

	pthread_exit(NULL);
}



//  esegue i vari controlli per il login amministratore, restituisce: 0 per login non corretto, 1 se l'operazione va a buon fine
//	Parametri -> descrittore della connessione, messaggio di login
int control_login(int connsd ,  DB *db_conf , BootAM *buffer)
{
	if(recv_login(connsd, buffer) )
	{
		if(buffer->pay_desc == _BOOT_LOGIN_AM )
		{
			if( check_psw(db_conf, buffer->nick, buffer->dim_nick, buffer->psw, buffer->dim_psw) )
			{
				if(send_commit_am(connsd))
				{
					return 1;
				}
			}
		}
	}
	return 0;
}
