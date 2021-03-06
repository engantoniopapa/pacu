#include "header.h"

// ricezione primo messaggio dal priary/replica, ritorna: 
//	0 se c'e' un errore, 
//  1 se l'operazione è stata svolta correttamente
// Parametri -> descrittore del socket di connessione
int pool_ComunicationServ(int connsd)
{
	InitMessage *msg_i;

	msg_i = calloc( 1 , sizeof(InitMessage));
	
	set_timeout(connsd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );	

	if( recv_InitMessage( connsd , msg_i)  ==  1)
	{	;}
	else
	{
			free(msg_i);
			return 0;
	}

	if(msg_i->pay_desc == _INIT_START_TR )
	{	
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ACK , 0);
		send_DiagMessage(connsd , msg);
		free(msg);
		printf("inviato diag ack\n");
		int b ;
		b = pool_serv_StartTr(connsd, msg_i);
		free(msg_i);
		return 1;
	}
	
	perror("errore messaggio non formattato \n");
	free(msg_i);
	DiagMessage * msg;
	msg = init_DiagMessage( _DIAG_ERROR ,_BIZANTINE_ERROR);
	send_DiagMessage(connsd , msg);
	free(msg);
	return 0;
}



// // preparazione della transazione che deve ricevere il pool server, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parametri -> descrittore della connessione, messaggio ricevuto
int pool_serv_StartTr(int connsd , InitMessage *msg_i)
{
	if(msg_i->l_res < 1)
	{
		return 0;
	}	
	
	int temp_rc ;	
	temp_rc = is_Primary_OR_Replica( msg_i->res , msg_i->l_res , name_server);

	if(temp_rc == -1)
	{
		TrMessage* msg;
		msg =  init_transaction_msg(_TR_ERROR , -1 , -1 , -1 , _ERR_TR_GE , 0 );
		send_TrMessage(connsd , msg);
		free(msg);
		return 0;
	}
	
	if(temp_rc == 0)
	{
		TrMessage* msg;
		msg =  init_transaction_msg(_TR_ERROR , -1 , -1 , -1 ,  _ERR_TR_IGNDOM , 0);
		send_TrMessage(connsd , msg);
		free(msg);
		return 0;
	}

	int dim  = _DIM_RES * 2;
 	char temp_path[dim];
 	memset(temp_path , '\0' ,  dim);
 	strncat(temp_path, fs_path ,dim);
 	strncat(temp_path, msg_i->res , msg_i->l_res);

	set_timeout(connsd  , timeout_tr  , 0  , timeout_tr , 0 );	
	
	char remote_ip[16] = {0x0};
	
	struct sockaddr remote_addr;
	memset(&remote_addr , '\0' , sizeof(struct sockaddr) );
	socklen_t len = sizeof(struct sockaddr_in);

	getpeername( connsd , &remote_addr, &len );
	struct sockaddr_in *s = (struct sockaddr_in *)&remote_addr;

	strncpy( remote_ip , inet_ntoa(s->sin_addr) ,_DIM_IPV4-1);
	
	TrMessage* msg;
	msg =  (TrMessage*) calloc( 1 , sizeof(TrMessage) );
	
	while(1)
	{	
		memset(msg, '0' , sizeof(TrMessage) );
		int b_recv = recv_TrMessage( connsd , msg) ;
				
		if( b_recv  > 0)
			break;
				
		if( b_recv  < 0)
		{
			printf( "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //recv error
			free(msg);
			return 0;
		}
	
		if( b_recv == 0 )
		{		
			if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
			{
				return 0;
			}
		}
	}
	
	int b;	
	int type;
	int opt1 = msg->opt1;
	unsigned int opt2 = msg->opt2;
	int id_p = msg->id_primary ;
	int id_c = msg->id_count ;
	
	type = msg->pay_desc;
	
	switch (type) 
	{
		case _TR_UPLOAD: 
		{
			b = pool_transaction_upload(connsd , msg_i , opt1 , opt2 , remote_ip ,type , id_p , id_c);
		}
		break;
		
		case _TR_NEW_GA: 
		{
			b = pool_transaction_upload(connsd , msg_i , opt1 , opt2 , remote_ip ,type , id_p , id_c);
		}
		break;
		
		case _TR_NEW_CAP: 
		{
			b = pool_transaction_upload(connsd , msg_i , opt1 , opt2 , remote_ip ,type , id_p , id_c);
		}
		break;
		
		case _TR_RM_CAP: 
		{
			b = pool_transaction_upload(connsd , msg_i , opt1 , opt2 , remote_ip ,type , id_p , id_c);
		}
		break;
		
		case _TR_MV_CAP: 
		{
			b = pool_transaction_upload(connsd , msg_i , opt1 , opt2 , remote_ip ,type , id_p , id_c);
		}
		break;

		case _TR_RM: 
		{
			b = pool_transaction_remove(connsd , msg_i , remote_ip , type , id_p , id_c);
		}
		break;
	
		case _TR_MKDIR: 
		{
			b = pool_transaction_mkdir(connsd , msg_i , remote_ip , type , id_p , id_c);
		}
		break;

		case _TR_MOD_CAP: 
		{
			b = pool_transaction_modCap(connsd , msg_i , opt1 , opt2 , remote_ip , id_p , id_c);
		}
		break;
	
		case _TR_RM_DIR: 
		{
			b = pool_transaction_removeDir(connsd, msg_i , remote_ip , type , id_p , id_c);
		}
		break;
		
		case _TR_ABORT: 
		{
			b = 1;
		}
		break;
		
		default:
		{			
			printf("\e[1;31mErrore: operazione sconosciuta \033[m \n");
		
			set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0  ,  _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			b = 0;
		}
		break; 
  }	
  
	free(msg);	
	return b;
}



// transazione sulla replica di _TR_UPLOAD, _TR_NEW_GA, _TR_NEW_CAP, _TR_MV_CAP, _TR_RM_CAP
int pool_transaction_upload(int connsd ,  InitMessage *msg_i , int opt1 , unsigned int opt2 , char *remote_ip , int type ,int id_p ,int id_c)
{
	int dim_temp = strlen(tmp_path) + L_TEMP;
	char path_filetemp[dim_temp];
	char path_save[dim_temp];
	int fl;
	int dim  = _DIM_RES * 2;
	char dest_path[dim];
	int sv;

	int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);

	if(write_control_res(connsd  , fd,  max_line , opt2) )
	{
		TrMessage* msg;
		msg =  calloc(1 , sizeof( TrMessage) );
		
		while(1)
		{
			memset(msg , 0 , sizeof( TrMessage) );
			int b_recv = recv_TrMessage( connsd , msg) ;
			
			if( b_recv  > 0)
				break;
				
			if( b_recv  < 0)
			{
				printf( "\e[1;31mErrore nella ricezione del messaggio di ack \033[m \n"); //recv error
				close(fd);
				remove(path_filetemp) ;
				free(msg);
				return 0;
			}
		
			if( b_recv == 0 )
			{
				if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
				{					
					internal_bully(msg_i->res , msg_i->l_res );
					close(fd);
					remove(path_filetemp) ;
					free(msg);
					return 0; 
				}
			}
		}
		
		if(msg->pay_desc != _TR_COMMIT)
		{
			set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
		
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
		else
		{		
			memset(dest_path , '\0' ,  dim);
			strncat(dest_path, fs_path ,dim);
			strncat(dest_path, msg_i->res , msg_i->l_res);
			
			short int exist_file = 0;
			exist_file = isFile(dest_path);
			
			insert_log(PATH_DB_LOG , id_p , id_c , remote_ip , ntohs(*port_server) , type , dest_path , "\0" , "\0" , exist_file , -1 , LOG_PREPARA );
			
			if(exist_file)
			{			
				fl = open(dest_path, O_RDWR);
				
				sv = create_fileTemp(tmp_path , path_save , dim_temp);
				
				cp_rewrite_file(dest_path ,  path_save ,max_line ); 
				update_log_res2res3(PATH_DB_LOG , id_p , id_c , LOG_PRONTO  , dest_path  , path_save , path_filetemp);
				close(sv);
				write_res_on_res(fd , fl , max_line );
				close(fd);
				remove(path_filetemp) ;
			}
			else
			{
				update_log_res2res3(PATH_DB_LOG , id_p , id_c , LOG_PRONTO  , dest_path  , "\0" , path_filetemp);
				cp_rewrite_file(path_filetemp , dest_path , max_line );
				close(fd);
				remove(path_filetemp) ;
			}
						
			int prova_commit = 1;
			int recv_abort = 0;
			set_transaction_msg(msg , _TR_COMMIT , id_p , id_c , 0 ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al server Primary
			{		
				if(send_TrMessage(connsd , msg)<= 0)
					prova_commit = 0;
			}
			
			if( prova_commit )
			{			
				int b_recv;
				while(1)
				{
					memset(msg , 0 , sizeof( TrMessage) );
					b_recv = recv_TrMessage( connsd , msg) ;
			
					if( b_recv  > 0)
					{
						if(msg->pay_desc != _TR_COMMIT)
						{
							recv_abort = 1;
						}
						break;
					}
					printf("minore di zero \n");
					if( b_recv  < 0)
					{
						printf( "\e[1;31mErrore nella ricezione del messaggio di commit/abort dal primary \033[m \n"); //recv error
						break;
					}
		
					if( b_recv == 0 )
					{
						if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
						{
							internal_bully(msg_i->res , msg_i->l_res );
							break;
						}
					}
				}
				
				if(b_recv  > 0 && recv_abort == 0)
				{
					update_log(PATH_DB_LOG , id_p , id_c , LOG_COMMIT);
					
					if(exist_file)
					{
						close(fl);
						remove(path_save);
					}

					close(fd);
					remove(path_filetemp);
					free(msg);
					return 1;				
				}
				else
				{
					if(exist_file)
					{
						sv = open(path_save , O_RDWR);
						write_res_on_res(sv , fl , max_line );
						close(sv);
						close(fl);
					}
					else
					{
						remove(dest_path);
					}
				
					update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
				
					if(exist_file)
					{
						remove(path_save);
					}
					close(fd);
					remove(path_filetemp);
					free(msg);
					return 0;
				}	
			}
			else
			{
				if(exist_file)
				{
					sv = open(path_save , O_RDWR);
					write_res_on_res(sv , fl , max_line );
					close(sv);
					close(fl);
				}
				else
				{
					remove(dest_path);
				}
				
				update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
				
				if(exist_file)
				{
					remove(path_save);
				}
				close(fd);
				remove(path_filetemp);
				free(msg);
				return 0;
			}
		}
	}
	else
	{		
		close(fd);
		remove(path_filetemp) ;
		
		TrMessage* msg;
		msg =  calloc(1 , sizeof( TrMessage) );
		set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_GE , 0);
		send_TrMessage(connsd , msg);
		free(msg);
		return 0;
	}	
	
	return 0;
}



// transazione sulla replica di _TR_RM
int pool_transaction_remove(int connsd , InitMessage *msg_i ,char *remote_ip , int type ,int id_p ,int id_c)
{
	int sv;
	
	TrMessage* msg;
	msg =  calloc(1 , sizeof( TrMessage) );		
	while(1)
	{
		memset(msg , 0 , sizeof( TrMessage) );	
		int b_recv = recv_TrMessage( connsd , msg) ;
			
		if( b_recv  > 0)
			break;
				
		if(b_recv < 0)
		{
			printf( "\e[1;31mErrore nella ricezione del messaggio di ack \033[m \n"); //recv error
			free(msg);
			return 0;
		}
		if( b_recv == 0 )
		{
			if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
			{
				internal_bully(msg_i->res , msg_i->l_res );
				free(msg);
				return 0;
			}
		}
	}
	
	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_BIZ , 0);
		send_TrMessage(connsd , msg);

		free(msg);
		return 0;
	}
	else
	{
		int dim  = _DIM_RES * 2;
		char dest_path[dim];
		memset(dest_path , '\0' ,  dim);
		strncat(dest_path, fs_path ,dim);
		strncat(dest_path, msg_i->res , msg_i->l_res);
	
		insert_log(PATH_DB_LOG , id_p , id_c , remote_ip , ntohs(*port_server) , _TR_RM , dest_path , "\0" ,"\0" ,  -1 , -1 , LOG_PREPARA );
		
		int dim_temp = strlen(tmp_path) + L_TEMP;

		char path_save[dim_temp];
		sv = create_fileTemp(tmp_path , path_save , dim_temp);
		close(sv);
		cp_rewrite_file(dest_path ,  path_save ,max_line );

		update_log_res2(PATH_DB_LOG , id_p , id_c , LOG_PRONTO  , dest_path  , path_save);

		remove(dest_path);
								
		int prova_commit = 1;
		int recv_abort = 0;
		set_transaction_msg(msg , _TR_COMMIT , id_p , id_c , 0 ,  0 , 0);
		
		if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al server Primary
		{		
			if(send_TrMessage(connsd , msg)<= 0)
				prova_commit = 0;
		}
		
		if( prova_commit )
		{			
			int b_recv;
			while(1)
			{
				memset(msg , 0 , sizeof( TrMessage) );
				b_recv = recv_TrMessage( connsd , msg) ;
		
				if( b_recv  > 0)
				{
					if(msg->pay_desc != _TR_COMMIT)
					{
						recv_abort = 1;
					}
					break;
				}
			
				if( b_recv  < 0)
				{
					printf( "\e[1;31mErrore nella ricezione del messaggio di commit/abort dal primary \033[m \n"); //recv error
					break;
				}
	
				if( b_recv == 0 )
				{
					if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
					{
						internal_bully(msg_i->res , msg_i->l_res );
						break;
					}
				}
			}
			
			if(b_recv  > 0 && recv_abort == 0)
			{
				update_log(PATH_DB_LOG , id_p , id_c , LOG_COMMIT);
				remove(path_save);
				free(msg);
				return 1;				
			}
			else
			{
				cp_rewrite_file(path_save , dest_path ,max_line );
				update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
				free(msg);
				return 0;
			}	
		}
		else
		{
			cp_rewrite_file(path_save , dest_path ,max_line );
			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
			free(msg);
			return 0;
		}
	}
	free(msg);
	return 0;
}



