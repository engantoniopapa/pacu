#include "header.h"


// avvia la transazione lato server, ritorna: 1 se l'operazione è andata a buon fine, 0 se è fallita
// Parametri-> socket per la connessione, init message inviato dal client 
int start_transaction(int connsd , InitMessage *msg_i)
{
	set_timeout(connsd  , timeout_tr  , 0  , timeout_tr , 0 );	
	
	char remote_ip[16] = {0x0};
	Id_Trans id_t;

	id_t.msg_count = 1 ;

	get_idTr( &(id_t.id_p) , &(id_t.id_c) );
	
	struct sockaddr remote_addr;
	memset(&remote_addr , '\0' , sizeof(struct sockaddr) );
	socklen_t len = sizeof(struct sockaddr_in);

	getpeername( connsd , &remote_addr, &len );
	struct sockaddr_in *s = (struct sockaddr_in *)&remote_addr;

	strncpy( remote_ip , inet_ntoa(s->sin_addr) ,_DIM_IPV4-1);
	
	TrMessage* msg;
	msg =  init_transaction_msg(_TR_DO_ID , id_t.id_p , id_t.id_c , id_t.msg_count ,  0 , 0);
		
	if(send_TrMessage(connsd , msg) <= 0)
	{
		printf( "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //recv error
		free(msg);
		return 0;
	}
	


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
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				printf("il client: %s non è attivo \n" , remote_ip);
				return 0;
			}
		}
	}
	
	++id_t.msg_count;
	
	if( id_t.msg_count != msg->msg_count || id_t.id_p != msg->id_primary || id_t.id_c != msg->id_count )
	{
		printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
		
		if( id_t.id_p != msg->id_primary || id_t.id_c != msg->id_count )
		{
			set_transaction_msg(msg , _TR_ERROR , id_t.id_p , id_t.id_c , id_t.msg_count +1 ,  _ERR_TR_BIZ , 0);
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t.id_p , id_t.id_c , id_t.msg_count +1 ,  _ERR_TR_MISSPACK , 0);
		}
		
		send_TrMessage(connsd , msg);
		
		free(msg);
		return 0;
	}
	
	
	int b;
	
	int type;
	int opt1 = msg->opt1;
	unsigned int opt2 = msg->opt2;
	
	type = msg->pay_desc;
	
	switch (type) 
	{
		case _TR_UPLOAD: 
		{
			b = transaction_upload(connsd , &id_t , msg_i , opt2 , remote_ip);
		}
		break;

		case _TR_RM: 
		{
			b = transaction_remove(connsd , &id_t , msg_i , remote_ip);
		}
		break;
	
		case _TR_MKDIR: 
		{
			b = transaction_mkdir(connsd , &id_t , msg_i , remote_ip);
		}
		break;

		case _TR_NEW_GA: 
		{
			b = transaction_new_fileGa(connsd , &id_t , msg_i , remote_ip);
		}
		break;
	
		case _TR_NEW_CAP: 
		{
			b = transaction_newCap(connsd , &id_t , msg_i , opt1 , opt2 , remote_ip);
		}
		break;
	
		case _TR_MV_CAP: 
		{
			b = transaction_moveCap(connsd , &id_t , msg_i , opt1 , opt2 , remote_ip);
		}
		break;

		case _TR_MOD_CAP: 
		{
			b = transaction_modCap(connsd , &id_t , msg_i , opt1 , opt2 , remote_ip);
		}
		break;
	
		case _TR_RM_CAP: 
		{
			b = transaction_removeCap(connsd , &id_t , msg_i , opt1 , remote_ip);
		}
		break;
	
		case _TR_RM_DIR: 
		{
			b = transaction_removeDir(connsd , &id_t , msg_i , remote_ip);
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
		
			set_transaction_msg(msg , _TR_ERROR , id_t.id_p , id_t.id_c , id_t.msg_count +1  ,  _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			b = 0;
		}
		break; 
  }	
  
	free(msg);

	return b;
}



