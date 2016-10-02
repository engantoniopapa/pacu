#include "header.h"



// inizia la transazione
// Parametri -> socket di connessione , path della risorsa locale, path della risorsa remota, 
// elemento shell e descrizione dell'errore
int init_transaction(int sockfd , char *res_loc , char *res_net , int cap , int id, int op , Shell_El *shell , char *errore)
{	
	TrMessage *msg;
	Id_Trans Id_t;
	char remote_ip[16] = {0x0};
	short int b = 0;
	
	struct sockaddr remote_addr;
	memset(&remote_addr , '\0' , sizeof(struct sockaddr) );
	socklen_t len = sizeof(struct sockaddr_in);

	getpeername( sockfd , &remote_addr, &len );
	struct sockaddr_in *s = (struct sockaddr_in *)&remote_addr;

	strncpy( remote_ip , inet_ntoa(s->sin_addr) ,_DIM_IPV4-1);
	
	msg = (TrMessage *) calloc (  1 , sizeof(TrMessage)  );

	if(recv_TrMessage(sockfd , msg) > 0 )
	{
		if(msg->pay_desc == _TR_DO_ID)
		{	
			if( msg->msg_count != 1)
			{
				strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
				free(msg);
				return 0;
			}
			
			Id_t.msg_count = msg->msg_count;
			Id_t.id_p = msg->id_primary;
			Id_t.id_c = msg->id_count;
			
			free(msg);
			
			set_timeout(sockfd  , timeout_tr  , 0  , timeout_tr , 0 );	
			
			switch (op) 
			{
				case _SHELL_UPLOAD: 
				{
					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_UPLOAD , res_loc , res_net ,"\0" , -1 , -1 , LOG_PREPARA );
					b = opTr_upload(sockfd , remote_ip , &Id_t , res_loc , res_net , shell , errore );
				}
				break;

				case _SHELL_RM: 
				{

					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_RM , "\0" , res_net , "\0" ,-1 , -1 , LOG_PREPARA );
					b = opTr_remove(sockfd , remote_ip , &Id_t , res_net ,shell , errore );
				}
				break;
			
				case _SHELL_MKDIR: 
				{
					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_MKDIR , "\0" , res_net , "\0" ,-1 , -1 , LOG_PREPARA );
					b = opTr_mkdir(sockfd , remote_ip , &Id_t , res_net ,shell , errore );
				}
				break;

				case _SHELL_NEWGA: 
				{
					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_NEW_GA , "\0" , res_net , "\0" ,-1 , -1 , LOG_PREPARA );
					b = opTr_newGa(sockfd , remote_ip , &Id_t , res_net ,shell , errore );
				}
				break;
			
				case _SHELL_NEWCAP: 
				{
					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_NEW_CAP , "\0" , res_net ,"\0" , -1 , cap , LOG_PREPARA );
					b = opTr_newCap(sockfd , remote_ip , &Id_t , res_net ,shell, cap , errore );
				}
				break;
			
				case _SHELL_MVCAP: 
				{
					if( id > 0 )
					{
						insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_MV_CAP , res_loc , res_net , "\0" ,id , cap , LOG_PREPARA );
						b = opTr_mvCap(sockfd , remote_ip , res_loc , &Id_t , res_net ,shell, id , cap , errore );
					}
					else
						b = 0;
				}
				break;

				case _SHELL_MODCAP: 
				{
					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_MOD_CAP , res_loc , res_net , "\0" , -1 , -1 , LOG_PREPARA );
					b = opTr_ModCap(sockfd , remote_ip , res_loc , &Id_t , res_net , shell, cap, errore );
				}
				break;
			
				case _SHELL_RMCAP: 
				{
					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_RM_CAP , res_loc , res_net , "\0" ,id , -1 , LOG_PREPARA );
					b = opTr_removeCap(sockfd , remote_ip , res_loc , &Id_t , res_net ,shell, id , errore );
				}
				break;
			
				case _SHELL_RMDIR: 
				{
					insert_log(PATH_DB_LOG , Id_t.id_p , Id_t.id_c , remote_ip , shell->port , _TR_RM_DIR , "\0" , res_net , "\0" , -1 , -1 , LOG_PREPARA );
					b = opTr_removeDir(sockfd , remote_ip , &Id_t , res_net ,shell , errore );
				}
				break;
				
				default:
				{
					strcpy(errore , "\e[1;31mErrore: operazione sconosciuta \033[m \n");
					b = 0;
				}
				break; 
      }			

			return b;
		}
		else
		{
			if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
			{
				free(msg);
				return 0;
			}
		}
	}
	else	
	{
		strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n");
		free(msg);
		return 0;
	}
	
	free(msg);
	return 0;
}	