// transazione sulla replica di _TR_MKDIR
int pool_transaction_mkdir(int connsd , InitMessage *msg_i ,char *remote_ip , int type ,int id_p ,int id_c)
{
	TrMessage* msg;
	msg =  calloc(1 , sizeof( TrMessage) );		
	while(1)
	{
		memset(msg , 0 , sizeof( TrMessage) );	
		int b_recv = recv_TrMessage( connsd , msg) ;
			
		if( b_recv  > 0)
			break;
				
		if(b_recv < 0)
		{
			printf( "\e[1;31mErrore nella ricezione del messaggio di ack \033[m \n"); //recv error
			free(msg);
			return 0;
		}
		if( b_recv == 0 )
		{
			if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
			{
				internal_bully(msg_i->res , msg_i->l_res );
				free(msg);
				return 0;
			}
		}
	}
	
	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_BIZ , 0);
		send_TrMessage(connsd , msg);

		free(msg);
		return 0;
	}
	else
	{	
		int dim  = _DIM_RES * 2;
		char dest_path[dim];
		memset(dest_path , '\0' ,  dim);
		strncat(dest_path, fs_path ,dim);
		strncat(dest_path, msg_i->res , msg_i->l_res);
		
		insert_log(PATH_DB_LOG , id_p , id_c , remote_ip , ntohs(*port_server) , _TR_MKDIR , dest_path , "\0" ,"\0" ,  -1 , -1 , LOG_PREPARA );
		
		if(!isDir(dest_path) )
		{
			update_log(PATH_DB_LOG , id_p , id_c , LOG_PRONTO);
			createDirectory(dest_path);
		}
		else
		{
			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ABORT , id_p , id_c , 0 ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			free(msg);
			return 0;
		}
		
		int prova_commit = 1;
		int recv_abort = 0;
		set_transaction_msg(msg , _TR_COMMIT , id_p , id_c , 0 ,  0 , 0);
		
		if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al server Primary
		{		
			if(send_TrMessage(connsd , msg)<= 0)
				prova_commit = 0;
		}
		
		if( prova_commit )
		{			
			int b_recv;
			while(1)
			{
				memset(msg , 0 , sizeof( TrMessage) );
				b_recv = recv_TrMessage( connsd , msg) ;
		
				if( b_recv  > 0)
				{
					if(msg->pay_desc != _TR_COMMIT)
					{
						recv_abort = 1;
					}
					break;
				}
			
				if( b_recv  < 0)
				{
					printf( "\e[1;31mErrore nella ricezione del messaggio di commit/abort dal primary \033[m \n"); //recv error
					break;
				}
	
				if( b_recv == 0 )
				{
					if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
					{
						internal_bully(msg_i->res , msg_i->l_res );
						break;
					}
				}
			}
			
			if(b_recv  > 0 && recv_abort == 0)
			{
				update_log(PATH_DB_LOG , id_p , id_c , LOG_COMMIT);
				free(msg);
				return 1;				
			}
			else
			{	
				removeDirectory(dest_path);
				update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
				free(msg);
				return 0;
			}	
		}
		else
		{
			removeDirectory(dest_path);
			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
			free(msg);
			return 0;
		}
	}
	free(msg);
	return 0;
}