// operazione di upload file, ritorna 1 se l'operazione va a buon fine 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction, grandezza file
int transaction_upload(int connsd , Id_Trans *id_t , InitMessage *msg_i , unsigned int opt2 , char *remote_ip)
{
	int dim_temp = strlen(tmp_path) + L_TEMP;
	char path_filetemp[dim_temp];
	char path_save[dim_temp] ;

	int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
	int fl;
	int sv;
	int index;

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
				if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip ,remote_ip , (int) *port_client_pp ) <= 0 )
				{
					printf("il client: %s non è attivo \n" , remote_ip);
					return 0;
				}
			}
		}
		++id_t->msg_count;
	
		if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
			memset(msg, '0' , sizeof(TrMessage) );
		
			if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
			{
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
			}
			else
			{
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
			}
		
			send_TrMessage(connsd , msg);
		
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
		
		if(msg->pay_desc != _TR_COMMIT)
		{
			++id_t->msg_count;
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
		
			close(fd);
			remove(path_filetemp) ;
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
			short int exist_file = 0;
			
			exist_file = isFile(dest_path);
			
			insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_UPLOAD , dest_path , "\0" , "\0" , exist_file , -1 , LOG_PREPARA );
			++id_t->msg_count;
						

			int rc;
			
			if(exist_file)
			{
				rc = is_gaFile(dest_path) ;	
		
				if(rc == -1)
				{
					close(fd);
					update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
					set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
					send_TrMessage(connsd , msg);
		
					remove(path_filetemp) ;
					free(msg);
					return 0;	
				}
			
				fl = open(dest_path, O_RDWR);

				if(fl <= 0)
				{
					printf( "Errore nell'apertura della risorsa!!!! \n");
					set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
					send_TrMessage(connsd , msg);
					close(fd);
					update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
					remove(path_filetemp) ;
					free(msg);
					return 0;
				}
			
				if(rc == 0)
				{
					if (!lockWrite(fl))
					{
						printf( "Errore nel lock fisico della risorsa (esistente)!!!! \n");
						close(fd);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
						send_TrMessage(connsd , msg);
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						remove(path_filetemp) ;
						close(fl);
						free(msg);
						return 0;
					}
					
					if(!isFile(dest_path))
					{
						printf( "la risorsa è stata cancellata in modo concorrente \n");
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_ABORT , 0);
						send_TrMessage(connsd , msg);
						remove(path_filetemp) ;
						unlockFile(fl);
						close(fl);
						free(msg);
						return 0;
					}
				}
			
				index = -1;
				if(rc == 1)
				{
					index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
				
					if(index == -1)
					{
						printf( "Errore nel lock fisico e logico della risorsa (esistente)!!!! \n");
						close(fd);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
						send_TrMessage(connsd , msg);
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						remove(path_filetemp) ;			
						close(fl);
						free(msg);
						return 0;
					}
					
					if(is_gaFile(dest_path) != 1)
					{
						printf( "la risorsa è stata cancellata in modo concorrente \n");
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_ABORT , 0);
						send_TrMessage(connsd , msg);
						remove(path_filetemp) ;
						unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
						close(fl);
						free(msg);
						return 0;
					}
					
				}
			}

			if(exist_file) // generazione file salvataggio
			{	
				sv = create_fileTemp(tmp_path , path_save , dim_temp);
				
				cp_rewrite_file(dest_path ,  path_save ,max_line );			
				update_log_res2(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO  , dest_path  , path_save);
				write_res_on_res(fd , fl , max_line );
				close(sv);
				close(fd);
				remove(path_filetemp) ;
			}
			else
			{
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO);
				cp_rewrite_file(path_filetemp , dest_path , max_line );
				
				rc = is_gaFile(dest_path) ;	
		
				if(rc == -1)
				{
					close(fd);
					update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
					set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
					send_TrMessage(connsd , msg);
					remove(path_filetemp) ;
					free(msg);
					return 0;	
				}
			
				fl = open(dest_path, O_RDWR);

				if(fl <= 0)
				{
					printf( "Errore nell'apertura della risorsa!!!! \n");
					set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
					send_TrMessage(connsd , msg);
					close(fd);
					update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
					remove(path_filetemp) ;
					free(msg);
					return 0;
				}
			
				if(rc == 0)
				{
					if (!lockWrite(fl))
					{
						printf( "Errore nel lock fisico della risorsa (non esistente)!!!! \n");
						close(fd);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
						send_TrMessage(connsd , msg);
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						remove(path_filetemp) ;
						close(fl);
						free(msg);
						return 0;
					}
					
					if(!isFile(dest_path))
					{
						printf( "la risorsa è stata cancellata in modo concorrente \n");
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  , _ERR_TR_ABORT , 0);
						send_TrMessage(connsd , msg);
						remove(path_filetemp) ;
						unlockFile(fl);
						close(fl);
						free(msg);
						return 0;
					}
				}
			
				index = -1;
				if(rc == 1)
				{
					index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
				
					if(index == -1)
					{
						printf( "Errore nel lock fisico e logico della risorsa (non esistente)!!!! \n");
						close(fd);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
						send_TrMessage(connsd , msg);
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						remove(path_filetemp) ;			
						close(fl);
						free(msg);
						return 0;
					}
					
					if(is_gaFile(dest_path) != 1)
					{
						printf( "la risorsa è stata cancellata in modo concorrente \n");
						update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
						set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_ABORT , 0);
						send_TrMessage(connsd , msg);
						remove(path_filetemp) ;
						unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
						close(fl);
						free(msg);
						return 0;
					}
				}
				
				close(fd);
				remove(path_filetemp) ;
			}

			Server *list_vista;
			int len_vista;
			char name_dom[_DIM_DOM+1] ={0x0};
			short int riuscita = 0;
			
			int q ;
			for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
			{
				if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
				{
					break;
				}
				else
				{
					name_dom[q-1] = msg_i->res[q];
				}
			}

			list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
			
			if(len_vista < 0) // ci deve essere almeno una replica attiva
			{
				riuscita = 0;
			} 
			else
			{
				riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

				if(riuscita)
				{
					if(op_rep_tr1(msg_i ,_TR_UPLOAD , list_vista , len_vista , fl , id_t ) )	//invio file a repliche
					{
						set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
						if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
							send_TrMessage(connsd , msg);
						riuscita = 1;
					}
					else
						riuscita = 0;
				}
				else
				{
					riuscita = 0;
				}
				free(list_vista);
			}

			if(riuscita)
			{
				close(fd);
				
				if(rc == 0)
				{
					unlockFile(fl);
				}
	
				if(rc == 1)
				{
					unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
				}	
		
				close(fl);
				
				if(exist_file)
				{
					remove(path_save);
				}	

				remove(path_filetemp) ;
				
				free(msg);
				return 1;
			}
			else
			{
				if(exist_file)
				{	
					sv = open(path_save, O_RDWR);
					write_res_on_res(sv, fl  ,max_line ); // riporto il file al vecchio stato con una redo
					close(sv);
				}
				else
					remove(dest_path);
					
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				close(fd);

				if(rc == 0)
				{
					unlockFile(fl);
				}
		
				if(rc == 1)
				{
					unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
				}	
			
				close(fl);
				
				if(exist_file)
				{
					remove(path_save);
				}	

				remove(path_filetemp) ;
				
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				free(msg);

				return 0;
			}
		}
	}
	else
	{		
		close(fd);
		remove(path_filetemp) ;
		return 0;
	}	
	
	return 0;
}



