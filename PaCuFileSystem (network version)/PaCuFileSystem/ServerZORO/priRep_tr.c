#include "header.h"


// operazioni di _TR_UPLOAD, _TR_NEW_GA, _TR_NEW_CAP, _TR_MV_CAP, _TR_RM_CAP 
int op_rep_tr1(InitMessage *msg_i , unsigned char type , Server *list_vista , int len_vista , int fd , 	Id_Trans *id_t )
{
	short int stato_op = 1;
	int t;
	pthread_t threads[len_vista];

	ContainerTr1 container[len_vista];
	
	for(t=0;t < len_vista;t++)
	{
		container[t].msg_i = msg_i;
		container[t].id_t = id_t;
		container[t].type = type;
		memset(container[t].ip , '\0' , _DIM_IP);
		strncpy(container[t].ip , list_vista[t].ip ,_DIM_IP-1);	
		container[t].fd = fd;		
		container[t].consd = -1;	
		container[t].state = -1;
		
		int rc ;
	  rc = pthread_create (&threads[t], NULL,th_rep_tr1 , (void *)&(container[t]) );
	  
	  if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
	  }
	}

	for(t=0;t < len_vista ;t++)
	{	
  		pthread_join(threads[t], NULL );
	}			

	for(t=0;t < len_vista ;t++)
	{	
  	if(container[t].state == -1 || container[t].state == 0)
  	{
  		stato_op = 0;
  		break;
  	}
	}		

	if(stato_op == 0)
	{
		for(t=0;t < len_vista ;t++)
		{	
			threads[t]= -1;
  		if(container[t].state == 1)
  			container[t].state = 0;
		}
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
	}
	else
	{
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_COMMIT);
	}
	
	for(t=0;t < len_vista;t++)
	{
		int rc ;
	  rc = pthread_create (&threads[t], NULL,th_rep_send_response1 , (void *)&(container[t]));
	  
	  if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
	  }
	}
	
	for(t=0;t < len_vista ;t++)
	{	
  		pthread_join(threads[t], NULL );
  		
  		if( container->consd > 0)
  			close(container[t].consd);
	}	

	return stato_op;
}




// operazioni di _TR_RM, _TR_MKDIR, _TR_RM_DIR
int op_rep_tr2(InitMessage *msg_i , unsigned char type , Server *list_vista , int len_vista, Id_Trans *id_t)
{
	short int stato_op = 1;
	int t;
	pthread_t threads[len_vista];
	ContainerTr2 container[len_vista];

	
	for(t=0;t < len_vista;t++)
	{
		container[t].msg_i = msg_i;
		container[t].id_t = id_t;
		container[t].type = type;
		memset(container[t].ip , '\0' , _DIM_IP);
		strncpy(container[t].ip , list_vista[t].ip ,_DIM_IP-1);	
		container[t].consd = -1;	
		container[t].state = -1;
		
		int rc ;
	  rc = pthread_create (&threads[t], NULL,th_rep_tr2 , (void *)&(container[t]));
	  
	  if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
	  }
	}

	for(t=0;t < len_vista ;t++)
	{	
  		pthread_join(threads[t], NULL );
	}			

	for(t=0;t < len_vista ;t++)
	{	
  	if(container[t].state == -1 || container[t].state == 0)
  	{
  		stato_op = 0;
  		break;
  	}
	}		

	if(stato_op == 0)
	{
		for(t=0;t < len_vista ;t++)
		{	
			threads[t]= -1;
  		if(container[t].state == 1)
  			container[t].state = 0;
		}
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
	}
	else
	{
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_COMMIT);
	}
	
	for(t=0;t < len_vista;t++)
	{
		int rc ;
	  rc = pthread_create (&threads[t], NULL,th_rep_send_response2 , (void *)&(container[t]));
	  
	  if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
	  }
	}
	
	for(t=0;t < len_vista ;t++)
	{	
  		pthread_join(threads[t], NULL );
  		
  		if( container->consd > 0)
  			close(container[t].consd);
	}	

	return stato_op;
}