// gestisce il tipo di errore, ritorna 0 
// Parametri socket di connessione , messaggio da controllare , elemento shell e descrizione dell'errore
int ctrl_errorTR(int sockfd , TrMessage * msg , Shell_El *shell , char *errore )
{
	int short b;
	
	if(msg->pay_desc == _TR_ERROR)
	{			
		print_TrError(msg->opt1 , errore);
	
		if(msg->opt1 == _ERR_TR_NODOM)
		{
			IDT_Message* buffer;
			int rs;
			
			buffer = (IDT_Message *) calloc ( 1 , sizeof(IDT_Message));		
			rs = recv_IDT_Message( sockfd, buffer);
			
			if(  rs && buffer->pay_desc == _IDT_PRIMARY)
			{
				strncpy( shell->ip , buffer->ip , _DIM_IPV4);
				shell->port = buffer->port;
		
				char server[_DIM_NAME_SERVER +1];
				memset( server , '\0' , _DIM_NAME_SERVER+1 );
				strncpy(server , buffer->name , _DIM_NAME_SERVER);
		
				char dom[_DIM_SHELL_DOM ];
				memset( dom , '\0' , _DIM_SHELL_DOM );
				strncpy(dom , &(shell->path_dom[1]) , _DIM_SHELL_DOM);
				b = update_dom_primary( dom , server  , shell->ip , shell->port , PATH_DB_TMP_PRI , PATH_DB_TMPDOM);
		
				if(b)
				{
					get_list_server(PATH_DB_TMP_PRI);
					get_list_domini(PATH_DB_TMPDOM);
					strcpy(errore , "\e[1;31mAggiornamento dominio eseguito con successo riprovare il comando \033[m \n");
				}		
			}
			free(buffer);
		}
		return 0;
	}
	else
	{
		strcpy(errore , "\e[1;31mErrore nel contenuto del messaggio \033[m \n"); //bizantine error
		return 0;
	}
	return 0;
}



// transazione per l'upload di un file, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, path risorsa locale , elemento shell , eventuale descrizione dell'errore
int opTr_upload(int sockfd , char *remote_ip , Id_Trans *Id_t , char *res_loc , char *res_net , Shell_El *shell , char *errore)
{
	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;
	
	int fd;
	fd = open(res_loc , O_RDONLY) ;
	
	if (fd == -1)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'apertura del file da inviare \033[m \n"); 
		return 0;
	}
	
	struct stat buf;
	
	if(fstat(fd, &buf) == -1)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'analisi del file da inviare \033[m \n"); 
		close(fd);
		return 0;
	}
	
	unsigned int size_file ;
	size_file = (unsigned int) buf.st_size;

	msg =  init_transaction_msg(_TR_UPLOAD , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , size_file);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		close(fd);
		free(msg);
		return 0;
	}
	
	if(write_res_on_res(fd , sockfd ,  max_line) )
	{
		close(fd);
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del file \033[m \n"); //send error
		close(fd);
		return 0;
	}			

	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{	
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
				
		if(rc == 1)
			break;
			
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
					printf("\e[1;32mRisorsa Uplodata correttamente :) \033[m \n\n");
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}

		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}
	
	if(Id_t->msg_count !=  msg->msg_count)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		free(msg);
		printf("\e[1;32mRisorsa Uplodata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	return 0;
}



