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

	int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
	int b ;

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
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
		
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
		else
		{
			
			++id_t->msg_count;
			
			set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			
			int dim  = _DIM_RES * 2;
			char dest_path[dim];
			memset(dest_path , '\0' ,  dim);
			strncat(dest_path, fs_path ,dim);
			strncat(dest_path, msg_i->res , msg_i->l_res);
			
			short int exist_file = 0;
			
			exist_file = isFile(dest_path);
			int rc;
			int fl;
			int index;
			
			if(exist_file)
			{
				rc = is_gaFile(dest_path) ;	
		
				if(rc == -1)
				{
					close(fd);
					remove(path_filetemp) ;
					free(msg);
					return 0;	
				}
			
				fl = open(dest_path, O_RDONLY);
	
				if(fl <= 0)
				{
					printf( "Errore nell'apertura della risorsa!!!! \n");
					close(fd);
					remove(path_filetemp) ;
					free(msg);
					return 0;
				}
			
				if(rc == 0)
				{
					if (!lockWrite(fl))
					{
						printf( "Errore nel lock della risorsa!!!! \n");
						close(fd);
						remove(path_filetemp) ;
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
						printf( "Errore nel lock della risorsa!!!! \n");
						close(fd);
						remove(path_filetemp) ;			
						close(fl);
						free(msg);
						return 0;
					}
				}
			}
			
			//invio file a repliche e scrittura sul log se l'operazione va a buon fine
				
			if( cp_rewrite_file(path_filetemp , dest_path , max_line ) )
			{
				if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
					send_TrMessage(connsd , msg);
		
				b = 1;
			}
			else
			{
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
				
				if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
					send_TrMessage(connsd , msg);
					
				b = 0;
			}
			
			if(exist_file)
			{
				if(rc == 0)
				{
					if (!unlockFile(fl))
					{
						printf( "Errore nell'unlock della risorsa!!!! \n");
						close(fd);
						remove(path_filetemp) ;
						close(fl);
						free(msg);
						return 0;
					}
				}
			
				if(rc == 1)
				{
					if(!unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex))
					{
						printf( "Errore nell'unlock della risorsa!!!! \n");
						close(fd);
						remove(path_filetemp) ;
						close(fl);
						free(msg);
						return 0;
					}
				}	
				
				close(fl);
			}	
		}
		free(msg);
	}
	else
	{	
		b = 0;
	}	

	close(fd);
	remove(path_filetemp) ;
	return b;
}



// operazione di remove file, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction
int transaction_remove(int connsd , Id_Trans *id_t , InitMessage *msg_i  , char *remote_ip)
{
	TrMessage* msg;
	short int b = 0;
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
		//invio operazione remove a rapliche e scrittura sul log se l'operazione va a buon fine

		++id_t->msg_count;

		set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);


		int dim  = _DIM_RES * 2;
		char dest_path[dim];
		memset(dest_path , '\0' ,  dim);
		strncat(dest_path, fs_path ,dim);
		strncat(dest_path, msg_i->res , msg_i->l_res);

		int rc = is_gaFile(dest_path) ;	

		if(rc == -1)
		{
			free(msg);
			return 0;	
		}

		int fl;
		fl = open(dest_path, O_RDONLY);

		if(fl <= 0)
		{
			printf( "Errore nell'apertura della risorsa!!!! \n");
			free(msg);
			return 0;
		}

		if(rc == 0)
		{
			if (!lockWrite(fl))
			{
				printf( "Errore nel lock della risorsa!!!! \n");
				close(fl);
				free(msg);
				return 0;
			}
		}

		int index = -1;
		if(rc == 1)
		{
			index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
			if(index == -1)
			{
				printf( "Errore nel lock della risorsa!!!! \n");
				close(fl);
				free(msg);
				return 0;
			}
		}
		
		if( !remove(dest_path) )
		{
			if(rc == 1)
				remove_logicalFile(index , lista_File ); //rimuovo il file logico dalla lista
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
				send_TrMessage(connsd , msg);

			b = 1;
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			b = 0;
		}
	
		if(rc == 0)
		{
			if (!unlockFile(fl))
			{
				printf( "Errore nell'unlock della risorsa!!!! \n");
				close(fl);
				free(msg);
				return 0;
			}
		}
	
		if(rc == 1)
		{
			if(!unlockFile(fl))
			{
				printf( "Errore nell'unlock della risorsa!!!! \n");
				close(fl);
				free(msg);
				return 0;
			}
		
		close(fl);	
		}
		free(msg);

	}
	return b;
}