// operazioni di _TR_MOD_CAP
int op_rep_tr3(InitMessage *msg_i ,unsigned char type , Server *list_vista , int len_vista , List_ModCap* list_cap , int len_cap ,	Id_Trans *id_t)
{
	short int stato_op = 1;
	int t;
	pthread_t threads[len_vista];
	ContainerTr3 container[len_vista];
	
	for(t=0;t < len_vista;t++)
	{
		container[t].msg_i = msg_i;
		container[t].id_t = id_t;
		container[t].type = type;
		memset(container[t].ip , '\0' , _DIM_IP);
		strncpy(container[t].ip , list_vista[t].ip ,_DIM_IP-1);
		container[t].list_cap = list_cap;			
		container[t].len_cap = len_cap;
		container[t].consd = -1;	
		container[t].state = -1;
		
		int rc ;
	  rc = pthread_create (&threads[t], NULL,th_rep_tr3 , (void *)&(container[t]));
	  
	  if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
	  }
	}

	for(t=0;t < len_vista ;t++)
	{	
  		pthread_join(threads[t], NULL );
	}			


	for(t=0;t < len_vista ;t++)
	{	
  	if(container[t].state == -1 || container[t].state == 0)
  	{
  		stato_op = 0;
  		break;
  	}
	}		

	if(stato_op == 0)
	{
		for(t=0;t < len_vista ;t++)
		{	
			threads[t]= -1;
  		if(container[t].state == 1)
  			container[t].state = 0;
		}
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
	}
	else
	{
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_COMMIT);
	}
	
	
	for(t=0;t < len_vista;t++)
	{
		int rc ;
	  rc = pthread_create (&threads[t], NULL,th_rep_send_response3 , (void *)&(container[t]));
	  
	  if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
	  }
	}
	
	for(t=0;t < len_vista ;t++)
	{	
  		pthread_join(threads[t], NULL );
  		
  		if( container->consd > 0)
  			close(container[t].consd);
	}	

	return stato_op;
}




void *th_rep_tr1(void *l)
{
	ContainerTr1 *container = (ContainerTr1*) l;
	printf("Thred replica 1 \n" );
	
	container->consd = connect_ServerServer(container->ip , ntohs(*port_server) , 2) ;
	
	if( container->consd > 0)
	{	
		set_timeout(container->consd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );	
		
		if(send_InitMessage( container->msg_i , container->consd ))
		{	
			DiagMessage *msgD;
			msgD = (DiagMessage*) calloc (  1 , sizeof(DiagMessage)  );
			
			if(recv_DiagMessage(container->consd , msgD) )
			{
				if(msgD->pay_desc == _DIAG_ACK)
				{
					if(opRep_upload( container->consd , container->ip , container->id_t , container->fd ,  ntohs(*port_server)  ) )
						container->state = 1;
					else
						container->state = 0;
				}
			
				if(msgD->pay_desc == _DIAG_ERROR)
				{
					container->state = 0;
				}
			}
			else	
			{
				container->state = 0;
			}
			free(msgD);
		}
		else
		{
		 container->state = 0;
		}
	}
	else
	{
	 container->state = -1;
	 container->consd = -1;
	}

	pthread_exit(NULL);
}



void *th_rep_tr2(void *l)
{
	ContainerTr2 *container = (ContainerTr2*) l;
	printf("Thred replica 2 \n");
	
	container->consd = connect_ServerServer(container->ip , ntohs(*port_server) , 2) ;
	
	if( container->consd > 0)
	{	
		set_timeout(container->consd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );	
		
		if(send_InitMessage( container->msg_i , container->consd ))
		{
			DiagMessage *msgD;
			msgD = (DiagMessage*) calloc (  1 , sizeof(DiagMessage)  );
			
			if(recv_DiagMessage(container->consd , msgD) )
			{
				if(msgD->pay_desc == _DIAG_ACK)
				{
					if(opRep_generic( container->consd , container->type ,container->ip , container->id_t ,  ntohs(*port_server)  ) )
						container->state = 1;
					else
						container->state = 0;
				}
			
				if(msgD->pay_desc == _DIAG_ERROR)
				{
					container->state = 0;
				}
			}
			else	
			{
		 		container->state = 0;
			}
			free(msgD);
		}
		else
		{
		 container->state = 0;
		}
	}
	else
	{
	 container->state = -1;
	 container->consd = -1;
	}
	
	pthread_exit(NULL);
}