// operazione di remove file, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction
int transaction_remove(int connsd , Id_Trans *id_t , InitMessage *msg_i  , char *remote_ip)
{
	int fl;
	int index = -1;
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
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				printf("il client: %s non è attivo \n" , remote_ip);
				return 0;
			}
		}
	}
	
	++id_t->msg_count;

	if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
	{
		printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
		memset(msg, '0' , sizeof(TrMessage) );

		if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
		}

		send_TrMessage(connsd , msg);

		free(msg);
		return 0;
	}

	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
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
	
		insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_RM , dest_path , "\0" ,"\0" ,  -1 , -1 , LOG_PREPARA );

		++id_t->msg_count;

		int rc = is_gaFile(dest_path) ;	

		if(rc == -1)
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			return 0;	
		}

		fl = open(dest_path, O_RDWR);
		
		int dim_temp = strlen(tmp_path) + L_TEMP;

		if(fl <= 0)
		{
			printf( "Errore nell'apertura della risorsa!!!! \n");
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			return 0;
		}

		if(rc == 0)
		{
			if (!lockWrite(fl))
			{
				printf( "Errore nel lock della risorsa!!!! \n");
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				close(fl);
				free(msg);
				return 0;
			}
			
			if(!isFile(dest_path))
			{
				printf( "la risorsa è stata cancellata in modo concorrente \n");
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_NORES , 0);
				send_TrMessage(connsd , msg);
				unlockFile(fl);
				close(fl);
				free(msg);
				return 0;
			}
		}

		if(rc == 1)
		{
			index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
			if(index == -1)
			{
				printf( "Errore nel lock logico e fisico della risorsa!!!! \n");
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				close(fl);
				free(msg);
				return 0;
			}
			
			if(is_gaFile(dest_path) != 1)
			{
				printf( "la risorsa è stata cancellata in modo concorrente \n");
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_NORES , 0);
				send_TrMessage(connsd , msg);
				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
				close(fl);
				free(msg);
				return 0;
			}
		}

		char path_save[dim_temp];
		sv = create_fileTemp(tmp_path , path_save , dim_temp);
		close(sv);
		cp_rewrite_file(dest_path ,  path_save ,max_line );


		update_log_res2(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO  , dest_path  , path_save);

		Server *list_vista;
		int len_vista;
		char name_dom[_DIM_DOM+1] ={0x0};
		short int riuscita = 0;
		
		int q ;
		for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
		{
			if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
			{
				break;
			}
			else
			{
				name_dom[q-1] = msg_i->res[q];
			}
		}

		list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
		
		if(len_vista < 0) // ci deve essere almeno una replica attiva
		{
			riuscita = 0;
		} 
		else
		{
			riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

			if(riuscita)
			{
				if(op_rep_tr2(msg_i ,_TR_RM , list_vista , len_vista ,id_t  ) )	//invio file a repliche ---------------------------------
				{					
					remove(dest_path);
					
					if(rc == 0)
					{
						unlockFile(fl);
					}

					if(rc == 1)
					{
						unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
					}
					
					close(fl);

					
					set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
					
					if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
						send_TrMessage(connsd , msg);
					
					riuscita = 1;
				}
				else
					riuscita = 0;
			}
			else
			{
				riuscita = 0;
			}
			free(list_vista);
		}

		if(riuscita)
		{
			remove(path_save);
			free(msg);
			return 1;
		}
		else
		{		
				
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);

			if(rc == 0)
			{
				unlockFile(fl);
			}
	
			if(rc == 1)
			{
				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
			}	
		
			close(fl);
			remove(path_save);			
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			return 0;
		}
	}
	return 0;
}



// operazione di creazione di una directory, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction
int transaction_mkdir(int connsd , Id_Trans *id_t , InitMessage *msg_i , char *remote_ip)
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
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				printf("il client: %s non è attivo \n" , remote_ip);
				return 0;
			}
		}
	}
	++id_t->msg_count;

	if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
	{
		printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
		memset(msg, '0' , sizeof(TrMessage) );

		if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		}
		else
		{
			memset(msg, '0' , sizeof(TrMessage) );
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
		}

		send_TrMessage(connsd , msg);

		free(msg);
		return 0;
	}

	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
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
		
		insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_MKDIR , dest_path , "\0" ,"\0" ,  -1 , -1 , LOG_PREPARA );
		
		++id_t->msg_count;

		if(!isDir(dest_path) )
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO);
		}
		else
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			free(msg);
			return 0;
		}
		
		Server *list_vista;
		int len_vista;
		char name_dom[_DIM_DOM+1] ={0x0};
		short int riuscita = 0;
		
		int q ;
		for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
		{
			if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
			{
				break;
			}
			else
			{
				name_dom[q-1] = msg_i->res[q];
			}
		}

		list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
		
		if(len_vista < 0) // ci deve essere almeno una replica attiva
		{
			riuscita = 0;
		} 
		else
		{
			riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

			if(riuscita)
			{
				if(op_rep_tr2(msg_i ,_TR_MKDIR , list_vista , len_vista ,id_t  ) )	//invio file a repliche ---------------------------------
				{
					createDirectory(dest_path);
					
					set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
					
					if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
						send_TrMessage(connsd , msg);
					
					riuscita = 1;
				}
				else
					riuscita = 0;
			}
			else
			{
				riuscita = 0;
			}
			free(list_vista);
		}

		if(riuscita)
		{
			free(msg);
			return 1;
		}
		else
		{		
			removeDirectory(dest_path); // ritorno al vecchio stato con una redo
				
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			return 0;
		}
	}
	return 0;
}