// operazione di creazione di una directory, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction
int transaction_mkdir(int connsd , Id_Trans *id_t , InitMessage *msg_i , char *remote_ip)
{
	TrMessage* msg;
	short int b = 0;
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
		//invio operazione mkdir a repliche e scrittura sul log se l'operazione va a buon fine

		++id_t->msg_count;

		set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);


		int dim  = _DIM_RES * 2;
		char dest_path[dim];
		memset(dest_path , '\0' ,  dim);
		strncat(dest_path, fs_path ,dim);
		strncat(dest_path, msg_i->res , msg_i->l_res);

		
		if( createDirectory(dest_path) )
		{
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
				send_TrMessage(connsd , msg);
				
			b = 1;
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			b = 0;
		}
		free(msg);

	}
	return b;
}



// operazione di creazione di un file GA, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction
int transaction_new_fileGa(int connsd , Id_Trans *id_t , InitMessage *msg_i , char *remote_ip) 
{
	TrMessage* msg;
	short int b = 0;
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
		//invio operazione di new_ga a repliche e scrittura sul log se l'operazione va a buon fine

		++id_t->msg_count;

		set_transaction_msg( msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);

		int dim  = _DIM_RES * 2;
		char dest_path[dim];
		memset(dest_path , '\0' ,  dim);
		strncat(dest_path, fs_path ,dim);
		strncat(dest_path, msg_i->res , msg_i->l_res);

		
		if( !isFile(dest_path) && cp_rewrite_file("model_ga.ga" , dest_path ,max_line ))
		{
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
				send_TrMessage(connsd , msg);
				
			b = 1;
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			b = 0;
		}
		free(msg);

	}
	return b;
}



// operazione di inserimento di un capitolo in un file GA, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction, dimensione e posizione del muovo capitolo
int transaction_newCap(int connsd , Id_Trans *id_t , InitMessage *msg_i  , int opt1 , int opt2 , char *remote_ip) // da finire
{
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
	int b ;

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
			if( !anal_ga(fc) )
			{
				set_transaction_msg( msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count +1 ,  _ERR_TR_NOFORMAT_CAP , 0);
				send_TrMessage(connsd , msg);

				close(fc);
				remove(path_filecap) ;
				free(msg);
				return 0;
			}
		
			//invio file a rapliche e scrittura sul log se l'operazione va a buon fine
			
			++id_t->msg_count;

			set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);

			int fl;
			int index;
		
			fl = open(dest_path, O_RDWR);

			if(fl <= 0)
			{
				printf( "Errore nell'apertura della risorsa!!!! \n");
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
				close(fc);
				remove(path_filecap) ;			
				close(fl);
				free(msg);
				return 0;
			}
			
			char path_filetemp[dim_temp];

			int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
			int id_cap;	
			if( (id_cap = insert_CapGA( fl , fc , fd , opt1 , tmp_path) ) && cp_rewrite_file(path_filetemp , dest_path , max_line ) )
			{
				lista_File->lista = addItemAfterPos(1 , id_cap , lista_File->lista);
				if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
					send_TrMessage(connsd , msg);
		
				b = 1;
			}
			else
			{
				set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
				
				if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
					send_TrMessage(connsd , msg);
					
				b = 0;
			}

			if(!unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex))
			{
				printf( "Errore nell'unlock della risorsa!!!! \n");
				close(fc);
				remove(path_filecap) ;
				close(fd);
				remove(path_filetemp) ;
				close(fl);
				free(msg);
				return 0;
			}
				
			close(fl);		
			close(fd);
			remove(path_filetemp) ;		
		}
		free(msg);
	}
	else
	{	
		b = 0;
	}	

	close(fc);
	remove(path_filecap) ;
	return b;
}