void *th_rep_tr3(void *l)
{
	ContainerTr3 *container = (ContainerTr3*) l;
	printf("Thred replica 1 \n");
	
	container->consd = connect_ServerServer(container->ip , ntohs(*port_server) , 2) ;
	
	if( container->consd > 0)
	{	
		set_timeout(container->consd , timeout_init_msg  , 0  , timeout_init_msg  , 0 );	
		
		if(send_InitMessage( container->msg_i , container->consd ))
		{
			DiagMessage *msgD;
			msgD = (DiagMessage*) calloc (  1 , sizeof(DiagMessage)  );
			
			if(recv_DiagMessage(container->consd , msgD) )
			{
				if(msgD->pay_desc == _DIAG_ACK)
				{
					if(opRep_modcap( container->consd , container->ip , container->id_t , container->list_cap , container->len_cap,  ntohs(*port_server)  ) )
						container->state = 1;
					else
						container->state = 0;
				}
			
				if(msgD->pay_desc == _DIAG_ERROR)
				{
					container->state = 0;
				}
			}
			else	
			{
		 		container->state = 0;
			}
			free(msgD);
		}
		else
		{
		 container->state = 0;
		}
	}
	else
	{
	  container->state = -1;
	  container->consd = -1;
	}
	
	pthread_exit(NULL);
}



// thread preposto all'invio di un commit o di un abort alla replica
void *th_rep_send_response1(void *l)
{
	ContainerTr1 *container = (ContainerTr1*) l;
	printf("Thred response \n");
	
	if(container->state != -1)
	{
		if(container->state == 1)
		{
				TrMessage* msg;
				msg =  init_transaction_msg(_TR_COMMIT , container->id_t->id_p , container->id_t->id_c , 0 ,  0 , 0);
				send_TrMessage(container->consd , msg);
				free(msg);
		}
		else
		{
				TrMessage* msg;
				msg =  init_transaction_msg(_TR_ABORT , container->id_t->id_p , container->id_t->id_c , 0 ,  0 , 0);
				send_TrMessage(container->consd , msg);
				free(msg);
		}
	}
	
	pthread_exit(NULL);
}


// thread preposto all'invio di un commit o di un abort alla replica
void *th_rep_send_response2(void *l)
{
	ContainerTr2 *container = (ContainerTr2*) l;
	printf("Thred response \n");
	
	if(container->state != -1)
	{
		if(container->state == 1)
		{
				TrMessage* msg;
				msg =  init_transaction_msg(_TR_COMMIT , container->id_t->id_p , container->id_t->id_c , 0 ,  0 , 0);
				send_TrMessage(container->consd , msg);
				free(msg);
		}
		else
		{
				TrMessage* msg;
				msg =  init_transaction_msg(_TR_ABORT , container->id_t->id_p , container->id_t->id_c , 0 ,  0 , 0);
				send_TrMessage(container->consd , msg);
				free(msg);
		}
	}
	
	pthread_exit(NULL);
}


// thread preposto all'invio di un commit o di un abort alla replica
void *th_rep_send_response3(void *l)
{
	ContainerTr3 *container = (ContainerTr3*) l;
	printf("Thred response \n");
	
	if(container->state != -1)
	{
		if(container->state == 1)
		{
				TrMessage* msg;
				msg =  init_transaction_msg(_TR_COMMIT , container->id_t->id_p , container->id_t->id_c , 0 ,  0 , 0);
				send_TrMessage(container->consd , msg);
				free(msg);
		}
		else
		{
				TrMessage* msg;
				msg =  init_transaction_msg(_TR_ABORT , container->id_t->id_p , container->id_t->id_c , 0 ,  0 , 0);
				send_TrMessage(container->consd , msg);
				free(msg);
		}
	}
	
	pthread_exit(NULL);
}