// transazione per la rimozione di un file, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_remove(int sockfd , char *remote_ip , Id_Trans *Id_t , char *res_net , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;

	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_RM , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
					
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
					printf("\e[1;32mRisorsa Eliminata correttamente :) \033[m \n\n");
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}

	if(Id_t->msg_count !=  msg->msg_count)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		free(msg);
		printf("\e[1;32mRisorsa Eliminata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	return 0;
}



// transazione per la creazione di una directory, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_mkdir(int sockfd , char *remote_ip , Id_Trans *Id_t , char *res_net , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_MKDIR , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
					
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
					printf("\e[1;32mDirectory creata correttamente :) \033[m \n\n");
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}
	
	if(Id_t->msg_count !=  msg->msg_count)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		free(msg);
		printf("\e[1;32mDirectory creata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	return 0;
}



// transazione per la creazione di un file GA, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_newGa(int sockfd , char *remote_ip , Id_Trans *Id_t , char *res_net , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_NEW_GA  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
					
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
					printf("\e[1;32mFile GA creato correttamente :) \033[m \n\n");
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}
		
	if(Id_t->msg_count !=  msg->msg_count)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		free(msg);
		printf("\e[1;32mFile GA creato correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	return 0;
}



// transazione per la creazione di un nuovo capitolo su un file GA, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_newCap(int sockfd , char *remote_ip , Id_Trans *Id_t , char *res_net , Shell_El *shell, int cap, char *errore)
{

	int dim_temp = strlen(tmp_path) + L_TEMP;
	char path_filecap[dim_temp];

	int fc = create_fileTemp(tmp_path , path_filecap , dim_temp);
	
	close(fc);
	
	int *stato = NULL;
	char com[500];
	char risp[10];
									
	memset( com , '\0' , 500 );
	memset( risp , '\0' ,10 );
	 
	printf("Inserisci il nome di un programma di modifica testuale (es: gedit, nano ecc ecc): ");
	fgets( com , 100 , stdin);
	printf("\n");
	com[strlen(com)-1] = '\0';
	int pid = fork();
	if(pid == 0)
	{
	 execlp( com , com , path_filecap , NULL);
	}
	waitpid(pid , stato , 0);
	
	setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
	
	puts("\nCreare il capitolo sul FileSystem PaCu (y/n)? ");
	fgets( risp , 9 , stdin);
	printf("\n");
	setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
	
	if((strncmp(risp ,"y" , 1)== 0) || (strncmp(risp ,"Y" , 1)== 0) || (strncmp(risp ,"yes" , 3)== 0) ||(strncmp(risp ,"Yes" , 3)== 0))
	{ ; }
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		TrMessage *msg;
	
		short int rc;
		++Id_t->msg_count ;

		msg =  init_transaction_msg(_TR_ABORT   , Id_t->id_p , Id_t->id_c , Id_t->msg_count , 0, 0);
	
		rc = send_TrMessage(sockfd , msg);
		
		free(msg);
		return 0;
	}
	

	fc = open(path_filecap , O_RDONLY);

	if( !anal_ga(fc) )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		close(fc);
		return 0;
	}
	
	struct stat buf;
	
	if(fstat(fc, &buf) == -1)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'analisi del file da inviare \033[m \n"); 
		close(fc);
		return 0;
	}
	
	unsigned int size_cap ;
	size_cap = (unsigned int) buf.st_size;
	
	TrMessage *msg;
	
	short int rc;
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_NEW_CAP  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  cap , size_cap);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		close(fc);
		free(msg);
		return 0;
	}

	if(write_control_res(fc , sockfd ,  max_line , size_cap) )
	{
		close(fc);
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del file \033[m \n"); //send error
		close(fc);
		return 0;
	}	
	
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
					
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
					printf("\e[1;32mNuovo capitolo creato correttamente :) \033[m \n\n");
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}
	
	if(Id_t->msg_count !=  msg->msg_count)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		
		free(msg);
		printf("\e[1;32mNuovo capitolo creato correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{	
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
}