// operazione di creazione di un file GA, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction
int transaction_new_fileGa(int connsd , Id_Trans *id_t , InitMessage *msg_i , char *remote_ip) 
{
	TrMessage* msg;
	short int b = 0;
	msg =  calloc(1 , sizeof( TrMessage) );
	int fl;
	int index = -1;
	
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
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				printf("il client: %s non è attivo \n" , remote_ip);
				return 0;
			}
		}
	}
	
	++id_t->msg_count;

	if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
	{
		printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
		memset(msg, '0' , sizeof(TrMessage) );

		if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
		}

		send_TrMessage(connsd , msg);

		free(msg);
		return 0;
	}

	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
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
		
		insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_NEW_GA , dest_path , "\0" ,"\0" ,  -1 , -1 , LOG_PREPARA );

		++id_t->msg_count;

		if(!isFile(dest_path) )
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO);
		}
		else
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			free(msg);
			return 0;
		}

		fl = open(dest_path, O_RDWR);
		
		if(cp_rewrite_file("model_ga.ga" , dest_path ,max_line ) && ( index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex) ) != -1 )
		{

			Server *list_vista;
			int len_vista;
			char name_dom[_DIM_DOM+1] ={0x0};
			short int riuscita = 0;
			
			int q ;
			for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
			{
				if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
				{
					break;
				}
				else
				{
					name_dom[q-1] = msg_i->res[q];
				}
			}

			list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
			
			if(len_vista < 0) // ci deve essere almeno una replica attiva
			{
				riuscita = 0;
			} 
			else
			{
				riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

				if(riuscita)
				{
					if(op_rep_tr1(msg_i ,_TR_NEW_GA , list_vista , len_vista , fl , id_t ) )	//invio file a repliche ---------------------------------
					{
						set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
						if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
							send_TrMessage(connsd , msg);
						riuscita = 1;
					}
					else
						riuscita = 0;
				}
				else
				{
					riuscita = 0;
				}
				free(list_vista);
			}

			if(riuscita)
			{
				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);

				close(fl);
				
				free(msg);

				return 1;
			}
			else
			{
				remove(dest_path); // ritorno al vecchio stato con una redo
					
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);

				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);

				close(fl);
	
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				free(msg);

				return 0;
			}
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
			
			close(fl);	
			b = 0;
		}
		free(msg);
	}
	
	if(b == 0)
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
		
	return b;
}



// operazione di inserimento di un capitolo in un file GA, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction,  posizione del nuovo capitoloe dimensione 
int transaction_newCap(int connsd , Id_Trans *id_t , InitMessage *msg_i  , int opt1 , int opt2 , char *remote_ip) // da finire
{
	int fl;
	int index = -1;
	int sv;	
	int dim  = _DIM_RES * 2;
	char dest_path[dim];
	memset(dest_path , '\0' ,  dim);
	strncat(dest_path, fs_path ,dim);
	strncat(dest_path, msg_i->res , msg_i->l_res);
	
	int rc = is_gaFile(dest_path) ;	

	if(rc <= 0)
	{	
		printf("La risorsa non è un file GA\n");
		return 0;	
	}
	
	int dim_temp = strlen(tmp_path) + L_TEMP;
	char path_filecap[dim_temp];

	int fc = create_fileTemp(tmp_path , path_filecap , dim_temp);

	if(write_control_res(connsd  , fc,  max_line , opt2) )
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
				close(fc);
				remove(path_filecap) ;
				free(msg);
				return 0;
			}

			if( b_recv == 0 )
			{			
				if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
				{
					printf("il client: %s non è attivo \n" , remote_ip);
					return 0;
				}
			}
		}
		
		++id_t->msg_count;
	
		if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
			memset(msg, '0' , sizeof(TrMessage) );
		
			if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
			{
				set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
			}
			else
			{
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
			}
		
			send_TrMessage(connsd , msg);
		
			close(fc);
			remove(path_filecap) ;
			free(msg);
			return 0;
		}
		
		if(msg->pay_desc != _TR_COMMIT)
		{
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
		
			close(fc);
			remove(path_filecap) ;
			free(msg);
			return 0;
		}
		else
		{	
			insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_NEW_CAP , dest_path , "\0" ,"\0" ,  opt1 , -1 , LOG_PREPARA );
			++id_t->msg_count;
			
			if( !anal_ga(fc) )
			{
				set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_NOFORMAT_CAP , 0);
				send_TrMessage(connsd , msg);

				close(fc);
				remove(path_filecap) ;
				free(msg);
				return 0;
			}
	
			fl = open(dest_path, O_RDWR);

			if(fl <= 0)
			{
				printf( "Errore nell'apertura della risorsa!!!! \n");
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				close(fc);
				remove(path_filecap) ;
				free(msg);
				return 0;
			}
		
			index = -1;

			index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
			if(index == -1)
			{
				printf( "Errore nel lock della risorsa!!!! \n");
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				close(fc);
				remove(path_filecap) ;
				close(fl);
				free(msg);
				return 0;
			}
			
			if(is_gaFile(dest_path)  !=1)
			{
				printf( "la risorsa è stata cambiata in modo concorrente!!!! \n");
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_NORES , 0);
				send_TrMessage(connsd , msg);
				close(fc);
				remove(path_filecap) ;
				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
				close(fl);
				free(msg);
				return 0;
			}
			
			char path_save[dim_temp];
			sv = create_fileTemp(tmp_path , path_save , dim_temp);
			close(sv);
			
			if( !cp_rewrite_file(dest_path ,  path_save ,max_line ))
			{
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				close(fc);
				remove(path_filecap) ;
				remove(path_save) ;	
				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
				close(fl);
				free(msg);
				return 0;
			}	
			
			char path_filetemp[dim_temp];

			int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
			int id_cap;	
			
			update_log_res2(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO  , dest_path  , path_save);
			
			if(( id_cap = insert_CapGA( fl , fc , fd , opt1 , tmp_path) ) == 0)
			{
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				remove(path_save) ;		
				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);		
				close(fl);		
				close(fc);
				close(fd);
				free(msg);
				remove(path_filetemp) ;	
				return 0;
			}
			
			Server *list_vista;
			int len_vista;
			char name_dom[_DIM_DOM+1] ={0x0};
			short int riuscita = 0;
			
			int q ;
			for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
			{
				if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
				{
					break;
				}
				else
				{
					name_dom[q-1] = msg_i->res[q];
				}
			}

			list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
			
			if(len_vista < 0) // ci deve essere almeno una replica attiva
			{
				riuscita = 0;
			} 
			else
			{
				riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

				if(riuscita)
				{
					if(op_rep_tr1(msg_i ,_TR_NEW_CAP , list_vista , len_vista , fd , id_t  ) )	//invio file a repliche
					{
						write_res_on_res(fd , fl , max_line );
						lista_File[index].lista = addItemAfterPos(1 , id_cap , lista_File[index].lista);		
						
						set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
						if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
							send_TrMessage(connsd , msg);
						riuscita = 1;
					}
					else
						riuscita = 0;
				}
				else
				{
					riuscita = 0;
				}
				free(list_vista);
			}

			if(riuscita)
			{
				close(fd);

				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
		
				close(fl);
				remove(path_save);	
				remove(path_filetemp) ;
				
				free(msg);
				return 1;
			}
			else
			{
				sv = open(path_save, O_RDWR);
				write_res_on_res( sv, fl  ,max_line ); // riporto il file al vecchio stato con una redo
				close(sv);
					
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
				close(fd);

				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
				close(fl);

				remove(path_save);
				remove(path_filetemp) ;
				
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				free(msg);

				return 0;
			}	
		}
		free(msg);
	}
	else
	{	
		close(fc);
		remove(path_filecap) ;
		return 0;
	}	

						 
	close(fc);
	remove(path_filecap) ;
	return 1;
}