// operazione di move sul capitolo del file Ga, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction, capitolo da eliminare
int transaction_moveCap(int connsd , Id_Trans *id_t , InitMessage *msg_i  , int opt1 , int opt2 , char *remote_ip)
{

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

	int b ;
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
		//invio file a repliche e scrittura sul log se l'operazione va a buon fine
	
		++id_t->msg_count;

		set_transaction_msg( msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
	
	
		int fl;
		fl = open(dest_path, O_RDWR);

		if(fl <= 0)
		{
			printf( "Errore nell'apertura della risorsa!!!! \n");
			close(fd);
			remove(path_filetemp) ;		
			free(msg);
			return 0;
		}
		
		int index_lock = -1;

		index_lock = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
		if(index_lock == -1)
		{	
			printf( "Errore nel lock della risorsa!!!! \n");
			close(fd);
			remove(path_filetemp) ;
			close(fl);
			free(msg);
			return 0;
		}

		if( move_PosCapGA(fl, fd, opt1, opt2 , tmp_path) && cp_rewrite_file(path_filetemp , dest_path , max_line ) )
		{
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
				send_TrMessage(connsd , msg);

			b = 1;
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
		
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
			
			b = 0;
		}

		if(!unlockAllChapters(index_lock, fl , lista_File , thread_helper_c , list_mutex))
		{
			printf( "Errore nell'unlock della risorsa!!!! \n");
			close(fl);
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}

		close(fl);	
	}
	free(msg);

	close(fd);
	remove(path_filetemp) ;

	return b;
}


// operazione di remove directory, ritorna 1 se l'operazione va a buon fine, 0 se fallisce
// socket, struttura identificativa della transazione , messaggio di Init Start Transaction, capitolo da eliminare
int transaction_removeCap(int connsd , Id_Trans *id_t , InitMessage *msg_i  , int opt1 , char *remote_ip)
{

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
	int b ;

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
		//invio file a rapliche e scrittura sul log se l'operazione va a buon fine
	
		++id_t->msg_count;

		set_transaction_msg( msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
	
		int fl;
		fl = open(dest_path, O_RDWR);

		if(fl <= 0)
		{
			printf( "Errore nell'apertura della risorsa!!!! \n");
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
	
		int index = -1;
	
		index = lockAllChapters(fl , lista_File , thread_helper_c , list_mutex);
		if(index == -1)
		{
			printf( "Errore nel lock della risorsa!!!! \n");
			close(fl);
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
		
		if( extract_excluse_IdCapGA(fl, fd, opt1) && cp_rewrite_file(path_filetemp , dest_path , max_line ) )
		{
			remove_logicalFile(index , lista_File ); //rimuovo il file logico dalla lista

			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
				send_TrMessage(connsd , msg);

			b = 1;
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);
		
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
			
			b = 0;
		}

		if(!unlockAllChapters(index, fl , lista_File , thread_helper_c , list_mutex))
		{
			printf( "Errore nell'unlock della risorsa!!!! \n");
			close(fl);
			close(fd);
			remove(path_filetemp) ;
			free(msg);
			return 0;
		}
				
		close(fl);	
	}
	free(msg);

	close(fd);
	remove(path_filetemp) ;
	return b;
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
		//invio operazione remove a repliche e scrittura sul log se l'operazione va a buon fine

		++id_t->msg_count;

		set_transaction_msg(msg , _TR_COMMIT , id_t->id_p , id_t->id_c , id_t->msg_count ,  0 , 0);


		int dim  = _DIM_RES * 2;
		char dest_path[dim];
		memset(dest_path , '\0' ,  dim);
		strncat(dest_path, fs_path ,dim);
		strncat(dest_path, msg_i->res , msg_i->l_res);

		
		if( removeDirectory(dest_path) )
		{
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio del commint al client
				send_TrMessage(connsd , msg);
				
			b = 1;
		}
		else
		{
			set_transaction_msg(msg , _TR_ERROR , id_t->id_p , id_t->id_c , id_t->msg_count ,  _ERR_TR_FULLDIR , 0);
			
			if( send_TrMessage(connsd , msg) <= 0) // due prove di invio dell'errore al client
				send_TrMessage(connsd , msg);
				
			b = 0;
		}
		free(msg);
	}
	return b;
}
