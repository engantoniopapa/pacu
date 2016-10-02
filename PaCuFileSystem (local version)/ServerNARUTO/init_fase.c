#include "header.h"



// ricezione primo messaggio dal client, ritorna: 
//	0 se c'e' un errore, 
//  1 se l'operazione è stata svolta e richiede la chiusura della connessione
//	2 se l'operazione è stata svolta e siamo in attesa di eventuali comandi utente
//  3 se Iniziamo un operazione di transazione
// Parametri -> descrittore del socket di connessione
int init_message(int connsd)
{
	InitMessage *msg_i;

	msg_i = calloc( 1 , sizeof(InitMessage));
	
	set_timeout(connsd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );	

	if( recv_InitMessage( connsd , msg_i)  ==  1)
	{;}
	else
	{
			free(msg_i);
			return 0;
	}

	if(msg_i->pay_desc == _INIT_UPDATE_DOM )
	{	
		free(msg_i);
		int b ;
		
		b = op_init_update(connsd);
		
		return b;
		
	}
	
	if(msg_i->pay_desc == _INIT_START_TR )
	{	
		int b ;
		
		b = op_init_StartTr(connsd, msg_i);
		free(msg_i);
		return 3;
	}
	
	if(msg_i->pay_desc == _INIT_LS )
	{	
		int b ;
		b = op_init_ls(connsd , msg_i);
		
		free(msg_i);
		
		return b;
	}
	
	if(msg_i->pay_desc == _INIT_CD )
	{	
		int b ;
		b = op_init_cd(connsd , msg_i);
		
		free(msg_i);
		
		return b;
	}			
 	
 	if(msg_i->pay_desc == _INIT_SEARCH )
	{	
		int b ;
		b = op_init_search(connsd , msg_i);
		
		free(msg_i);
		
		return b;
		
	}	
 	
	if(msg_i->pay_desc == _INIT_READ_RESOURCE )
	{	
		int b ;
		b = op_init_readR(connsd , msg_i);
		
		free(msg_i);
		
		return b;
	}	
 	
	if(msg_i->pay_desc == _INIT_STATE_TR )
	{	
		// da implementare
		free(msg_i);
		return 1 ;
	}	
	
	perror("errore messaggio non formattato \n");
	free(msg_i);
	DiagMessage * msg;
	msg = init_DiagMessage( _DIAG_ERROR ,_BIZANTINE_ERROR);
	send_DiagMessage(connsd , msg);
	free(msg);
	return 0;
}



//	Invio della lista Domini, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione
int send_dom_list(int connsd , char *list)
{
	int rc;

	set_timeout(connsd  , timeout_domlist , 0  , timeout_domlist , 0 );
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



//invia la lista file, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
int send_list_file(int connsd , char **files, size_t counter)
{
		int rc;
		char *list;
		int i;
		int n_char = 1;
		int b ;
		
		for(i = 0 ; i < counter ; ++i)
		{
			n_char = n_char + strlen(files[i] ) + 20;
			
		}
		
		list = (char *) calloc ( n_char , sizeof(char) );

		for(i = 0 ; i < counter ; ++i)
		{
			if(strncmp(files[i], "." , 1) != 0 )
			{
				if(strstr(files[i],"$")  )
				{
					strncat(list , "\e[1;31m" , 8 ) ;
					strcat(list , files[i] );
					list[strlen(list)-1] = '\0';
					strncat(list , "\033[m" , 6 ) ;
					strncat(list , "\n" , 1);
				}
			
				else
				{
					strcat(list , files[i] );
					strncat(list , "\n" ,1);
				}	
			}	
		}
		
		rc = writeBuffer_on_file(connsd , max_line, list) ;

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
			b = 1;
		}
		
		free(list);
		return b ;
}



//invia la lista in risposta alla search, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
int send_list_filtered(int connsd , char **files, size_t counter)
{
		int rc;
		char *list;
		int i;
		int n_char = 1;
		int b ;
		
		int dim;
		
		dim = strlen( realpath(fs_path, NULL) );
		
		for(i = 0 ; i < counter ; ++i)
		{
			n_char = n_char + strlen(files[i] ) + 20;
			
		}
		
		list = (char *) calloc ( n_char , sizeof(char) );

		for(i = 0 ; i < counter ; ++i)
		{
			if(files[i])
			{
				if( *files[i] == '$'  )
				{				
					strcat(list , &files[i][dim] );
					strncat(list , "\n" , 1);
				}
			
				else
				{
					strncat(list , "\e[1;31m" , 8 ) ;
					strcat(list , &files[i][dim] );
					strncat(list , "\033[m" , 6 ) ;
					strncat(list , "\n" ,1);
				}	
			}
		}
		
		rc = writeBuffer_on_file(connsd , max_line, list) ;

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
			b = 1;
		}
		
		free(list);
		return b ;
}