// operazione di move sul capitolo del file Ga, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction, capitolo da eliminare
int transaction_moveCap(int connsd , Id_Trans *id_t , InitMessage *msg_i  , int opt1 , int opt2 , char *remote_ip)
{

	int fl;
	int index_lock = -1;
	int sv;
	int dim  = _DIM_RES * 2;
	char dest_path[dim];
	memset(dest_path , '\0' ,  dim);
	strncat(dest_path, fs_path ,dim);
	strncat(dest_path, msg_i->res , msg_i->l_res);
	
	int rc = is_gaFile(dest_path) ;	

	if(rc <= 0)
	{	
		printf("La risorsa non è un file GA\n");
		return 0;	
	}
	
	int dim_temp = strlen(tmp_path) + L_TEMP;
	char path_filetemp[dim_temp];

	int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);

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
			close(fd);
			remove(path_filetemp) ;		
			free(msg);
			return 0;
		}

		if( b_recv == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				printf("il client: %s non è attivo \n" , remote_ip);
				return 0;
			}
		}	
	}

	++id_t->msg_count;

	if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
	{
		printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
		memset(msg, '0' , sizeof(TrMessage) );

		if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		}
		else
		{
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
		}

		send_TrMessage(connsd , msg);

		close(fd);
		remove(path_filetemp) ;
		free(msg);
		return 0;
	}

	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		send_TrMessage(connsd , msg);

		close(fd);
		remove(path_filetemp) ;
		free(msg);
		return 0;
	}
	else
	{
		insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip ,ntohs(*port_client) , _TR_MV_CAP , dest_path , "\0" ,"\0" ,  opt1 , opt2 , LOG_PREPARA );
	
		++id_t->msg_count;
		
		fl = open(dest_path, O_RDWR);

		if(fl <= 0)
		{
			printf( "Errore nell'apertura della risorsa!!!! \n");
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			close(fd);
			remove(path_filetemp) ;		
			free(msg);
			return 0;
		}

		index_lock = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
		if(index_lock == -1)
		{	
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			printf( "Errore nel lock della risorsa!!!! \n");
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			close(fd);
			remove(path_filetemp) ;
			close(fl);
			free(msg);
			return 0;
		}
		
		if(is_gaFile(dest_path) != 1)
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			printf( "La risorsa è stata modificata in modo concorrente!!!! \n");
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			close(fd);
			remove(path_filetemp) ;
			unlockAllChapters(index_lock, fl , lista_File , thread_helper_c , list_mutex);
			close(fl);
			free(msg);
			return 0;
		}
		
		char path_save[dim_temp];
		sv = create_fileTemp(tmp_path , path_save , dim_temp);
		close(sv);	
		
		if( !cp_rewrite_file(dest_path ,  path_save , max_line ) )
		{
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				close(fd);
				remove(path_filetemp) ;
				remove(path_save) ;	
				unlockAllChapters(index_lock, fl , lista_File , thread_helper_c , list_mutex);
				close(fl);
				free(msg);
				return 0;
		}	
		
		update_log_res2(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO  , dest_path  , path_save);
		
		if(!move_PosCapGA(fl, fd, opt1, opt2 , tmp_path) )
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);

			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			
			close(fd);
			remove(path_save) ;	
			remove(path_filetemp) ;
			unlockAllChapters(index_lock, fl , lista_File , thread_helper_c , list_mutex);
			close(fl);
			free(msg);
			return 0;
		}

		Server *list_vista;
		int len_vista;
		char name_dom[_DIM_DOM+1] ={0x0};
		short int riuscita = 0;
		
		int q ;
		for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
		{
			if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
			{
				break;
			}
			else
			{
				name_dom[q-1] = msg_i->res[q];
			}
		}

		list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
		
		if(len_vista < 0) // ci deve essere almeno una replica attiva
		{
			riuscita = 0;
		} 
		else
		{
			riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

			if(riuscita)
			{
				if(op_rep_tr1(msg_i ,_TR_MV_CAP , list_vista , len_vista , fd , id_t  ) )	//invio file a repliche
				{
					write_res_on_res(fd , fl , max_line );
					
					set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
					if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
						send_TrMessage(connsd , msg);
					riuscita = 1;
				}
				else
					riuscita = 0;
			}
			else
			{
				riuscita = 0;
			}
			free(list_vista);
		}

		if(riuscita)
		{
			close(fd);

			unlockAllChapters(index_lock, fl , lista_File , thread_helper_c , list_mutex);
	
			close(fl);
			remove(path_save);	
			remove(path_filetemp) ;
			
			free(msg);
			return 1;
		}
		else
		{
			sv = open(path_save, O_RDWR);
			write_res_on_res( sv, fl  ,max_line ); // riporto il file al vecchio stato con una redo
			close(sv);
				
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			close(fd);

			unlockAllChapters(index_lock, fl , lista_File , thread_helper_c , list_mutex);
			close(fl);

			remove(path_save);
			remove(path_filetemp) ;
			
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			free(msg);

			return 0;
		}
	}
		
	free(msg);

	close(fd);
	remove(path_filetemp) ;

	return 1;
}