// transazione per lo spostamento di un capitolo di un file GA, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket, struttura per i byte di controllo del pacchetto, elemento shell, id capitolo, nuova posizione,
// eventuale descrizione dell'errore
int opTr_mvCap(int sockfd , char *remote_ip , char *res, Id_Trans *Id_t , char *res_net , Shell_El *shell , int id , int cap , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_MV_CAP  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  id , cap);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
					
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
					int fl;
					fl = open(res, O_RDWR);
		
					int dim_temp = strlen(tmp_path) + L_TEMP;
					char path_filetemp[dim_temp];

					int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
		
					move_PosCapGA(fl, fd, id, cap , tmp_path) ;
					cp_rewrite_file(path_filetemp , res , max_line )	;
					
					printf("\e[1;32mCapitolo spostato correttamente (anche sulla risorsa in locale!!!) :) \033[m \n\n");
					close(fl);
					close(fd);
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}

	if(Id_t->msg_count !=  msg->msg_count)
	{
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		int fl;
		fl = open(res, O_RDWR);
		
		int dim_temp = strlen(tmp_path) + L_TEMP;
		char path_filetemp[dim_temp];

		int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
		
		move_PosCapGA(fl, fd, id, cap , tmp_path) ;
		cp_rewrite_file(path_filetemp , res , max_line )	;
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		free(msg);
		printf("\e[1;32mCapitolo spostato correttamente (anche sulla risorsa in locale!!!) :) \033[m \n\n");
		close(fl);
		close(fd);
		return 1;
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	return 0;
}



// transazione per la rimozione di una directory, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_removeDir(int sockfd , char *remote_ip , Id_Trans *Id_t , char *res_net , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_RM_DIR , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
		
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
					printf("\e[1;32mDirectory Eliminata correttamente :) \033[m \n\n");
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}
	
	if(Id_t->msg_count !=  msg->msg_count)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		free(msg);
		printf("\e[1;32mDirectory Eliminata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	return 0;
}


// transazione per la rimozione di un capitolo di un file GA, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket, struttura per i byte di controllo del pacchetto, elemento shell, id capitolo, eventuale descrizione dell'errore
int opTr_removeCap(int sockfd , char *remote_ip , char *res, Id_Trans *Id_t , char *res_net , Shell_El *shell , int id  , char *errore)
{
	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_RM_CAP  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  id , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO );
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;

	while(1)
	{
		memset(msg ,  0 , sizeof(msg));
		rc = recv_TrMessage(sockfd , msg);
		
		if(rc == 0 )
		{
			if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
			{
				int stato;
				int f;
				stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
				if( stato == LOG_COMMIT)
				{
				
					int fl;
					fl = open(res, O_RDWR);

					int dim_temp = strlen(tmp_path) + L_TEMP;
					char path_filetemp[dim_temp];

					int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);

					extract_excluse_IdCapGA(fl, fd, id) ;
					cp_rewrite_file(path_filetemp , res , max_line );
					update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);

					close(fl);
					close(fd);
				
					printf("\e[1;32mCapitolo eliminato correttamente (anche sulla risorsa in locale!!!) :) \033[m \n\n");
					f = 1;
				}
				else
				{
					if( stato == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
						f = 0;
					}
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
						f = 0;
					}				
		
				}			
				free(msg);
				return f;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
			free(msg);
			return 0;
		}
	}

	if(Id_t->msg_count !=  msg->msg_count)
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
		free(msg);
		return 0;
	}
	
	if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{

		int fl;
		fl = open(res, O_RDWR);
		
		int dim_temp = strlen(tmp_path) + L_TEMP;
		char path_filetemp[dim_temp];

		int fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
		
		extract_excluse_IdCapGA(fl, fd, id) ;
		cp_rewrite_file(path_filetemp , res , max_line );
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
		free(msg);
		printf("\e[1;32mCapitolo eliminato correttamente (anche sulla risorsa in locale!!!) :) \033[m \n\n");
		close(fl);
		close(fd);
		return 1;
	}
	else
	{
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	return 0;
	
}



