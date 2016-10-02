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
					b = opTr_upload(sockfd , remote_ip , &Id_t , res_loc , shell , errore );
				}
				break;

				case _SHELL_RM: 
				{
					b = opTr_remove(sockfd , remote_ip , &Id_t , shell , errore );
				}
				break;
			
				case _SHELL_MKDIR: 
				{
					b = opTr_mkdir(sockfd , remote_ip , &Id_t , shell , errore );
				}
				break;

				case _SHELL_NEWGA: 
				{
					b = opTr_newGa(sockfd , remote_ip , &Id_t , shell , errore );
				}
				break;
			
				case _SHELL_NEWCAP: 
				{
					b = opTr_newCap(sockfd , remote_ip , &Id_t , shell, cap , errore );
				}
				break;
			
				case _SHELL_MVCAP: 
				{
					if( id > 0 )
						b = opTr_mvCap(sockfd , remote_ip , res_loc , &Id_t , shell, id , cap , errore );
					else
						b = 0;
				}
				break;

				case _SHELL_MODCAP: 
				{
					//b = opTr_ModCap(sockfd , remote_ip , res_loc , &Id_t , shell, id , cap , errore );
				}
				break;
			
				case _SHELL_RMCAP: 
				{
					b = opTr_removeCap(sockfd , remote_ip , res_loc , &Id_t , shell, id , errore );
				}
				break;
			
				case _SHELL_RMDIR: 
				{
					b = opTr_removeDir(sockfd , remote_ip , &Id_t , shell , errore );
				}
				break;
				
				default:
				{
					strcpy(errore , "\e[1;31mErrore: operazione scponosciuta \033[m \n");
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
int opTr_upload(int sockfd , char *remote_ip , Id_Trans *Id_t , char *res_loc , Shell_El *shell , char *errore)
{
	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	
	int fd;
	fd = open(res_loc , O_RDONLY) ;
	
	if (fd == -1)
	{
		strcpy(errore , "\e[1;31mErrore nell'apertura del file da inviare \033[m \n"); 
		return 0;
	}
	
	struct stat buf;
	
	if(fstat(fd, &buf) == -1)
	{
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
		strcpy(errore , "\e[1;31mErrore nell'invio del file \033[m \n"); //send error
		close(fd);
		return 0;
	}			

	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}

		if(rc == -1)
		{
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
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		printf("\e[1;32mRisorsa Uplodata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
}



// transazione per la rimozione di un file, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_remove(int sockfd , char *remote_ip , Id_Trans *Id_t , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_RM , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
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
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		printf("\e[1;32mRisorsa Eliminata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
}



// transazione per la creazione di una directory, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_mkdir(int sockfd , char *remote_ip , Id_Trans *Id_t , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_MKDIR , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
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
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		printf("\e[1;32mDirectory creata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
}



// transazione per la creazione di un file GA, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_newGa(int sockfd , char *remote_ip , Id_Trans *Id_t , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_NEW_GA  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
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
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		printf("\e[1;32mFile GA creato correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
}



// transazione per la creazione di un file GA, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_newCap(int sockfd , char *remote_ip , Id_Trans *Id_t , Shell_El *shell, int cap, char *errore)
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
		close(fc);
		return 0;
	}
	
	struct stat buf;
	
	if(fstat(fc, &buf) == -1)
	{
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
		strcpy(errore , "\e[1;31mErrore nell'invio del file \033[m \n"); //send error
		close(fc);
		return 0;
	}	
	
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
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
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		printf("\e[1;32mNuovo capitolo creato correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
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
int opTr_mvCap(int sockfd , char *remote_ip , char *res, Id_Trans *Id_t , Shell_El *shell , int id , int cap , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_MV_CAP  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  id , cap);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
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

		free(msg);
		printf("\e[1;32mCapitolo spostato correttamente (anche sulla risorsa in locale!!!) :) \033[m \n\n");
		close(fl);
		close(fd);
		return 1;
	}
	else
	{
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
}



// transazione per la rimozione di una directory, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket , struttura per i byte di controllo del pacchetto, elemento shell , eventuale descrizione dell'errore
int opTr_removeDir(int sockfd , char *remote_ip , Id_Trans *Id_t , Shell_El *shell , char *errore)
{

	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_RM_DIR , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
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
		strcpy(errore , "\e[1;31mErrore: ID transaction non corrispondente \033[m \n");
		free(msg);
		return 0;
	}
	
	if(msg->pay_desc == _TR_COMMIT)
	{
		free(msg);
		printf("\e[1;32mDirectory Eliminata correttamente :) \033[m \n\n");
		return 1;
	}
	else
	{
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
}


// transazione per la rimozione di un capitolo di un file GA, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e' qualche errore
// Parametri -> socket, struttura per i byte di controllo del pacchetto, elemento shell, id capitolo, eventuale descrizione dell'errore
int opTr_removeCap(int sockfd , char *remote_ip , char *res, Id_Trans *Id_t , Shell_El *shell , int id  , char *errore)
{
	TrMessage *msg;
	
	short int rc;
	
	++Id_t->msg_count ;

	msg =  init_transaction_msg(_TR_RM_CAP  , Id_t->id_p , Id_t->id_c , Id_t->msg_count ,  id , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
		strcpy(errore , "\e[1;31mErrore nell'invio del messaggio \033[m \n"); //send error
		free(msg);
		return 0;
	}
	
	++Id_t->msg_count ;
	set_transaction_msg(msg , _TR_COMMIT ,  Id_t->id_p , Id_t->id_c , Id_t->msg_count  ,  0 , 0);
	
	rc = send_TrMessage(sockfd , msg);
	
	if( rc <= 0)
	{
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
				printf("\e[1;31mErrore: Il server: %s non è attivo \033[m \n" , remote_ip);
				return 0;
			}
		}
		
		if(rc == 1)
			break;
		
		if(rc == -1)
		{
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

		free(msg);
		printf("\e[1;32mCapitolo eliminato correttamente (anche sulla risorsa in locale!!!) :) \033[m \n\n");
		close(fl);
		close(fd);
		return 1;
	}
	else
	{
		if(!ctrl_errorTR(sockfd , msg , shell , errore ) )
		{
			free(msg);
			return 0;
		}
	}
	
	return 0;
	
}






//int dim_temp = strlen(tmp_path) + L_TEMP;
	//char path_filetemp[dim_temp];
	
	 
 /*if( op == _SHELL_MODCAP )
 {
 		int fd ;
 		int sd;

		fd = open(temp_res_loc, O_RDONLY);
		sd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
		
		extract_from_PosCapGA(fd, sd, atoi(list_arg->args[2]));
		
		int *stato = NULL;

		int pid =fork();
		if(pid == 0)
		{
		 execlp( "nano" , "nano" , path_filetemp , NULL);
		}
		waitpid(pid , stato , 0);
		
		close(sd);
		close(fd);
 }*/