// operazione di remove directory, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction, capitolo da eliminare
int transaction_removeCap(int connsd , Id_Trans *id_t , InitMessage *msg_i  , int opt1 , char *remote_ip)
{
	int fl;	
	int index = -1;
	int sv;
	int dim  = _DIM_RES * 2;
	char dest_path[dim];
	memset(dest_path , '\0' ,  dim);
	strncat(dest_path, fs_path ,dim);
	strncat(dest_path, msg_i->res , msg_i->l_res);
	
	int rc = is_gaFile(dest_path) ;	

	if(rc <= 0)
	{	
		printf("La risorsa non è un file GA\n");
		return 0;	
	}
	
	int dim_temp = strlen(tmp_path) + L_TEMP;
	char path_filetemp[dim_temp];

	int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);

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
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}

		if( b_recv == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				printf("il client: %s non è attivo \n" , remote_ip);
				return 0;
			}
		}	
	}
	
	++id_t->msg_count;

	if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
	{
		printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
		memset(msg, '0' , sizeof(TrMessage) );

		if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
		}

		send_TrMessage(connsd , msg);

		close(fd);
		remove(path_filetemp) ;
		free(msg);
		return 0;
	}

	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		send_TrMessage(connsd , msg);

		close(fd);
		remove(path_filetemp) ;
		free(msg);
		return 0;
	}
	else
	{
		insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_RM_CAP , dest_path , "\0" ,"\0" ,  opt1 , -1 , LOG_PREPARA );
	
		++id_t->msg_count;
	

		fl = open(dest_path, O_RDWR);

		if(fl <= 0)
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			printf( "Errore nell'apertura della risorsa!!!! \n");
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}

		index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);

		if(index == -1)
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			printf( "Errore nel lock della risorsa!!!! \n");
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			
			close(fl);
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
		
		if(is_gaFile(dest_path) != 1)
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			printf( "La risorsa è stata modifocata in maniera concorrente!!!! \n");
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count , _ERR_TR_NORES , 0);
			send_TrMessage(connsd , msg);
			unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
			close(fl);
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
		
		char path_save[dim_temp];
		sv = create_fileTemp(tmp_path , path_save , dim_temp);
		close(sv);	
		
		if( !cp_rewrite_file(dest_path ,  path_save ,max_line ) )
		{
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
				send_TrMessage(connsd , msg);
				unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
				close(fl);
				close(fd);
				remove(path_filetemp) ;
				remove(path_save) ;	
				free(msg);
				return 0;
		}	
		
		update_log_res2(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO  , dest_path  , path_save);
		
		if( !extract_excluse_IdCapGA(fl, fd, opt1) )
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);

			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
			close(fl);
			close(fd);
			remove(path_save) ;	
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
		
		Server *list_vista;
		int len_vista;
		char name_dom[_DIM_DOM+1] ={0x0};
		short int riuscita = 0;
		
		int q ;
		for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
		{
			if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
			{
				break;
			}
			else
			{
				name_dom[q-1] = msg_i->res[q];
			}
		}

		list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
		
		if(len_vista < 0) // ci deve essere almeno una replica attiva
		{
			riuscita = 0;
		} 
		else
		{
			riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

			if(riuscita)
			{
				if(op_rep_tr1(msg_i ,_TR_RM_CAP  , list_vista , len_vista , fd , id_t  ) )	//invio file a repliche
				{
					write_res_on_res(fd , fl , max_line );
					set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
					if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
						send_TrMessage(connsd , msg);
					riuscita = 1;
				}
				else
					riuscita = 0;
			}
			else
			{
				riuscita = 0;
			}
			free(list_vista);
		}

		if(riuscita)
		{
			close(fd);
			unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
			close(fl);
			remove(path_save);	
			remove(path_filetemp) ;
			
			free(msg);
			return 1;
		}
		else
		{
			sv = open(path_save, O_RDWR);
			write_res_on_res( sv, fl  ,max_line ); // riporto il file al vecchio stato con una redo
			close(sv);
				
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			close(fd);

			unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex);
			close(fl);

			remove(path_save);
			remove(path_filetemp) ;
			
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			free(msg);

			return 0;
		}			
	}
	
	free(msg);
	close(fd);
	remove(path_filetemp) ;
	return 1;
}



// operazione di remove directory, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction
int transaction_removeDir(int connsd , Id_Trans *id_t , InitMessage *msg_i , char *remote_ip)
{
	TrMessage* msg;
	short int b = 0;
	msg =  calloc(1 , sizeof( TrMessage) );
	
	while(1)
	{
		msg =  memset(msg , 0 , sizeof( TrMessage) );
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
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				printf("il client: %s non è attivo \n" , remote_ip);
				return 0;
			}
		}	
	}
	
	++id_t->msg_count;

	if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
	{
		printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
		memset(msg, '0' , sizeof(TrMessage) );

		if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
		}
		else
		{
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
		}

		send_TrMessage(connsd , msg);

		free(msg);
		return 0;
	}

	if(msg->pay_desc != _TR_COMMIT)
	{
		set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
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

		insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_RM_DIR , dest_path , "\0" ,"\0" ,  -1 , -1 , LOG_PREPARA );

		++id_t->msg_count;


		if(isDir(dest_path) )
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO);
		}
		else
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			free(msg);
			return 0;
		}
		
		Server *list_vista;
		int len_vista;
		char name_dom[_DIM_DOM+1] ={0x0};
		short int riuscita = 0;
		
		int q ;
		for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
		{
			if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
			{
				break;
			}
			else
			{
				name_dom[q-1] = msg_i->res[q];
			}
		}

		list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
		
		if(len_vista < 0) // ci deve essere almeno una replica attiva
		{
			riuscita = 0;
		} 
		else
		{
			riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

			if(riuscita)
			{
				removeDirectory(dest_path);
				
				if(op_rep_tr2(msg_i ,_TR_RM_DIR , list_vista , len_vista ,id_t  ) )	//invio file a repliche ---------------------------------
				{
					
					set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
					
					if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
						send_TrMessage(connsd , msg);
					
					riuscita = 1;
				}
				else
					riuscita = 0;
			}
			else
			{
				riuscita = 0;
			}
			free(list_vista);
		}

		if(riuscita)
		{
			free(msg);
			return 1;
		}
		else
		{		
			createDirectory(dest_path); // ritorno al vecchio stato con una redo
				
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			return 0;
		}
	}
	
	if(b == 0)
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
			
	return b;
}