// operazione sul messaggio di init_update, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parametri -> descrittore della connessione
int op_init_update(int connsd)
{
	DiagMessage *msg;
	msg = init_DiagMessage( _DIAG_ACK , 0);
	
	if( send_DiagMessage(connsd , msg) )
	{
		free(msg);
		
		if( send_dom_list(connsd , string_dom_serv) )
		{
				set_timeout(connsd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );
				return 1;	
		}		
		else
		{
				set_timeout(connsd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );
				perror("errore nell'invio della lista domini \n");
				return 0;	
		}
	}
	
	else
	{
		perror("errore nella send dell'ack \n");
		free(msg);
		return 0;
	}
}



// operazione sul messaggio di init_start_tr, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parametri -> descrittore della connessione, messaggio ricevuto
int op_init_StartTr(int connsd , InitMessage *msg_i)
{
	if(msg_i->l_res < 1)
	{
		return 0;
	}	
	
	int temp_rc ;	
	temp_rc = is_primary( msg_i->res , msg_i->l_res , name_server);
	
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
		temp_rc = is_dominio (msg_i->res , msg_i->l_res  );
		
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
		
		TrMessage* msg;
		msg =  init_transaction_msg(_TR_ERROR , -1 , -1 , -1 ,  _ERR_TR_NODOM , 0);
		if(send_TrMessage(connsd , msg))
		{
			char temp_ip[_DIM_IPV4];
			char temp_name[_DIM_NAME_SERVER+1];
			int temp_port;
			
			memset(temp_ip ,'\0' , _DIM_IPV4);
			
			if( whois_primary( msg_i->res , msg_i->l_res  , temp_name , temp_ip , &temp_port) )
			{
				IDT_Message *msg_idt;
				msg_idt = init_IDT_Message(_IDT_PRIMARY , temp_name , strlen(temp_name) , temp_ip , temp_port);
				send_IDT_Message(connsd , msg_idt );
				free(msg_idt);
			}	
		}
		free(msg);
		return 0;
	}

	int dim  = _DIM_RES * 2;
 	char temp_path[dim];
 	memset(temp_path , '\0' ,  dim);
 	strncat(temp_path, fs_path ,dim);
 	strncat(temp_path, msg_i->res , msg_i->l_res);
 	
 	if(msg_i->opt1 == OPT1_ISTR_DIR || msg_i->opt1 == OPT1_ISTR_FILE || msg_i->opt2 == OPT2_ISTR_IS || msg_i->opt2 ==OPT2_ISTR_NOIS )
 	{
 		if(msg_i->opt2 == OPT2_ISTR_IS)
 		{
 			if(msg_i->opt1 == OPT1_ISTR_DIR)
 			{
 				if(!isDir(temp_path))
 				{
					TrMessage* msg;
					msg =  init_transaction_msg(_TR_ERROR , -1 , -1 , -1 , _ERR_TR_NORES , 0);
					send_TrMessage(connsd , msg);
					free(msg);
					return 0 ;
 				}
 			}
 			
 	 		if(msg_i->opt1 == OPT1_ISTR_FILE)
 			{
 			 if(!isFile(temp_path) )
 			 {
					TrMessage* msg;
					msg =  init_transaction_msg(_TR_ERROR , -1 , -1 , -1 , _ERR_TR_NORES , 0);
					send_TrMessage(connsd , msg);
					free(msg);
					return 0 ;
 				}
 			}
 		}
 		else
 		{
 			char tmp[_DIM_RES * 2] = {0x0};
			get_top_path(tmp, temp_path);

			if(!isDir(tmp))
			{
				TrMessage* msg;
				msg =  init_transaction_msg(_TR_ERROR , -1 , -1 , -1 , _ERR_TR_NORES , 0);
				send_TrMessage(connsd , msg);
				free(msg);
				return 0 ;
			}
 		}
 	}
	else
	{
			TrMessage* msg;
			msg =  init_transaction_msg(_TR_ERROR , -1 , -1 , -1 ,  _ERR_TR_BIZ , 0);
			send_TrMessage(connsd , msg);
			free(msg);
			return 0 ;
	}

	int short b ;
	b = start_transaction(connsd , msg_i);

	return b;	
	
}