// transazione sulla replica di _TR_RM_DIR
int pool_transaction_removeDir(int connsd , InitMessage *msg_i ,char *remote_ip , int type ,int id_p ,int id_c)
{
	TrMessage* msg;
	msg =  calloc(1 , sizeof( TrMessage) );		
	while(1)
	{
		memset(msg , 0 , sizeof( TrMessage) );	
		int b_recv = recv_TrMessage( connsd , msg) ;
			
		if( b_recv  > 0)
			break;
				
		if(b_recv < 0)
		{
			printf( "\e[1;31mErrore nella ricezione del messaggio di ack \033[m \n"); //recv error
			free(msg);
			return 0;
		}
		if( b_recv == 0 )
		{
			if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
			{
				internal_bully(msg_i->res , msg_i->l_res );
				free(msg);
				return(0);
			}
		}
	}

	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_BIZ , 0);
		send_TrMessage(connsd , msg);

		free(msg);
		return 0;
	}
	else
	{	
		int dim  = _DIM_RES * 2;
		char dest_path[dim];
		memset(dest_path , '\0' ,  dim);
		strncat(dest_path, fs_path ,dim);
		strncat(dest_path, msg_i->res , msg_i->l_res);

		insert_log(PATH_DB_LOG , id_p , id_c , remote_ip , ntohs(*port_server), _TR_RM_DIR , dest_path , "\0" ,"\0" ,  -1 , -1 , LOG_PREPARA );
		
		if(isDir(dest_path) )
		{
			update_log(PATH_DB_LOG , id_p , id_c , LOG_PRONTO);
			removeDirectory(dest_path);
		}
		else
		{
			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ABORT , id_p , id_c , 0 ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			free(msg);
			return 0;
		}
		
		int prova_commit = 1;
		int recv_abort = 0;
		set_transaction_msg(msg , _TR_COMMIT , id_p , id_c , 0 ,  0 , 0);
		
		if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al server Primary
		{		
			if(send_TrMessage(connsd , msg)<= 0)
				prova_commit = 0;
		}
		
		if( prova_commit )
		{			
			int b_recv;
			while(1)
			{
				memset(msg , 0 , sizeof( TrMessage) );
				b_recv = recv_TrMessage( connsd , msg) ;
		
				if( b_recv  > 0)
				{
					if(msg->pay_desc != _TR_COMMIT)
					{
						recv_abort = 1;
					}
					break;
				}
			
				if( b_recv  < 0)
				{
					printf( "\e[1;31mErrore nella ricezione del messaggio di commit/abort dal primary \033[m \n"); //recv error
					break;
				}
	
				if( b_recv == 0 )
				{
					if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
					{
						internal_bully(msg_i->res , msg_i->l_res );
						break;
					}
				}
			}
			
			if(b_recv  > 0 && recv_abort == 0)
			{
				update_log(PATH_DB_LOG , id_p , id_c , LOG_COMMIT);
				free(msg);
				return 1;				
			}
			else
			{	
				createDirectory(dest_path);
				update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
				free(msg);
				return 0;
			}	
		}
		else
		{
			createDirectory(dest_path);
			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
			free(msg);
			return 0;
		}
	}
	free(msg);
	return 0;
}