// operazione di modifica dei capitoli del file Ga, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction, capitolo da eliminare
int transaction_modCap(int connsd , Id_Trans *id_t , InitMessage *msg_i  , int opt1 , int opt2 , char *remote_ip)
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
	
	int rc = is_gaFile(dest_path) ;	

	if(rc <= 0)
	{	
		printf("La risorsa non è un file GA\n");
		free(msg);
		return 0;	
	}
	
	
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
	int sizePatch = opt2;
	descrittori[0] = create_fileTemp(tmp_path , path_mod_file[0] , dim_temp);

	insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_MOD_CAP , dest_path , path_mod_file[0] , "\0" ,  opt1 , -1 ,  						 LOG_PREPARA );

	if(write_control_res(connsd  , descrittori[0],  max_line , opt2) )
	{ 			
		if( !anal_ga(descrittori[0]) )
		{
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_NOFORMAT_CAP , 0);
			send_TrMessage(connsd , msg);

			close(descrittori[0]);
			remove(path_mod_file[0]);
			free(msg);
			return 0;
		}	
	}
	else
	{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_GE , 0);
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
				set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
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
				if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
				{
					printf("il client: %s non è attivo \n" , remote_ip);
					
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
		
		++id_t->msg_count;
		
		if( id_t->msg_count != msg->msg_count || id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
		{	
			printf("\e[1;31mErrore: perdita di alcuni pacchetti o  sull'id trasazione\033[m \n");
			memset(msg, '0' , sizeof(TrMessage) );
		
			if( id_t->id_p != msg->id_primary || id_t->id_c != msg->id_count )
			{
				set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 , _ERR_TR_BIZ , 0);
			}
			else
			{
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_MISSPACK , 0);
			}
		
			send_TrMessage(connsd , msg);
		
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
				close(descrittori[i]);
				remove(path_mod_file[i]);
			}
			free(msg);
			return 0;
		}

		if(msg->pay_desc != _TR_COMMIT && msg->pay_desc != _TR_MOD_CAP)
		{
			set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 , _ERR_TR_BIZ , 0);
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

				insert_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , remote_ip , ntohs(*port_client) , _TR_MOD_CAP , dest_path , path_mod_file[m] , "\0" ,  msg->opt1 , 										 -1 ,  LOG_PREPARA );

				if(write_control_res(connsd  , descrittori[m],  max_line , msg->opt2) )
				{
					if( !anal_ga(descrittori[m]) )
					{
						set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_NOFORMAT_CAP , 0);
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
					update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
					set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
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
		++id_t->msg_count;	
		while(1)
		{
			memset(msg , 0 , sizeof( TrMessage) ); 
			int b_recv = recv_TrMessage( connsd , msg) ;	
	
			if( b_recv  > 0 && msg->pay_desc == _TR_COMMIT )
				break;
			
			++id_t->msg_count;	// incrementato solo se si deve inviare un msg di errore
			
			if( b_recv  > 0 && msg->pay_desc != _TR_COMMIT )
			{
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
				set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_BIZ , 0);
				send_TrMessage(connsd , msg);
			
				for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
				{	
					close(descrittori[i]);
					remove(path_mod_file[i]);
				}
				free(msg);
				return 0;
			}

			if(b_recv < 0)
			{
				update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
				set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
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
				if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP ,my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
				{
					printf("il client: %s non è attivo \n" , remote_ip);
					update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);					
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
	
	++id_t->msg_count;	

	fl = open(dest_path, O_RDONLY);

	if(fl <= 0)
	{
		printf( "Errore nell'apertura della risorsa!!!! \n");
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
		
		set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
		send_TrMessage(connsd , msg);
		
		for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
		{
			remove(path_mod_file[i]);	
		}
		free(msg);
		free(l_mod_cap);
		return 0;
	}
	
	struct list_el *list_el_tmp[len_list];
	for(i = 0 ; i < len_list ; ++i) // prendo il lock logico dei capitoli
	{
		list_el_tmp[i] = lockLogicalWrite(fl, l_mod_cap[i].id_cap , thread_helper_c , lista_File , list_mutex);
		
		if( list_el_tmp[i] == NULL )
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
		
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count  ,  _ERR_TR_GE , 0);
			send_TrMessage(connsd , msg);
			
			int v = 0;
			for(v = 0 ; v < i ; ++v) // rilascio il lock logico dei capitoli
			{
				unlockChapter(list_el_tmp[v] , fl , thread_helper_c , lista_File , list_mutex);
			}
		
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
				remove(path_mod_file[i]);	
			}
			
			close(fl);
			free(msg);
			free(l_mod_cap);
			return 0;
		}
	}
	
	lockRead(fl);
	for(i = 0 ; i < len_list ; ++i) // effettuo le copie di salvataggio dei capitoli
	{
		int sv = create_fileTemp(tmp_path , l_mod_cap[i].path_sav_cap , dim_temp);
		
		if (extract_from_IdCapGA(fl , sv, l_mod_cap[i].id_cap) )
		{
			update_log_res3(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PREPARA , dest_path   , l_mod_cap[i].path_mod_cap  , l_mod_cap[i].path_sav_cap);
			close(sv);
		}
		else
		{
			update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
		
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
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
			
			for(j = 0 ; j < len_list ; ++j) // rilascio il lock logico dei capitoli
			{
				unlockChapter(list_el_tmp[j] , fl , thread_helper_c , lista_File , list_mutex);
			}
			
			unlockFile(fl);
			close(sv);
			close(fl);
			free(l_mod_cap);
			free(msg);
			return 0;
		}
	}
	unlockFile(fl);
	
	int fall = 0;
	
	for(i = 0 ; i < len_list ; ++i) // calcolo le patch
	{
		int sv = open(l_mod_cap[i].path_sav_cap, O_RDONLY);
		char modifica[sizePatch + 1];
		memset(modifica, '\0', sizePatch + 1);
		int md = open(l_mod_cap[i].path_mod_cap, O_RDWR);
		readn(md, modifica, sizePatch);
		printf("Modifica = %s\n", modifica);
		char ftemp[1024] ={0x0};

		if( applyPatchToChapter(sv , modifica, tmp_path , ftemp) == 0)
		{
			fall = 1 ;
			close(sv);
			close(md);
			break;
		}
		
		cp_rewrite_file(ftemp , l_mod_cap[i].path_mod_cap , max_line );
		
		close(sv);
		close(md);
		remove(ftemp);
	}
	
	if(fall && (isFile(dest_path)  == 0) )
	{
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);
	
		set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
		send_TrMessage(connsd , msg);

		for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
		{
			remove(l_mod_cap[i].path_mod_cap);	
			remove(l_mod_cap[i].path_sav_cap);	
		}
		
		for(i = 0 ; i < len_list ; ++i) // rilascio il lock logico dei capitoli
		{
			unlockChapter(list_el_tmp[i] , fl , thread_helper_c , lista_File , list_mutex);
		}
		
		close(fl);
		free(l_mod_cap);
		free(msg);
		return 0;
	}

	update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_PRONTO);					

	Server *list_vista;
	int len_vista;
	char name_dom[_DIM_DOM+1] ={0x0};
	short int riuscita = 0;
	
	int q ;
	for( q  = 1 ; ( (q < msg_i->l_res) && (q < _DIM_DOM+1 )) ; ++q)
	{
		if(strncmp(&(msg_i->res[q]) , "/" , 1) == 0 )
		{
			break;
		}
		else
		{
			name_dom[q-1] = msg_i->res[q];
		}
	}

	list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
	
	if(len_vista < 0) // ci deve essere almeno una replica attiva
	{
		riuscita = 0;
	} 
	else
	{
		riuscita = control_replic(list_vista , &len_vista , PATH_DB_TMPDOM , name_dom);

		if(riuscita)
		{
			if(op_rep_tr3(msg_i ,_TR_RM_CAP  , list_vista , len_vista , l_mod_cap , len_list ,id_t  ) )	//invio file a repliche
			{
				close(fl);
				fl = open(dest_path, O_RDWR);
				
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
				
				set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
				if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
					send_TrMessage(connsd , msg);
				riuscita = 1;
			}
			else
				riuscita = 0;
		}
		else
		{
			riuscita = 0;
		}
		free(list_vista);
	}

	if(riuscita)
	{
		for(i = 0 ; i < len_list ; ++i) // rilascio il lock logico dei capitoli
		{
			unlockChapter(list_el_tmp[i] , fl , thread_helper_c , lista_File , list_mutex);
		}

		close(fl);
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
	{	// non faccio la redo visto che scrivo solo a operazione riuscita sulle repliche
		update_log(PATH_DB_LOG , id_t->id_p , id_t->id_c , LOG_ABORT);

		for(i = 0 ; i < len_list ; ++i) // rilascio il lock logico dei capitoli
		{
			unlockChapter(list_el_tmp[i] , fl , thread_helper_c , lista_File , list_mutex);
		}
		close(fl);

		for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
		{
			remove(l_mod_cap[i].path_mod_cap);	
			remove(l_mod_cap[i].path_sav_cap);	
		}
	
		set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_GE , 0);
		send_TrMessage(connsd , msg);
		free(msg);
		free(l_mod_cap);
		return 0;
	}
	
	free(msg);
	free(l_mod_cap);
	return 0;
}