// operazione sul messaggio di init_ls, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parametri -> descrittore della connessione, messaggio ricevuto
int op_init_ls(int connsd , InitMessage *msg_i)
{
	if(msg_i->l_res < 1)
	{
		return 0;
	}	
	
	int temp_rc ;	
	temp_rc = is_primary( msg_i->res , msg_i->l_res , name_server);
	
	if(temp_rc == -1)
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}
	
	if(temp_rc == 0)
	{
		temp_rc = is_dominio (msg_i->res , msg_i->l_res  );
		
		if(temp_rc == -1)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
		
		if(temp_rc == 0)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _IGNORE_DOM_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
		
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_DOM_ERROR);
		if(send_DiagMessage(connsd , msg))
		{
			char temp_ip[_DIM_IPV4];
			char temp_name[_DIM_NAME_SERVER+1];
			int temp_port;
			
			memset(temp_ip ,'\0' , _DIM_IPV4);
			
			if( whois_primary( msg_i->res , msg_i->l_res  , temp_name , temp_ip , &temp_port) )
			{
				IDT_Message *msg_idt;
				msg_idt = init_IDT_Message(_IDT_PRIMARY , temp_name , strlen(temp_name) , temp_ip , temp_port);
				send_IDT_Message(connsd , msg_idt );
				free(msg_idt);
			}	
			
		}
		
		free(msg);
		return 0;
		
	}

	int dim  = _DIM_RES * 2;
 	char temp_path[dim];
 	memset(temp_path , '\0' ,  dim);
 	strncat(temp_path, fs_path ,dim);
 	strncat(temp_path, msg_i->res , msg_i->l_res);
 	
	if(isDir(temp_path) )
 	{
		char **files;
	  size_t *counter;
	  int b;
	  
 		counter = (size_t *)calloc(1, sizeof(size_t));
 
	  files = get_all_files(temp_path, counter);
	  if (!files) {
	      fprintf(stderr, "%s: errore\n", temp_path );
	  }
 			 		
		DiagMessage *msg;
		msg = init_DiagMessage( _DIAG_ACK , 0);
	
		if( send_DiagMessage(connsd , msg) )
		{
			free(msg);
			if( send_list_file(connsd ,files, *counter) )
			{
				b = 1;
			}
			else
			{
				b = 0 ;
			}
		}
		else
		{
			perror("errore nella send dell'ack \n");
			free(msg);
			b = 0;
		}
	  
	 	
	  freeContent(files, *counter);
 	  free(counter);

	  return b ;
	}
	else
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_RESOURCE);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}
}



// operazione sul messaggio di init_cd, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parametri -> descrittore della connessione, messaggio ricevuto
int op_init_cd(int connsd , InitMessage *msg_i)
{
	if(msg_i->l_res < 1)
	{
		return 0;
	}

	int temp_rc ;	
	temp_rc = is_primary( msg_i->res , msg_i->l_res , name_server);

	if(temp_rc == -1)
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}

	if(temp_rc == 0)
	{
		temp_rc = is_dominio (msg_i->res , msg_i->l_res  );
	
		if(temp_rc == -1)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
	
		if(temp_rc == 0)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _IGNORE_DOM_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
		
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_DOM_ERROR);
		if(send_DiagMessage(connsd , msg))
		{
			char temp_ip[_DIM_IPV4];
			char temp_name[_DIM_NAME_SERVER+1];
			int temp_port;
		
			memset(temp_ip ,'\0' , _DIM_IPV4);
		
			if( whois_primary( msg_i->res , msg_i->l_res  , temp_name , temp_ip , &temp_port) )
			{
				IDT_Message *msg_idt;
				msg_idt = init_IDT_Message(_IDT_PRIMARY , temp_name , strlen(temp_name) , temp_ip , temp_port);
				send_IDT_Message(connsd , msg_idt );
				free(msg_idt);
			}	
		
		}
	
		free(msg);
		return 0;
	
	}

	int dim  = _DIM_RES * 2;
	char temp_path[dim];
	memset(temp_path , '\0' ,  dim);
	strncat(temp_path, fs_path ,dim);
	strncat(temp_path, msg_i->res , msg_i->l_res);

	if(isDir(temp_path) )
	{
		DiagMessage *msg;
		msg = init_DiagMessage( _DIAG_ACK , 0);

		if( send_DiagMessage(connsd , msg) )
		{
			free(msg);
			return 1;
		}

		else
		{
			perror("errore nella send dell'ack \n");
			free(msg);
			return 0;
		}
	}
	else
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_RESOURCE);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}

	return 1;
}