// transazione sulla replica di _TR_MOD_CAP
int pool_transaction_modCap(int connsd , InitMessage *msg_i ,int opt1 , int opt2 , char *remote_ip ,int id_p ,int id_c)
{
	int fl;
	int dim  = _DIM_RES * 2;
	char dest_path[dim] ;
	char orig_res[dim] ; // percorso della risorsa nel primo messaggio
	TrMessage *msg = (TrMessage *) calloc ( 1 , sizeof(TrMessage) ) ;
	
	memset(dest_path , '\0' , dim);
	memset(orig_res , '\0' , dim);
	
	strncat(dest_path, fs_path ,dim);
	strncat(dest_path, msg_i->res , msg_i->l_res);
	strncat(orig_res, msg_i->res , msg_i->l_res);
	
	int loc_max_cap = tr_max_cap;
	
	
	int dim_temp = strlen(tmp_path) + L_TEMP;
	int id_cap[loc_max_cap];
	int descrittori[loc_max_cap];
	char path_mod_file[loc_max_cap][_DIM_RES];
	
	int i;
	
	for( i = 0 ; i < loc_max_cap ; ++i)
	{
	
		id_cap[i] = -1;
		descrittori[i] = -1 ;
		memset( path_mod_file[i] , '\0' , _DIM_RES );
	
	}

	id_cap[0] = opt1;
	descrittori[0] = create_fileTemp(tmp_path , path_mod_file[0] , dim_temp);

	insert_log(PATH_DB_LOG, id_p ,id_c ,remote_ip ,ntohs(*port_server) ,_TR_MOD_CAP ,dest_path ,path_mod_file[0] ,"\0" ,opt1 ,-1 ,LOG_PREPARA );

	if(write_control_res(connsd  , descrittori[0],  max_line , opt2) )
	{;}
	else
	{
			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);					
			set_transaction_msg( msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			close(descrittori[0]);
			remove(path_mod_file[0]);
			free(msg);
			return 0;
	}
	
	int m ;
	int len_list = 1;
	short int ack = 0;
	
	for(m = 1 ; ( m < loc_max_cap ) && ( ack == 0 ) ; ++m) 
	{

		while(1)
		{
			memset(msg , 0 , sizeof( TrMessage) ); 
			int b_recv = recv_TrMessage( connsd , msg) ;	
		
			if( b_recv  > 0)
				break;
				
			if(b_recv < 0)
			{
				set_transaction_msg( msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_BIZ , 0);
				send_TrMessage(connsd , msg);
				
				for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
				{	
					close(descrittori[i]);
					remove(path_mod_file[i]);
				}
				free(msg);
				return 0;
			}
		
			if( b_recv == 0 )
			{			
				if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
				{
					internal_bully(msg_i->res , msg_i->l_res );
					
					for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
					{
						close(descrittori[i]);
						remove(path_mod_file[i]);	
					}
					free(msg);
					return 0;
				}
			}
		}

		if(msg->pay_desc != _TR_COMMIT && msg->pay_desc != _TR_MOD_CAP)
		{
			set_transaction_msg( msg , _TR_ERROR , id_p , id_c , 0 , _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
		
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{	
				close(descrittori[i]);
				remove(path_mod_file[i]);
			}
			free(msg);
			return 0;
		}
		else
		{
			if(msg->pay_desc == _TR_MOD_CAP)
			{
				
				id_cap[m] = msg->opt1;
				descrittori[m] = create_fileTemp(tmp_path , path_mod_file[m] , dim_temp);

				insert_log(PATH_DB_LOG,id_p,id_c,remote_ip,ntohs(*port_server),_TR_MOD_CAP,dest_path,path_mod_file[m],"\0",msg->opt1,-1 ,  LOG_PREPARA );

				if(write_control_res(connsd  , descrittori[m],  max_line , msg->opt2) )
				{;}
				else
				{
					update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);					
					set_transaction_msg( msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_BIZ , 0);
					send_TrMessage(connsd , msg);
					for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
					{	
						close(descrittori[i]);
						remove(path_mod_file[i]);
					}
					free(msg);
					return 0;
				}					
			}
			else
			{
				ack = 1;
				len_list = m; // m è già incrementato di uno visto che conta il messaggio di ack e non di modifica cap
			}
		}
	}
	
	if( ack == 0)
	{
		while(1)
		{
			memset(msg , 0 , sizeof( TrMessage) ); 
			int b_recv = recv_TrMessage( connsd , msg) ;	
	
			if( b_recv  > 0)
				break;
			
			if(b_recv < 0)
			{
				update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);					
				set_transaction_msg( msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_BIZ , 0);
				send_TrMessage(connsd , msg);
			
				for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
				{	
					close(descrittori[i]);
					remove(path_mod_file[i]);
				}
				free(msg);
				return 0;
			}
	
			if( b_recv == 0 )
			{			
				if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
				{
					internal_bully(msg_i->res , msg_i->l_res );
					
					update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);					
					for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
					{
						close(descrittori[i]);
						remove(path_mod_file[i]);	
					}
					free(msg);
					return 0;
				}
			}
		}
		len_list = loc_max_cap;
	}
	
	for( i = 0 ; i < len_list; ++i)
	{	
		close(descrittori[i]);
	}
	
	List_ModCap *l_mod_cap = init_List_ModCap( len_list , path_mod_file ,  id_cap) ;
	

	fl = open(dest_path, O_RDONLY);


	if(fl <= 0)
	{
		printf( "Errore nell'apertura della risorsa!!!! \n");
		update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
		
		set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0  ,  _ERR_TR_GE , 0);
		send_TrMessage(connsd , msg);
		
		for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
		{
			remove(path_mod_file[i]);	
		}
		free(msg);
		free(l_mod_cap);
		return 0;
	}
	lockRead(fl);
	
	for(i = 0 ; i < len_list ; ++i) // effettuo le copie di salvataggio dei capitoli
	{
		int sv = create_fileTemp(tmp_path , l_mod_cap[i].path_sav_cap , dim_temp);
		
		if (extract_from_IdCapGA(fl , sv, l_mod_cap[i].id_cap) )
		{
			update_log_res3(PATH_DB_LOG , id_p , id_c , LOG_PREPARA , dest_path  , l_mod_cap[i].path_mod_cap  , l_mod_cap[i].path_sav_cap);
			close(sv);
		}
		else
		{
			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
		
			set_transaction_msg(msg , _TR_ERROR , id_p , id_c , 0 ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			
			int j;
			for( j = 0 ; (j < len_list ); ++j)
			{
				remove(l_mod_cap[j].path_mod_cap);	
			}
			
			for( j = 0 ; j < i ; ++j)
			{
				remove(l_mod_cap[j].path_sav_cap);	
			}
			close(sv);
			unlockFile(fl);
			close(fl);
			free(msg);
			free(l_mod_cap);
			return 0;
		}
	}
	unlockFile(fl);
	close(fl);
	
	fl = open(dest_path,O_RDWR);
	update_log(PATH_DB_LOG , id_p , id_c , LOG_PRONTO);	

	lockWrite(fl);

		for( i = 0  ;   i < len_list ; ++i)// scrittura sul file originale
		{
			int fc;
			int fr;
			fc = open(l_mod_cap[i].path_mod_cap, O_RDWR);
			char result_file[_DIM_RES] = {0x0};

			mod_CapGA(fl, fc , result_file ,l_mod_cap[i].id_cap , tmp_path);

			close(fc);
			fr = open(result_file, O_RDWR);
			write_res_on_res(fr ,fl , max_line );
			close(fr);
			remove(result_file);
		}

	unlockFile(fl);
	
	int prova_commit = 1;
	int recv_abort = 0;
	set_transaction_msg(msg , _TR_COMMIT , id_p , id_c , 0 ,  0 , 0);
		
	if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al server Primary
	{		
		if(send_TrMessage(connsd , msg)<= 0)
			prova_commit = 0;
	}
		
	if( prova_commit )
	{			
		int b_recv;
		while(1)
		{
			memset(msg , 0 , sizeof( TrMessage) );
			b_recv = recv_TrMessage( connsd , msg) ;
	
			if( b_recv  > 0)
			{
				if(msg->pay_desc != _TR_COMMIT)
				{
					recv_abort = 1;
				}
				break;
			}
		
			if( b_recv  < 0)
			{
				printf( "\e[1;31mErrore nella ricezione del messaggio di commit/abort dal primary \033[m \n"); //recv error
				break;
			}

			if( b_recv == 0 )
			{
				if( isAlive_pp(listServ_pp , sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_server_pp ) <= 0 )
				{
					internal_bully(msg_i->res , msg_i->l_res );
					break;
				}
			}
		}			
		if(b_recv  > 0 && recv_abort == 0)
		{
			close(fl);
			update_log(PATH_DB_LOG , id_p , id_c , LOG_COMMIT);
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
				remove(l_mod_cap[i].path_mod_cap);	
				remove(l_mod_cap[i].path_sav_cap);	
			}
	
			free(msg);
			free(l_mod_cap);
			return 1;				
		}		
		else
		{	
			close(fl);
			redo_mod_cap( l_mod_cap , len_list , dest_path );

			update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
				remove(l_mod_cap[i].path_mod_cap);	
				remove(l_mod_cap[i].path_sav_cap);	
			}

			free(msg);
			free(l_mod_cap);
			free(msg);
			return 0;
		}	
	}
	else
	{
		close(fl);
		redo_mod_cap( l_mod_cap , len_list , dest_path );

		update_log(PATH_DB_LOG , id_p , id_c , LOG_ABORT);
		for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
		{
			remove(l_mod_cap[i].path_mod_cap);	
			remove(l_mod_cap[i].path_sav_cap);	
		}

		free(msg);
		free(l_mod_cap);
		free(msg);
		return 0;
	}
	
	free(msg);
	free(l_mod_cap);
	return 0;
}	
	