// Inizializza la struttura List_ModCap, e la ordina per avere id_cap crescenti
// Parametri: lunghezza vettore, path dei capitoli modificati, id dei capitoli 
//nota : ogi id_cap deve essere diverso	
List_ModCap *init_List_ModCap( int len_list , char path_mod_file[][_DIM_RES] ,  int *id_cap) 
{
	List_ModCap *b;
	b = ( List_ModCap *) calloc ( len_list , sizeof(List_ModCap) );
	
	int i , j;
	
	for( i = 0 ; i < len_list ; ++i)
		b[i].id_cap = id_cap[i] ;
	 
	int temp_int;
	int alto = len_list ; /* len_list è il numero degli elementi del vettore da ordinare */
	
	while (alto > 1) /* in questo modo si evita 1 passaggio*/
 	{ 
		for (i=0; i<alto-1; ++i)
		{
			if (b[i].id_cap > b[i+1].id_cap) /* sostituire ">" con "<" per avere un ordinamento decrescente */
			{ 
					temp_int = b[i].id_cap; 
					b[i].id_cap = b[i+1].id_cap; 
					b[i+1].id_cap = temp_int;
					
			} 
		}
		--alto;
	}

	j = 0;
	while(j < len_list)
	{
		for( i = 0 ; i < len_list ; ++i)
		{
			if(b[j].id_cap == id_cap[i])
			{
				strncpy( b[j].path_mod_cap , path_mod_file[i] , _DIM_RES );
				i = len_list;
				++j ;
			}
		}
	}
	return b ;
}	



// effettua la redo dell'operazione di mod_cap
int redo_mod_cap( List_ModCap *list , int len , char *orig )
{
	int i ;
	
	for( i = 0  ;   i < len ; ++i)
	{	
		int fl;
		int fr;
		fl = open(orig, O_RDONLY);
		lockRead(fl);
		int fc;
		fc = open(list[i].path_sav_cap, O_RDONLY);
		char result_file[_DIM_RES] = {0x0};
		
		if(mod_CapGA(fl, fc , result_file ,list[i].id_cap , tmp_path) == 0)
		{
		
			unlockFile(fl);
			close(fc);
			close(fl);	
			printf("ERRORE: Ripristino non possibile la risorsa rimane in unno stato inconsistente !! \n");
			return 0; 
		}
		
		unlockFile(fl);
		close(fc);
		close(fl);
		fl = open(orig, O_RDWR);
		fr = open(result_file, O_RDWR);
		lockWrite(fl);
		write_res_on_res(fr , fl , max_line );
		unlockFile(fl);
		close(fl);
		close(fr);
		remove(result_file);
	}
	
	return 1;
}