// operazione sul messaggio di init_search, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parametri -> descrittore della connessione, messaggio ricevuto
int op_init_search(int connsd , InitMessage *msg_i)
{
	if(msg_i->l_res < 1)
	{
		return 0;
	}
		
	int temp_rc ;	
	temp_rc = is_primary( msg_i->res , msg_i->opt1 , name_server);
	
	if(temp_rc == -1)
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}
	
	if(temp_rc == 0)
	{
		temp_rc = is_dominio (msg_i->res , msg_i->opt1  );
		
		if(temp_rc == -1)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
		
		if(temp_rc == 0)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _IGNORE_DOM_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
		
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_DOM_ERROR);
		if(send_DiagMessage(connsd , msg))
		{
			char temp_ip[_DIM_IPV4];
			char temp_name[_DIM_NAME_SERVER+1];
			int temp_port;
			
			memset(temp_ip ,'\0' , _DIM_IPV4);
			
			if( whois_primary( msg_i->res , msg_i->opt1  , temp_name , temp_ip , &temp_port) )
			{
				IDT_Message *msg_idt;
				msg_idt = init_IDT_Message(_IDT_PRIMARY , temp_name , strlen(temp_name) , temp_ip , temp_port);
				send_IDT_Message(connsd , msg_idt );
				free(msg_idt);
			}	
			
		}
		
		free(msg);
		return 0;
		
	}

	int dim  = _DIM_RES * 2;
 	char temp_path[dim];
 	memset(temp_path , '\0' ,  dim);
 	strncat(temp_path, fs_path ,dim);
 	strncat(temp_path, msg_i->res , msg_i->opt1);
 	
 	char param[201];
 	memset(param , '\0' , 201 );
 	strncat(param ,  &msg_i->res[msg_i->opt1] , msg_i->l_res - msg_i->opt1);
 	
	if(isDir(temp_path) )
 	{
		char **files;
	  int *counter;
	  int b;
	  
	  char *absol_path;
	  
 		counter = (int *)calloc(1, sizeof(int));
 		
 		absol_path = realpath(temp_path, NULL);
 		
	  files = search(absol_path, param, counter);
	  if (!files) {
	      fprintf(stderr, "%s: errore\n", absol_path );
	  }
 			 		
		DiagMessage *msg;
		msg = init_DiagMessage( _DIAG_ACK , 0);
	
		if( send_DiagMessage(connsd , msg) )
		{
			free(msg);
			if( send_list_filtered(connsd ,files, *counter) )
			{
				b = 1;
			}
			else
			{
				b = 0 ;
			}
		}
		else
		{
			perror("errore nella send dell'ack \n");
			free(msg);
			b = 0;
		}
	  
	 	
	  freeList_search(files, *counter);
 	  free(counter);

	  return b ;
	}
	else
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_RESOURCE);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}
}



// operazione sul messaggio di init_read_reasource, ritorna 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parametri -> descrittore della connessione, messaggio ricevuto
int op_init_readR(int connsd , InitMessage *msg_i)
{
	if(msg_i->l_res < 1)
	{
		return 0;
	}

	int temp_rc ;	
	temp_rc = is_primary( msg_i->res , msg_i->l_res , name_server);

	if(temp_rc == -1)
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}

	if(temp_rc == 0)
	{
		temp_rc = is_dominio (msg_i->res , msg_i->l_res  );
	
		if(temp_rc == -1)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _GENERIC_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
	
		if(temp_rc == 0)
		{
			DiagMessage * msg;
			msg = init_DiagMessage( _DIAG_ERROR , _IGNORE_DOM_ERROR);
			send_DiagMessage(connsd , msg);
			free(msg);
			return 0;
		}
		
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_DOM_ERROR);
		if(send_DiagMessage(connsd , msg))
		{
			char temp_ip[_DIM_IPV4];
			char temp_name[_DIM_NAME_SERVER+1];
			int temp_port;
		
			memset(temp_ip ,'\0' , _DIM_IPV4);
		
			if( whois_primary( msg_i->res , msg_i->l_res  , temp_name , temp_ip , &temp_port) )
			{
				IDT_Message *msg_idt;
				msg_idt = init_IDT_Message(_IDT_PRIMARY , temp_name , strlen(temp_name) , temp_ip , temp_port);
				send_IDT_Message(connsd , msg_idt );
				free(msg_idt);
			}	
		
		}
	
		free(msg);
		return 0;
	
	}

	int dim  = _DIM_RES * 2;
	char temp_path[dim];
	memset(temp_path , '\0' ,  dim);
	strncat(temp_path, fs_path ,dim);
	strncat(temp_path, msg_i->res , msg_i->l_res);

	if(isFile(temp_path) )
	{
		DiagMessage *msg;
		msg = init_DiagMessage( _DIAG_ACK , 0);

		if( send_DiagMessage(connsd , msg) )
		{
			free(msg);
			
			int fd;
			short int b ;
			fd = open(temp_path , O_RDONLY) ;
			lockRead(fd);
			
			if (fd == -1)
			{
				return 0;
			}
			
			else
			{
				if(write_res_on_res(fd , connsd ,  max_line) )
					b = 1;
				else
					b = 0;
			}
			
			unlockFile(fd);
			close(fd);
			return b;
		}

		else
		{
			perror("errore nella send dell'ack \n");
			free(msg);
			return 0;
		}
	}
	else
	{
		DiagMessage * msg;
		msg = init_DiagMessage( _DIAG_ERROR , _NO_RESOURCE);
		send_DiagMessage(connsd , msg);
		free(msg);
		return 0;
	}

	return 1;
}