// operazione per invio di una risorsa su una replica , ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, descrittore della risorsa , indirizzo e porta remoti
int opRep_upload(int sockfd , char *remote_ip , Id_Trans *Id_t , int fd  , int port)
{
	set_timeout(sockfd  , timeout_tr  , 0  , timeout_tr , 0 );	
	TrMessage *msg;
	short int rc;
	
	struct stat buf;
	
	if(fstat(fd, &buf) == -1)
	{
		printf("\e[1;31mErrore nell'analisi del file da inviare \033[m \n"); 
		return 0;
	}
	
	unsigned int size_file ;
	size_file = (unsigned int) buf.st_size;

	msg =  init_transaction_msg(_TR_UPLOAD , Id_t->id_p , Id_t->id_c , 0 ,  0 , size_file);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		printf("\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	if(write_res_on_res(fd , sockfd ,  max_line) )
	{;}
	else
	{
		printf("\e[1;31mErrore nell'invio del file \033[m \n"); //send error
		return 0;
	}			

	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , 0 ,  0 , 0);
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		printf( "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}

	while(1)
	{	
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
				
		if(rc == 1)
			break;
			
		if(rc == 0 )
		{
			if( isAlive_pp(listServ_pp, sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , port ) <= 0 )
			{
				printf( "\e[1;31mErrore la replica non è + attiva \033[m \n"); //send error
				free(msg);
				return 0;
			}
		}

		if(rc == -1)
		{
			printf("\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}

	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		return 1;
	}
	else
	{
			free(msg);
			return 0;
	}
		
	return 0;
}



					
// operazione per l'invio di _TR_RM, _TR_MKDIR, _TR_RM_DIR su una replica , ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, descrittore della risorsa , indirizzo e porta remoti
int opRep_generic( int sockfd , unsigned char type , char *remote_ip , Id_Trans *Id_t  , int port)
{
	set_timeout(sockfd  , timeout_tr  , 0  , timeout_tr , 0 );	
	TrMessage *msg;
	short int rc;
	

	msg =  init_transaction_msg(type , Id_t->id_p , Id_t->id_c , 0 ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		printf("\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , 0 ,  0 , 0);
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		printf( "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}

	while(1)
	{	
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
				
		if(rc == 1)
			break;
			
		if(rc == 0 )
		{
			if( isAlive_pp(listServ_pp, sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , port ) <= 0 )
			{
				printf( "\e[1;31mErrore la replica non è + attiva \033[m \n"); //send error
				free(msg);
				return 0;
			}
		}
		if(rc == -1)
		{
			printf("\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}

	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		return 1;
	}
	else
	{
			free(msg);
			return 0;
	}
		
	return 0;
}



// operazione per l'invio di TR_MODCAP  su una replica , ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, descrittore della risorsa , indirizzo e porta remoti
int opRep_modcap( int sockfd  , char *remote_ip , Id_Trans *Id_t  , List_ModCap* list_cap , int len_cap , int port)
{
	set_timeout(sockfd  , timeout_tr  , 0  , timeout_tr , 0 );	
	TrMessage *msg;
	short int rc;
	msg =  init_transaction_msg(_TR_UPLOAD , Id_t->id_p , Id_t->id_c , 0 ,  0 , 0);
	int i ;
	
	for( i = 0 ; i < len_cap ; ++i)
	{
		int fd = open( list_cap[i].path_mod_cap , O_RDONLY);	
		struct stat buf;
	
		if(fstat(fd, &buf) == -1)
		{
			printf("\e[1;31mErrore nell'analisi del file da inviare \033[m \n"); 
			close(fd);
			return 0;
		}
		
		unsigned int size_file ;
		size_file = (unsigned int) buf.st_size;
		
		set_transaction_msg(msg , _TR_MOD_CAP , Id_t->id_p , Id_t->id_c , 0 ,  list_cap[i].id_cap , size_file);
		
		rc = send_TrMessage(sockfd , msg);
	
		if( rc <= 0)
		{
			printf("\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
			free(msg);
			close(fd);
			return 0;
		}
	
		if(write_res_on_res(fd , sockfd ,  max_line) )
		{;}
		else
		{
			printf("\e[1;31mErrore nell'invio del file \033[m \n"); //send error
			close(fd);
			return 0;
		}
		
		close(fd);
	}

	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , 0 ,  0 , 0);
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		printf( "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}

	while(1)
	{	
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
				
		if(rc == 1)
			break;
			
		if(rc == 0 )
		{
			if( isAlive_pp(listServ_pp, sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , port ) <= 0 )
			{
				printf( "\e[1;31mErrore la replica non è + attiva \033[m \n"); //send error
				free(msg);
				return 0;
			}
		}

		if(rc == -1)
		{
			printf("\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}

	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		return 1;
	}
	else
	{
			free(msg);
			return 0;
	}
		
	return 0;
}


//gestione dei messaggi PR
int ctrl_messagePR(int connsd)
{
	PR_Message *msg_pr;
	int i ;
	msg_pr = calloc( 1 , sizeof(PR_Message));
	
	set_timeout(connsd  , timeout_pr  , 0  , timeout_pr  , 0 );	

	if( recv_PR_Message(connsd , msg_pr ) == 1)
	{;}
	else
	{
			free(msg_pr);
			return 0;
	}
	
	if(msg_pr->pay_desc == _PR_BULLY_ELECT)
	{	
		PR_Message *msg;
		msg = init_PR_Message( _PR_ACK , "" ,  "" , 0 , 0 , "");
		
		if(send_PR_Message(connsd , msg ) == 1 )
		{
			close(connsd);
			
			lock_Two_rooms(sem_bully);
				for ( i = 0; i < len_primariesElection ; i++)
				{
					if(strcmp(primariesElection[i].nomeDominio, msg_pr->dom) == 0)
					{
						if(primariesElection[i].status == 0)
						{
							break;
						}
						else
						{
							unlock_Two_rooms(sem_bully);
							return 0;
						}
					}
				}
			unlock_Two_rooms(sem_bully);
			
			Server *primary;

			primary = search_primary(PATH_DB_TMPDOM , msg_pr->dom) ;
			
			Server *vista;
			int len_vista;
			vista = vistaBully(msg_pr->dom , PATH_DB_TMPDOM,	&len_vista ); 
			start_bully(name_server , primary->name ,  msg_pr->dom , vista , len_vista );
			
			free(primary);
			free(vista);
			free(msg);
			free(msg_pr);	
			return 1;
		}
		free(msg);
		free(msg_pr);		
		return 0;
	}
	
	if(msg_pr->pay_desc == _PR_BULLY_PRIMARY)
	{	
		DB *db_dom; // descrittore del DB Domini
		char string_query[_DIM_QUERY_COND];
		char string_query2[_DIM_QUERY_COND];
		char *stringa_errore;
		char str_cond[_DIM_COND];
		
		db_dom = ( DB *) malloc (sizeof(DB) ) ;
		stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char) );
		 
		if( !open_db_rw( db_dom , PATH_DB_TMPDOM , stringa_errore))
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  exit(1);
		}

		int tmp_count ;
		memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
		snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from Dom  where server = '%s' AND dominio ='%s'", msg_pr->opt_char, msg_pr->dom);
		
		memset( string_query2 , '\0' , _DIM_QUERY_COND*sizeof(char) );
		snprintf(string_query2,_DIM_QUERY_COND*sizeof(char), "update Dom set type_server = 1 where server = '%s' AND dominio ='%s'", msg_pr->server, msg_pr->dom);

		lock_WR_Writers(sem_vista);
		
			if( generic_op_db(db_dom , string_query , stringa_errore) == -1)
			{
				printf("%s \n" , stringa_errore );
			}
			
			if( generic_op_db(db_dom , string_query2 , stringa_errore) == -1)
			{
				printf("%s \n" , stringa_errore );
			}
			
			memset( str_cond , '\0' , _DIM_COND);
			snprintf(str_cond , _DIM_COND , " where server = '%s' " , msg_pr->opt_char );

			tmp_count = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
			 
		unlock_WR_Writers(sem_vista);
		
		//printf("query 1 : %s \n " , string_query);
				//printf("query 2 : %s \n " , string_query2);
		
		if( tmp_count == 0) // se un server nella lista domini non compare + viene cancellato anche dalla lista server del server
		{
			DB *db_server; // descrittore del DB Domini
			db_server = ( DB *) malloc (sizeof(DB) ) ;
			
			if( !open_db_rw( db_server , PATH_DB_PRIMARY , stringa_errore))
			{
				fprintf(stderr, "%s\n", stringa_errore);
				exit(1);
			}
			
			memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
			snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from ListServer  where name = '%s'" , msg_pr->opt_char );
			
			lock_WR_Writers(sem_list_serv);
				if( generic_op_db(db_server , string_query , stringa_errore) == -1)
				{
					printf("%s \n" , stringa_errore );
				}
				
				free(list_server);
				memset( str_cond , '\0' , _DIM_COND);
				snprintf(str_cond , _DIM_COND*sizeof(char) ," where name <> '%s' " , name_server );
				int rc;
				list_server = create_list_server(db_server ,  &rc , "ListServer" , str_cond , stringa_errore );

			unlock_WR_Writers(sem_list_serv);
			
			if( !close_db(db_server, stringa_errore))
			{
				fprintf(stderr, "%s\n", stringa_errore);
				exit(1);
			}
			free(db_server);
		}
		
		memset( str_cond , '\0' , _DIM_COND);
		strcpy(str_cond ," where type_server = 1 ");	
		
		lock_WR_Writers(sem_list_dom_serv);
			free(string_dom_serv);
			int rc;
			string_dom_serv = create_string_dom_serv(db_dom ,  &rc , "Dom" , str_cond, stringa_errore );
		unlock_WR_Writers(sem_list_dom_serv);
		
		if( !close_db(db_dom, stringa_errore))
		{
  		fprintf(stderr, "%s\n", stringa_errore);
  		exit(1);
		}	
  	
		free(db_dom);
		free(stringa_errore);
		
		lock_Two_rooms(sem_bully);
			for ( i = 0; i < len_primariesElection ; i++)
			{
				if(strcmp(primariesElection[i].nomeDominio, msg_pr->dom) == 0)
				{
					primariesElection[i].status = 0;
					break;
				}
			}
		unlock_Two_rooms(sem_bully);
	
	free(msg_pr);		
	return 1;
	}
	
	if(msg_pr->pay_desc == _PR_LIST_SERVDOM)
	{
		int fd;
		fd = open(PATH_DB_TMPDOM , O_RDONLY);
	
		struct stat buf;
	
		if(fstat(fd, &buf) == -1)
		{
			close(fd);
			free(msg_pr);		
			return 0;
		}
		
		unsigned int size_file ;
		size_file = (unsigned int) buf.st_size;
		
		set_PR_Message(msg_pr , _PR_LIST_SERVDOM , "\0" , "\0" , size_file , 0 , "\0");
		
		if(send_PR_Message(connsd , msg_pr ) == 1 )
		{
			if(write_res_on_res(fd , connsd ,  max_line) )
			{
				close(fd);
				free(msg_pr);		
				return 1;
			}
			else
			{
				close(fd);
				free(msg_pr);		
				return 0;
			}	
		}
		else
		{
			close(fd);
			free(msg_pr);		
			return 0;
		}
		free(msg_pr);		
		return 0;
	}
	
	if(msg_pr->pay_desc == _PR_LIST_SERVER )
	{
		int fd;
		fd = open(PATH_DB_PRIMARY , O_RDONLY);
	
		struct stat buf;
	
		if(fstat(fd, &buf) == -1)
		{
			close(fd);
			free(msg_pr);		
			return 0;
		}
		
		unsigned int size_file ;
		size_file = (unsigned int) buf.st_size;
		
		set_PR_Message(msg_pr , _PR_LIST_SERVER , "\0" , "\0" , size_file , 0 , "\0");
		
		if(send_PR_Message(connsd , msg_pr ) == 1 )
		{
			if(write_res_on_res(fd , connsd ,  max_line) )
			{
				close(fd);
				free(msg_pr);		
				return 1;
			}
			else
			{
				close(fd);
				free(msg_pr);		
				return 0;
			}	
		}
		else
		{
			close(fd);
			free(msg_pr);		
			return 0;
		}
		free(msg_pr);		
		return 0;
	}
	
	if(msg_pr->pay_desc ==  _PR_LOG_STATE )
	{
		TrLog *log;
		char stringa_errore[_MAX_STRING_ERROR] = {0x0};
		int rc;

		log = select_detTR(PATH_DB_LOG, "Log", msg_pr->opt1,msg_pr->opt2, 0, "\0", 0 , "\0", stringa_errore, &rc);

		if(rc <= 0)
		{
			set_PR_Message(msg_pr , _PR_ERROR , "\0" , "\0" , log->list[0].state , 0 , "\0");
		
			if(send_PR_Message(connsd , msg_pr ) == 1 )
			{
				free(log->list);
				free( log );
				free(msg_pr);		
				return 1;
			}
			return 0; //errore
		}

		short int state;
		state = log->list[0].state;
	
		set_PR_Message(msg_pr , _PR_LOG_STATE , "\0" , "\0" , state , 0 , "\0");
		
		if(send_PR_Message(connsd , msg_pr ) == 1 )
		{
			free(log->list);
			free( log );
			free(msg_pr);		
			return 1;
		}
	
		free(log->list);
		free( log );
		free(msg_pr);		
		return 0;
	}
	
	if(msg_pr->pay_desc == _PR_DEL_SERVDOM )
	{
		DB *db_dom; // descrittore del DB Domini
		char string_query[_DIM_QUERY_COND];
		char *stringa_errore;
		char str_cond[_DIM_COND];
		
		db_dom = ( DB *) malloc (sizeof(DB) ) ;
		stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char) );
		 
		if( !open_db_rw( db_dom , PATH_DB_TMPDOM , stringa_errore))
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  exit(1);
		}

		int tmp_count ;
		memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
		snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from Dom  where server = '%s' AND dominio ='%s'", msg_pr->server, msg_pr->dom);

		lock_WR_Writers(sem_vista);
		
			if( generic_op_db(db_dom , string_query , stringa_errore) == -1)
			{
				printf("%s \n" , stringa_errore );
			}

			memset( str_cond , '\0' , _DIM_COND);
			snprintf(str_cond , _DIM_COND , " where server = '%s' " , msg_pr->server );

			tmp_count = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
			 
		unlock_WR_Writers(sem_vista);
		
		if( tmp_count == 0) // se un server nella lista domini non compare + viene cancellato anche dalla lista server del server
		{
			DB *db_server; // descrittore del DB Domini
			db_server = ( DB *) malloc (sizeof(DB) ) ;
			
			if( !open_db_rw( db_server , PATH_DB_PRIMARY , stringa_errore))
			{
				fprintf(stderr, "%s\n", stringa_errore);
				exit(1);
			}
			
			memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
			snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from ListServer  where name = '%s'" , msg_pr->server );
			
			lock_WR_Writers(sem_list_serv);
				if( generic_op_db(db_server , string_query , stringa_errore) == -1)
				{
					printf("%s \n" , stringa_errore );
				}
				
				free(list_server);
				memset( str_cond , '\0' , _DIM_COND);
				snprintf(str_cond , _DIM_COND*sizeof(char) ," where name <> '%s' " , name_server );
				int rc;
				list_server = create_list_server(db_server ,  &rc , "ListServer" , str_cond , stringa_errore );

			unlock_WR_Writers(sem_list_serv);
			
			if( !close_db(db_server, stringa_errore))
			{
				fprintf(stderr, "%s\n", stringa_errore);
				exit(1);
			}
			free(db_server);
		}
		
		memset( str_cond , '\0' , _DIM_COND);
		strcpy(str_cond ," where type_server = 1 ");	
		
		lock_WR_Writers(sem_list_dom_serv);
			free(string_dom_serv);
			int rc;
			string_dom_serv = create_string_dom_serv(db_dom ,  &rc , "Dom" , str_cond, stringa_errore );
		unlock_WR_Writers(sem_list_dom_serv);
		
		if( !close_db(db_dom, stringa_errore))
		{
  		fprintf(stderr, "%s\n", stringa_errore);
  		exit(1);
		}	
  	
		free(db_dom);
		free(stringa_errore);
		free(msg_pr);
	
		set_PR_Message(msg_pr , _PR_ACK , "\0" , "\0" , 0 , 0 , "\0");
		
		if(send_PR_Message(connsd , msg_pr ) == 1 )
		{
			free(msg_pr);		
			return 1;
		}
	}
	
	if(msg_pr->pay_desc == _PR_SET_LOG_STATE )// inviato dal primary alle repliche
	{
		TrLog * det_tr;
		int rc;
		char stringa_errore[_MAX_STRING_ERROR] = {0x0};
	
		det_tr = select_detTR(PATH_DB_LOG, "Log", msg_pr->opt1 ,msg_pr->opt2, 0, "\0", 0 , "\0", stringa_errore, &rc);
	
		if(strncmp(msg_pr->opt_char , "LOG_COMMIT" ,10) == 0 && (det_tr->list[0].state ==  LOG_PRONTO) )
		{
			update_log(PATH_DB_LOG , msg_pr->opt1 ,msg_pr->opt2 , LOG_COMMIT);
	
			if(det_tr->type == _TR_UPLOAD ) // conclusione della transazione sulla replica sull'operazione di upload
			{						
					if(isFile(det_tr->list[0].res3))
						cp_rewrite_file(det_tr->list[0].res3 ,  det_tr->local_res ,max_line );	
			}
		
			if(det_tr->type == _TR_RM)// conclusione della transazione sulla replica sull'operazione di remove
			{
				if( !isFile(det_tr->local_res) )
				{				
					remove(det_tr->local_res);	
				}
			}
		
			if(det_tr->type == _TR_MKDIR)// conclusione della transazione sulla replica sull'operazione di create directory
			{
				if( !isDir(det_tr->local_res) )
					createDirectory(det_tr->local_res);
			}

			if(det_tr->type == _TR_RM_DIR)// conclusione della transazione sulla replica sull'operazione di remove directory
			{
				if( isDir(det_tr->local_res) )
					removeDirectory(det_tr->local_res);	
			}
			
			if(det_tr->type == _TR_MOD_CAP)	// conclusione della transazione sulla primary sull'operazione di mod cap, relativa ai file GA
			{
				if( isFile(det_tr->local_res) )
				{	

					List_ModCap list[det_tr->length];
					int k ;

					for( k = 0 ; k < det_tr->length ; k++)
					{
						list[k].id_cap = det_tr->list[k].opt1;
						memset(list[k].path_mod_cap , '\0' , _DIM_RES);
						memset(list[k].path_sav_cap , '\0' , _DIM_RES);
						strcpy(list[k].path_sav_cap, det_tr->list[k].res3);
					}

					redo_mod_cap(list , det_tr->length , det_tr->local_res );
				}	
			}	
		
			free(det_tr->list);
			free(det_tr);	
			free(msg_pr);		
			return 1;
		}
		
		if(strncmp(msg_pr->opt_char , "LOG_ABORT" , 9 ) == 0 && (det_tr->list[0].state ==  LOG_PRONTO) )
		{		
			update_log(PATH_DB_LOG , msg_pr->opt1, msg_pr->opt2 , LOG_ABORT);
			
			if(det_tr->type == _TR_UPLOAD ) // redo replica sull'operazione di upload
			{						
				if( det_tr->list[0].opt1 == 1)
				{
					cp_rewrite_file(det_tr->list[0].res2 ,  det_tr->local_res ,max_line );	
				}
				else
				{
					if( isFile(det_tr->local_res) )
						remove(det_tr->local_res);
				}
			}
			
			if(det_tr->type == _TR_RM)// redo replica sull'operazione di remove
			{
				if( !isFile(det_tr->local_res) )
				{				
					cp_rewrite_file(det_tr->list[0].res2 , det_tr->local_res ,max_line );	
				}
			}
			
			if(det_tr->type == _TR_MKDIR)// redo replica sull'operazione di create directory
			{
				if( isDir(det_tr->local_res) )
					removeDirectory(det_tr->local_res);	
			}

			if(det_tr->type == _TR_RM_DIR)// redo replica sull'operazione di remove directory
			{
				if( !isDir(det_tr->local_res) )
					createDirectory(det_tr->local_res);	
			}
				
			if(det_tr->type == _TR_MOD_CAP)	// redo primary sull'operazione di mod cap, relativa ai file GA
			{
				if( isFile(det_tr->local_res) )
				{	

					List_ModCap list[det_tr->length];
					int k ;
					
					for( k = 0 ; k < det_tr->length ; k++)
					{
						list[k].id_cap = det_tr->list[k].opt1;
						memset(list[k].path_mod_cap , '\0' , _DIM_RES);
						memset(list[k].path_sav_cap , '\0' , _DIM_RES);
						strcpy(list[k].path_sav_cap, det_tr->list[k].res3);
					}										

					int fl = open(det_tr->local_res, O_RDWR);
					for( k = 0  ;   k < det_tr->length ; ++k)// scrittura sul file originale
					{
						int fc;
						int fr;
						fc = open(list[k].path_mod_cap, O_RDWR);
						char result_file[_DIM_RES] = {0x0};

						mod_CapGA(fl, fc , result_file ,list[k].id_cap , tmp_path);

						close(fc);
						fr = open(result_file, O_RDWR);
						write_res_on_res(fr ,fl , max_line );
						close(fr);
						remove(result_file);
					}
					close(fl);
				}	
			}	
							
			free(det_tr->list);
			free(det_tr);	
			free(msg_pr);		
			return 1;
		}
		
		free(det_tr->list);
		free(det_tr);
		free(msg_pr);		
		return 0;
	}
	return 0;
}