// transazione per la modifica di alcuni capitoli, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket, ip del server , risorsa struttura per i byte di controllo del pacchetto, elemento shell, id capitolo, eventuale descrizione dell'errore
int opTr_ModCap(int sockfd , char *remote_ip , char *res_loc , Id_Trans *Id_t , char *res_net , Shell_El *shell, int loc_max_cap , char* errore )
{
	int i_cap = 0;
	int exit = 0;
	short int rc;
	TrMessage *msg = (TrMessage *) calloc ( 1 , sizeof(TrMessage) );
	int array_pos_cap[loc_max_cap];
	
	int fr = open(res_loc, O_RDWR);
	int n_cap = count_CapGA(fr);

	
	int descrittori[loc_max_cap];
	
	char path_mod_file[loc_max_cap][_DIM_RES];
	
	int i;
	
	for( i = 0 ; i < loc_max_cap ; ++i)
	{
	
		descrittori[i] = -1;
		array_pos_cap[i] = -1;
		memset( path_mod_file[i] , '\0' , _DIM_RES );
	
	}
	
	while( i_cap < loc_max_cap && !exit )
	{
		char risp[10] = {0x0};
		
		printf(" 1) Inserisci la posizione del capitolo da modificare \n" );
		printf(" 2) esci \n:");
		setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
		fgets( risp , 9 , stdin);
		risp[strlen(risp)-1] = '\0';
		printf("\n");

		if(strlen(risp) == 1 && (strncmp(risp , "1" , 1) == 0 || strncmp(risp , "2" , 1) == 0) )
		{
		
			if(strlen(risp) == 1 && (strncmp(risp , "2" , 1) == 0 ))
			{
			 	exit = 1;
			}
			else
			{
				memset(risp , '\0' , 10 );
				printf("Inserisci la posizione del capitolo da modificare :" );
				fgets( risp , 9 , stdin);
				printf("\n");
				
				int pos_cap;
				pos_cap = atoi(risp) ;
				int g;
				short int ctr_cap = 0;
				
				for( g = 0 ; g < loc_max_cap ; ++g)
				{
					if( array_pos_cap[g] == pos_cap || pos_cap < 1 || pos_cap > n_cap )
					{
						ctr_cap = 1;
					}
					
				}
				
				if(ctr_cap )
				{
					printf("\e[1;31mErrore: nell'inserimento della posizione del capitolo %d \033[m \n" , pos_cap);
				}
				else
				{
					++Id_t->msg_count ;
					array_pos_cap[i_cap] = pos_cap;
					
					int id_cap = get_id_pos(fr, pos_cap);

					descrittori[i_cap] = create_fileTemp(tmp_path , path_mod_file[i_cap] , _DIM_RES);
					
					extract_from_IdCapGA(fr, descrittori[i_cap] , id_cap);
					
					//Copio il capitolo originale in un file temporaneo
					int dim_temp = strlen(tmp_path) + L_TEMP;
					char path_filetemp[dim_temp];

					int fdTemp = create_fileTemp(tmp_path , path_filetemp , dim_temp);
					
					write_res_on_res(descrittori[i_cap] , fdTemp ,  max_line);
					
					int *stato = NULL;
					char com[500];
									
					memset( com , '\0' , 500 );
					memset( risp , '\0' ,10 );
					 
					printf("Inserisci il nome di un programma di modifica testuale (es: gedit, nano ecc ecc): ");
					fgets( com , 100 , stdin);
					printf("\n");
					com[strlen(com)-1] = '\0';
					int pid = fork();
					if(pid == 0)
					{
					 execlp( com , com , path_mod_file[i_cap] , NULL);
					}
					waitpid(pid , stato , 0);
	
					setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
	
					puts("\nModificare il capitolo sul FileSystem PaCu (y/n)? ");
					fgets( risp , 9 , stdin);
					printf("\n");
					setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
	
					if((strncmp(risp ,"y" , 1)== 0) || (strncmp(risp ,"Y" , 1)== 0) || (strncmp(risp ,"yes" , 3)== 0) ||(strncmp(risp ,"Yes" , 3)== 0))
					{ ; }
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	
						short int rc;

						set_transaction_msg( msg ,_TR_ABORT   , Id_t->id_p , Id_t->id_c , Id_t->msg_count , 0, 0);
	
						rc = send_TrMessage(sockfd , msg);
		
						close(fr);
	
						for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
						{
	
							close(descrittori[i]);
							remove(path_mod_file[i]);
	
						}
						free(msg);
						return 0;
					}
					
					struct stat buf;
	
					if(!anal_ga(descrittori[i_cap]) || fstat(descrittori[i_cap], &buf) == -1)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						strcpy(errore , "\e[1;31mErrore nell'analisi del file da inviare \033[m \n"); 
						
						set_transaction_msg( msg ,_TR_ABORT   , Id_t->id_p , Id_t->id_c , Id_t->msg_count , 0, 0);
	
						rc = send_TrMessage(sockfd , msg);
						free(msg);
						
						close(fr);
	
						for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
						{
							close(descrittori[i]);
							remove(path_mod_file[i]);
						}
						return 0;
					}
	
					unsigned int size_cap ;
					size_cap = (unsigned int) buf.st_size;
					
					char *modifica;
					modifica = loadPatch(fdTemp, descrittori[i_cap] , tmp_path);
					
					close(descrittori[i_cap]);
					close(fdTemp);
					descrittori[i_cap] = open(path_mod_file[i_cap], O_RDWR|O_TRUNC);
					writen(descrittori[i_cap], modifica, strlen(modifica));
					
					set_transaction_msg( msg , _TR_MOD_CAP  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  id_cap , strlen(modifica));

					rc = send_TrMessage(sockfd , msg);
	
					if( rc <= 0)
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
						
						set_transaction_msg( msg ,_TR_ABORT   , Id_t->id_p , Id_t->id_c , Id_t->msg_count , 0, 0);
	
						rc = send_TrMessage(sockfd , msg);
						close(fr);
	
						for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
						{
							close(descrittori[i]);
							remove(path_mod_file[i]);
						}
						free(msg);
						return 0;
					}
					
					if(write_control_res(descrittori[i_cap] , sockfd ,  max_line , strlen(modifica) + 1) )
					{ ; }
					else
					{
						update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
						strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
						
						set_transaction_msg( msg ,_TR_ABORT   , Id_t->id_p , Id_t->id_c , Id_t->msg_count , 0, 0);
	
						rc = send_TrMessage(sockfd , msg);
						close(fr);
	
						for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
						{
							close(descrittori[i]);
							remove(path_mod_file[i]);
						}
						free(msg);
						return 0;
					}	
					
					if(i_cap == 0)
					{
						update_log_res3(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO , res_loc ,res_net, path_mod_file[i_cap] );
						update_log_opt(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_PRONTO , id_cap , -1  );
					}
					else
					{
						insert_log(PATH_DB_LOG ,Id_t->id_p , Id_t->id_c, remote_ip , shell->port , _TR_MOD_CAP , res_loc , res_net , path_mod_file[i_cap] ,    			id_cap , -1 ,LOG_PRONTO );
					}
					++i_cap;
				}
			}
		}
		else
		{
			printf("\e[1;31mErrore: scelta errata \033[m \n");
		}
	}

	if( i_cap == 0 && exit)
	{						
		update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
		close(fr);
				
		set_transaction_msg( msg ,_TR_ABORT   , Id_t->id_p , Id_t->id_c , Id_t->msg_count , 0, 0);

		rc = send_TrMessage(sockfd , msg);

		for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
		{
	
			close(descrittori[i]);
			remove(path_mod_file[i]);
	
		}
		free(msg);
		return 0;	
	
	}
	
	if( i_cap >= 1)
	{
		++Id_t->msg_count ;
		set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
		rc = send_TrMessage(sockfd , msg);
	
		if( rc <= 0)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
			close(fr);
	
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
	
				close(descrittori[i]);
				remove(path_mod_file[i]);
	
			}
			free(msg);
			return 0;
		}
	
		++Id_t->msg_count ;

		while(1)
		{
			memset(msg ,  0 , sizeof(msg));
			rc = recv_TrMessage(sockfd , msg);
	
			if(rc == 0 )
			{
				if( isAlive_pp(listCl_pp , sem_listCl_pp , timeout_pp , _TENTATIVI_PP , my_ip , remote_ip , (int) *port_client_pp ) <= 0 )
				{
					int stato;
					int f;
					stato = request_state(Id_t->id_p , Id_t->id_c , res_net , 25 , 3);
					if( stato == LOG_COMMIT)
					{
						if(i_cap > 1)
							printf("\e[1;32mCapitoli modificati correttamente, si prega di riscaricare la risorsa :) \033[m \n\n");
						else
							printf("\e[1;32mCapitolo creato correttamente, si prega di riscaricare la risorsa :) \033[m \n\n");
						f = 1;
					}
					else
					{
						if( stato == -1)
						{
							update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
							printf("\e[1;31mErrore: Il server: %s non è attivo, verificare più tardi se l'operazione è stata effettuata \033[m \n" , remote_ip);
							f = 0;
						}
						else
						{
							update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
							printf("\e[1;31mErrore: Operazione non riuscita \033[m \n");
							f = 0;
						}				
		
					}			
	
					for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
					{
	
						close(descrittori[i]);
						remove(path_mod_file[i]);
	
					}
					free(msg);
					return 0;
				}
			}
			
			if(rc == 1)
				break;
		
			if(rc == -1)
			{
				update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
				strcpy(errore , "\e[1;31mErrore nella ricezione del messaggio \033[m \n"); //send error
				close(fr);
	
				for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
				{
	
					close(descrittori[i]);
					remove(path_mod_file[i]);
	
				}
				free(msg);
				return 0;
			}
		}

		if(Id_t->msg_count !=  msg->msg_count)
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore: perdita di alcuni pacchetti \033[m \n");
			close(fr);
	
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
	
				close(descrittori[i]);
				remove(path_mod_file[i]);
	
			}
			free(msg);
			return 0;
		}

		if(  (Id_t->id_p !=  msg->id_primary)  && (Id_t->id_c !=  msg->id_count)  )
		{
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
			close(fr);
	
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
	
				close(descrittori[i]);
				remove(path_mod_file[i]);
	
			}
			free(msg);
			return 0;
		}

		if(msg->pay_desc == _TR_COMMIT)
		{		
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_COMMIT);
			if(i_cap > 1)
				printf("\e[1;32mCapitoli modificati correttamente, si prega di riscaricare la risorsa :) \033[m \n\n");
			else
				printf("\e[1;32mCapitolo creato correttamente, si prega di riscaricare la risorsa :) \033[m \n\n");
			close(fr);
	
			for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
			{
	
				close(descrittori[i]);
				remove(path_mod_file[i]);
	
			}
			free(msg);
			return 1;
		}
		else
		{	
			update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
			if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
			{
				close(fr);
	
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

	update_log(PATH_DB_LOG , Id_t->id_p , Id_t->id_c , LOG_ABORT);
	close(fr);
	
	for( i = 0 ; (i < loc_max_cap ) && (descrittori[i] != -1); ++i)
	{
	
		close(descrittori[i]);
		remove(path_mod_file[i]);
	
	}
	free(msg);
	return 0;
}



// operazione di richiesta dello stato della transazione agli altri server quando il primary crash prima di inviare il commit al client
// Parametri : id della transazione , risorsa per ricavare il dominio , tempo di sleep , numero di prove
// ritorna : -1 se l'operazione fallisce lo stato se l'operazione riesce
int request_state(int id_p , int id_c , char* res_net , int time_sleep , int prove)
{
	short int a = 0;
	short int b = 1;
	int sockfd;
	int index_server;
	
	char temp_res[_DIM_RES];
	memset(temp_res , '\0' , _DIM_RES);
	strcat(temp_res , "/");
	int i = 1;
	
	char ip[_DIM_IPV4] = {0x0}; 
	int port;	
	
	while(b)
	{
		if(strncmp("/" , &(res_net[i]) , 1) == 0)
		{
			b = 0;
		}
		else
		{
			temp_res[i] =  res_net[i] ;
		}
	}
	
	InitMessage* msg;
	msg = init_InitMessage( _INIT_WHOIS_PR , temp_res , strlen(temp_res) , 0 , 0);

	
	while(a < prove)
	{
		b = 1;
		while(b)
		{
			index_server = generatoreRandom(len_list_server);
			if( ( sockfd = connessioneClient(list_server[index_server].ip ,  list_server[index_server].port , 3) ) >= 0 ) 
				b = 0 ;
		}
	
		set_timeout(sockfd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );	
	
		if(send_InitMessage( msg , sockfd ))
		{
			DiagMessage *msgD;
			msgD = (DiagMessage*) calloc (  1 , sizeof(DiagMessage)  );
		
			if(recv_DiagMessage(sockfd , msgD) )
			{
				if(msgD->pay_desc == _DIAG_ACK)
				{
					IDT_Message* buffer;
					buffer = (IDT_Message *) calloc ( 1 , sizeof(IDT_Message));
					int rs;
					rs = recv_IDT_Message( sockfd, buffer);
					if(  rs && buffer->pay_desc == _IDT_PRIMARY)
					{
						strncpy( ip , buffer->ip , _DIM_IPV4);
						port = buffer->port;
				
						char server[_DIM_NAME_SERVER +1];
						memset( server , '\0' , _DIM_NAME_SERVER+1 );
						strncpy(server , buffer->name , _DIM_NAME_SERVER);
				
						char dom[_DIM_DOM ];
						memset( dom , '\0' , _DIM_DOM );
						strncpy(dom , &(temp_res[1]) , _DIM_DOM);
						b = update_dom_primary( dom , server  , ip , port , PATH_DB_TMP_PRI , PATH_DB_TMPDOM);
				
						if(b)
						{
							get_list_server(PATH_DB_TMP_PRI);
							get_list_domini(PATH_DB_TMPDOM);
						}

					}
					free(buffer);
					close(sockfd);
							
					if( ( sockfd = connessioneClient(ip , port , 3) ) >= 0 ) 
					{
						InitMessage* msg_i;
						msg_i = init_InitMessage( _INIT_STATE_TR , NULL , 0 , id_p , id_c);
								
						if(send_InitMessage( msg_i , sockfd ))
						{
							DiagMessage *msgDiag;
							msgDiag = (DiagMessage*) calloc (  1 , sizeof(DiagMessage)  );
	
							if(recv_DiagMessage(sockfd , msgDiag) )
							{
								if(msgDiag->pay_desc == _DIAG_ACK)
								{
									int stato_tr = msgDiag->param_state;
									free(msg_i);
									free(msgDiag);
									free(msgD);
									free(msg);
									close(sockfd);
									return stato_tr;

								}	
								if(msgDiag->pay_desc == _DIAG_ERROR)
								{ ; }
							}
							free(msgDiag);
						}
						free(msg_i);
					}
				}
		
				if(msgD->pay_desc == _DIAG_ERROR)
				{;}
				free(msgD);
			}
			else	
			{
				free(msgD);
			}
		}
		
		sleep(time_sleep);
	}
	free(msg);
	close(sockfd);
	return -1;

}

