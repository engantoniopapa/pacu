#include "header.h"



// fa partire la console, ritorna: 0 se c'e' un errore , 1 per l'usicta
int start_console()
{
	int rc;
	system("clear");

	Shell_El *shell;
	shell = create_Shell_El();
	reset_Shell_El(shell);
	
	TokenArgs *list_arg;
	list_arg = create_TokenArgs();
	reset_TokenArgs(list_arg);
	
	rc = operation_user(shell , list_arg);

	if( rc == 0)
	{
		printf( "Errore durante le operazioni console \n");
		free(shell);
		return 0;
	}

	free(shell);
	return 1;
}



// stampa i comandi user
void stampa_comandi_user()
{
	printf("\n-------- COMANDI CONSOLE --------\n");
	printf(" %sexit \033[m termina applicazione \n" , color); //1
	printf(" %sld \033[m elenco domini \n" , color); // 2
	printf(" %smount \033[m accedi ad un dominio (es: mount <nome dominio> ) \n" , color); //3 
	printf(" %sls \033[m mostra il contenuto della directory corrente \n" , color); //4
	printf(" %ssearch \033[m ricerca un file o una direcctory \n" , color); //5
	printf(" %scd  \033[m cambia la directory corrente  \n" , color); //6
	printf(" %sdwn  \033[m scarica un file  \n" , color); //7
	printf(" %sopen  \033[m leggi un file  da remoto\n" , color); //8
	printf(" %sreadga  \033[m leggi un file formato GA (es: readga <local pathfile> )\n" , color); //9
	printf(" %supload  \033[m carica un file (es: upload <local pathfile> <remote pathdir>)\n" , color); 
	printf(" %srm  \033[m cancella un file (es: rm <remote pathfile> )\n" , color); 
	printf(" %smkdir  \033[m crea una cartella (es: mkdir <nome directory> <remote path> )\n" , color); 
	printf(" %snewga  \033[m crea una file formato GA (es: newga <nome file GA> <remote pathdir> )\n" , color); 
	printf(" %snewcap  \033[m crea un capitolo nel file GA (es: newcap <remote pathfile> <n cap>)\n" , color); 
	printf(" %smvcap  \033[m sposta un capitolo del file GA (es: mvcap <local pathfile> <remote pathfile> <pos cap> <new pos cap> )\n" , color); 
	printf(" %smodcap  \033[m modifica un capitolo (es: modcap <local pathfile> <remote pathfile>  <n cap> (max 10 capitoli nella transazione)\n" , color); 
	printf(" %srmcap  \033[m elimina un capitolo (es: rmcap <local pathfile> <remote pathfile> <pos cap> )\n" , color); 
	printf(" %srmdir  \033[m elimina una directory solo se vuota (es: rmdir <remote pathdir> )\n" , color); 
	printf(" %shelp \033[m legenda dei comandi \n" , color); //20
	printf("\n");
}



// alloca dinamicamente una struttura Shell_El, ritorna la struttura allocata
Shell_El* create_Shell_El()
{
	Shell_El *l;
	l = (Shell_El *)calloc( 1 , sizeof(Shell_El) );
	return l;
}



// resetta la struttura Shell_El
// Parametri -> struttura Shell_El da resettare
void reset_Shell_El(Shell_El *l)
{
	
	memset(l->ip, '\0' , _DIM_IPV4);
	l->port = 0;
	
	memset(l->path_user , '\0' , _DIM_PATH_USER );
	
	memset(l->path_res , '\0' , _DIM_SHELL_RES);
	strcpy(l->path_res, "/" );
	
	memset(l->path_dom , '\0' , _DIM_SHELL_DOM);

}



// alloca dinamicamente una struttura TokenArgs, ritorna la struttura allocata
TokenArgs* create_TokenArgs()
{
	TokenArgs *l;
	l = (TokenArgs *)calloc( 1 , sizeof(TokenArgs) );
	return l;
}



// resetta la struttura TokenArgs
// Parametri -> struttura TokenArgs da resettare
void reset_TokenArgs(TokenArgs *l)
{
	l->l_arg = 0;
	
	int i;
	for (i = 0; i<_MAX_ARG; i++)
	{
		memset(l->args[i], '\0', _DIM_SHELL_RES);
	}

}



int parsingString(char *stringa , TokenArgs *t)
{
	reset_TokenArgs(t);
	char field [1024];
	int n;
	int i = 0;

	while (*stringa != '\0')
	{
		if(*stringa == ' ')
		{
			++stringa;
		}
		else
		{
			if(sscanf(stringa, "%[^ ]%n", field, &n) != 1)
			{
				puts("Errore scansione input utente");
				return 0;
			}
			t->l_arg++;
			strcpy(t->args[i], field );
			stringa += n; /* advance the pointer by the number of characters read */
			if ( *stringa != ' ' )
			{
			break; /* didn't find an expected delimiter, done? */
			}
			++stringa; /* skip the delimiter */
			++i;
		}
	}
	return 1;
}


// operazioni user, ritorna: 0 se c'e' un errore , 1 per l'uscita 
// Parametri -> struttura Shell_El , struttura TokenArgs
int operation_user(Shell_El *shell , TokenArgs *list_arg)
{
	int b = 1;
	char comando[_DIM_SHELL_COM];
	int scelta; // identificativo comando console
	setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
	
	while(b)
	{
		set_path_userConsole(shell->path_user  , shell->path_res);
		
		memset(comando, '\0' , _DIM_SHELL_COM  );

		printf("%s " , shell->path_user );
			
		scanf("%[^\n]", comando );

		scelta = parsing_user(comando , list_arg);
		
		switch (scelta) 
		{
			case 0: 
			{
				printf("Comando errato !!!\n");
			}
			break;
		
			case 1: 
			{		
				system("clear");
				return 1;
			}
			
			case 2: 
			{
				stampa_elencoDom();
			}
			break;
			
			case 3: 
			{
				if(op_mount(list_arg->args[0] , shell ))
					set_path_userConsole(shell->path_user  , shell->path_res);
				else
					printf("errore nel comando mount \n");
				
			}
			break;
			
			case 4: 
			{
				if(op_ls(list_arg->args[0] , shell ))
					{;}
				else
					printf("errore nel comando ls \n");
			}
			break;
			
			case 5: 
			{
				if(op_search(list_arg->args[0] , list_arg->args[1] , shell ))
					{;}
				else
					printf("errore nel comando search \n");
			}
			break;
			
			case 6: 
			{
				if( strlen(list_arg->args[0]) == 0)
				{
					printf("errore nell'usare il comando cd \n");
				}
				else
				{
					if(op_cd(list_arg->args[0] , shell ))
					{;}
					else
						printf("errore nel comando cd \n");
				}

			}
			break;
			
			case 7: 
			{
				if( strlen(list_arg->args[0]) == 0)
				{
					printf("errore nell'usare il comando dwn \n");
				}
				else
				{
					if( op_read_save(list_arg->args[0] , shell , 1 ) )
						{;}
					else
						printf("errore nel comando dwn \n");
				}
			}
			break;
			
			case 8: 
			{
				if( strlen(list_arg->args[0]) == 0)
				{
					printf("errore nell'usare il comando open \n");
				}
				else
				{
					if( op_read_save(list_arg->args[0] , shell , 0) )
						{;}
					else
						printf("errore nel comando open \n");
				}


			}
			break;
			
			case 9: 
			{
				if( strlen(list_arg->args[0]) == 0)
				{
					printf("errore nell'usare il comando readga \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_readga(list_arg->args[0] , errore) )
						{;}
					else
						printf("%s \n", errore);
				}

			}
			break;
			
			case _SHELL_UPLOAD: 
			{
				if( strlen(list_arg->args[0]) == 0 )
				{
					printf("errore nell'usare il comando upload \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction( _SHELL_UPLOAD , list_arg , shell , errore  ) )
						{printf("okkkk \n");}
					else
						printf("%s \n", errore);
				}
			}
			break;

			case _SHELL_RM: 
			{
				if( strlen(list_arg->args[0]) == 0)
				{
					printf("errore nell'usare il comando rm \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction( _SHELL_RM , list_arg , shell , errore) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;
			
			case _SHELL_MKDIR: 
			{
				if( strlen(list_arg->args[0]) == 0)
				{
					printf("errore nell'usare il comando mkdir \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction(  _SHELL_MKDIR , list_arg , shell , errore ) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;

			case _SHELL_NEWGA: 
			{
				if( strlen(list_arg->args[0]) == 0)
				{
					printf("errore nell'usare il comando newga \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction(_SHELL_NEWGA , list_arg , shell , errore ) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;
			
			case _SHELL_NEWCAP: 
			{
				if( strlen(list_arg->args[0]) == 0 ) // se non specifico il terzo argomento viene aggiunto in coda
				{
					printf("errore nell'usare il comando newcap \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction(_SHELL_NEWCAP , list_arg , shell , errore) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;
			
			case _SHELL_MVCAP: 
			{
				if( strlen(list_arg->args[0]) == 0 || strlen(list_arg->args[1]) == 0 || strlen(list_arg->args[2]) == 0 || strlen(list_arg->args[3]) == 0 )
				{
					printf("errore nell'usare il comando mvcap \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction(_SHELL_MVCAP , list_arg , shell , errore) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;

			case _SHELL_MODCAP: 
			{
				if( strlen(list_arg->args[0]) == 0 || strlen(list_arg->args[1]) == 0 || strlen(list_arg->args[2]) == 0)
				{
					printf("errore nell'usare il comando modcap \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction(_SHELL_MODCAP , list_arg , shell , errore) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;
			
			case _SHELL_RMCAP: 
			{
				if( strlen(list_arg->args[0]) == 0 || strlen(list_arg->args[1]) == 0 || strlen(list_arg->args[1]) == 0)
				{
					printf("errore nell'usare il comando modcap \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction(_SHELL_RMCAP , list_arg , shell , errore ) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;
			
			case _SHELL_RMDIR: 
			{
				if( strlen(list_arg->args[0]) == 0 )
				{
					printf("errore nell'usare il comando rmdir \n");
				}
				else
				{
					char errore[_MAX_STRING_ERROR];
					memset(errore , '\0' ,_MAX_STRING_ERROR);
					
					if( op_transaction(_SHELL_RMDIR , list_arg , shell , errore) )
						{;}
					else
						printf("%s \n", errore);
				}
			}
			break;

			case 20: 
			{
				stampa_comandi_user();
			}
			break;
			
			default:
			printf("Comando errato !!!\n");
			break; 
		}
			
	}
	return 1; // prova
}



// setta il path console
// Parametri ->path console da settare , percorso iniziale risorsa
void set_path_userConsole(char *path_user  , char *path)
{
 
 memset(path_user , '\0' , (_DIM_PATH_USER) * sizeof(char) );
 
 strcat(path_user , "User:" );
 strncat(path_user , path , (_DIM_PATH_USER - 6 ) );
 strcat(path_user , ">" );
}

//  parsing dei comnadi dell'utente, ritorna: 0 se c'e' un errore, n !=0 che identifica il comando
//  Parametri -> comando, argomenti comando
int parsing_user(char *comando , TokenArgs* t )
{
	short int b = 0 ;

	if(strncmp("exit" , comando , 4) == 0)
	{
		b = 1;
	}

	if(strncmp("ld" , comando , 2) == 0)
	{
		b = 2;
	}
		

	if(strncmp("mount " , comando , 6) == 0)
	{
		if(parsingString(&comando[5] , t) == 0 )
			b = 0;
		else
			b = 3;
	}

	if( ( (strlen(comando) == 2) && (strncmp("ls" , comando , 2) == 0 ) ) ||  strncmp("ls " , comando , 3) == 0  )
	{ 
		if(parsingString(&comando[2] , t) == 0 )
			b = 0;
		else
			b = 4;
	}
	
	if(strncmp("search " , comando , 7) == 0)
	{
		if(parsingString(&comando[6] , t) == 0 )
			b = 0;
		else
			b = 5;
	}
	
	if(strncmp("cd " , comando , 3) == 0)
	{
		if( parsingString(&comando[2] , t) == 0 )
			b = 0;
		else
			b = 6;
	}
	
	if(strncmp("dwn " , comando , 4) == 0)
	{
		if( parsingString(&comando[3] , t) == 0 )
			b = 0;
		else
			b = 7;
	}
	
	if(strncmp("open " , comando , 5) == 0)
	{
		if( parsingString(&comando[4] , t) == 0 )
			b = 0;
		else
			b = 8;
	}
	
	if(strncmp("readga " , comando , 7) == 0)
	{
		if( parsingString(&comando[6] , t) == 0 )
			b = 0;
		else
			b = 9;
	}

	if(strncmp("upload " , comando , 7) == 0)
	{
		if( parsingString(&comando[6] , t) == 0 )
			b = 0;
		else
			b = _SHELL_UPLOAD;
	}

	if(strncmp("rm " , comando , 3) == 0)
	{
		if( parsingString(&comando[2] , t) == 0 )
			b = 0;
		else
			b = _SHELL_RM;
	}

	if(strncmp("mkdir " , comando , 6) == 0)
	{
		if( parsingString(&comando[5] , t) == 0 )
			b = 0;
		else
			b = _SHELL_MKDIR;
	}

	if(strncmp("newga " , comando , 6) == 0)
	{
		if( parsingString(&comando[5] , t) == 0 )
			b = 0;
		else
			b = _SHELL_NEWGA;
	}

	if(strncmp("newcap " , comando , 7) == 0)
	{
		if( parsingString(&comando[6] , t) == 0 )
			b = 0;
		else
			b = _SHELL_NEWCAP;
	}
	
	if(strncmp("mvcap " , comando , 6) == 0)
	{
		if( parsingString(&comando[5] , t) == 0 )
			b = 0;
		else
			b = _SHELL_MVCAP;
	}

	if(strncmp("modcap " , comando , 7) == 0)
	{
		if( parsingString(&comando[6] , t) == 0 )
			b = 0;
		else
			b = _SHELL_MODCAP;
	}

	if(strncmp("rmcap " , comando , 6) == 0)
	{
		if( parsingString(&comando[5] , t) == 0 )
			b = 0;
		else
			b = _SHELL_RMCAP;
	}
	
	if(strncmp("rmdir " , comando , 6) == 0)
	{
		if( parsingString(&comando[5] , t) == 0 )
			b = 0;
		else
			b = _SHELL_RMDIR;
	}
	
	if(strncmp("help" , comando , 4) == 0 )
	{
		b = 20;
	}

	setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
	return b;
}



//stampa la lista domini
void stampa_elencoDom()
{
	printf("\n\nElenco DOMINI:\n\n");
	printf(" %s%s \033[m \n", color , "/" );
	
	int i ;
	
	for( i = 0 ; i < len_list_domini; i++)
	{
		printf(" |\n");
		printf(" |-->%s %s \033[m  \n", color ,list_domini[i].name ); 
	}
	printf("\n\n");
}


// operazione di mount del dominio, ritorna: 0 se c'e' un errore , 1 se l'operazione è riuscita
// Parametri -> nome dominio  , elemento shell
int op_mount(char *arg , Shell_El *shell)
{

	int rc;
	char *stringa_errore;
	DB *db_dom; // descrittore del DB Domini
	DB *db_serv; // descrittore del DB Domini
	short int b ;

	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	db_serv = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	
	rc = open_db_ro( db_dom , PATH_DB_TMPDOM , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  rc = open_db_ro( db_serv , PATH_DB_PRIMARY , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  char string_cond[_DIM_COND];
	memset( string_cond , '\0' , _DIM_COND*sizeof(char) );
	
	snprintf(string_cond , _DIM_COND*sizeof(char) , " where dominio = '%s'" , arg);
	
  rc = count_tb(db_dom  , "Dom"  , string_cond , stringa_errore );

	if( rc == 1 )
	{
		char name_server[_DIM_NAME_SERVER+1];
		memset( name_server , '\0' , (_DIM_NAME_SERVER+1) *sizeof(char) );
		
		int port = 0;
		
		char ip[_DIM_IPV4] ;
		memset(ip , '\0' ,_DIM_IPV4 * sizeof(char) );
		
		select_nameServer_dominio(db_dom , arg,  name_server );
		
		memset( string_cond , '\0' , _DIM_COND*sizeof(char) );
		snprintf(string_cond , _DIM_COND*sizeof(char) , " where name = '%s'" , name_server);

		rc = select_IpPort_server(db_serv , "ListServer" , ip ,  &port , string_cond);
		if( rc == 1 )
		{
			memset( shell->path_res , '\0' , _DIM_SHELL_RES);
			snprintf(shell->path_res , _DIM_SHELL_RES*sizeof(char) , "/%s" , arg);
			
			memset( shell->path_dom , '\0' , _DIM_SHELL_DOM );
			snprintf(shell->path_dom , _DIM_SHELL_RES*sizeof(char) , "/%s" , arg);
			
			strncpy( shell->ip , ip , _DIM_IPV4);
			shell->port = port;
			
			b = 1;
		}
		else
		{
			printf("Primary del dominio non trovato !!! \n");
			b = 0; 
		}		
		
	}
	
	else
	{	
	  if( rc == 0 )
		{
			printf("Il dominio selezionato non è presente nel sistema !!! \n");
			b = 0; 
  	}
		else
		{
			if( rc == -1 )	
		  	fprintf(stderr, "%s\n", stringa_errore);
    	
    	exit(1);
		}
		
	}
	
	rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  rc = close_db(db_serv, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(stringa_errore);
	free(db_dom);
	free(db_serv);
	
	return b ;
}



// operazione di cd, ritorna: 0 se c'e' un errore , 1 se l'operazione è riuscita
// Parametri -> risorsa  , elemento shell
int op_cd(char *arg , Shell_El *shell)
{
	short int b;
	  
	if(strncmp(".." , arg , 2) == 0 && strlen(arg) == 2 )
	{
		int count = 0;
		int i = 0;
		
		for(i = 0; i < _DIM_SHELL_RES ; ++i)
		{
			if( shell->path_res[i] == '/')
				++count;
			
			if( shell->path_res[i] == '\0')
				i = _DIM_SHELL_RES ;
		}
		
		if( count <= 1)
		{		
			b = 1;
		}
		
		else
		{
			for(i = _DIM_SHELL_RES -1; i >= 0 ; --i)
			{
				if( shell->path_res[i] == '\0')
				{;}
				else
				{
					if( shell->path_res[i] == '/')
					{
						shell->path_res[i] = '\0';
						i = -1 ;
					}
					else
					{
						shell->path_res[i] = '\0' ;
					}
				}
			}
			b = 1;
		}
	}
	else
	{
		char temp_res[_DIM_RES];
		memset(temp_res , '\0' , _DIM_RES);

		if(strncmp("/" , arg , 1) == 0)
		{
			strcpy(temp_res , shell->path_dom);
			strcat(temp_res , arg);
		}
		else
		{
			strcpy(temp_res , shell->path_res);
			strcat(temp_res , "/");
			strcat(temp_res , arg);
		}
		
		InitMessage* msg;
		msg = init_InitMessage( _INIT_CD , temp_res , strlen(temp_res) , 0 , 0);
		
		int count_prove = 0;
		int sockfd;
		short int true_con = 0;
		
		while(count_prove < 3 && true_con == 0)
		{
			if( ( sockfd = connessioneClient("127.0.0.1" /*shell->ip*/ ,  shell->port , 3) ) <= 0 ) 
			{
				count_prove++;
				sleep(2 * count_prove);
			}
			else
			{
				true_con  = 1;
			}
		}
		
		if(true_con == 0 )
		{
			short int b;
			b = 1;
			int index_server;
			while(b)
			{
				index_server = generatoreRandom(len_list_server);
				if( ( sockfd = connessioneClient(list_server[index_server].ip ,  list_server[index_server].port , 3) ) >= 0 ) 
					b = 0 ;
			}
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
					if(strncmp("/" , arg , 1) != 0)
					{
						strncat(shell->path_res , "/" , 1);
					}
					strcat(shell->path_res , arg );
					
					b =1;
				}
			
				if(msgD->pay_desc == _DIAG_ERROR)
				{
					
					if(msgD->param_state == _NO_DOM_ERROR)
					{
						IDT_Message* buffer;
						buffer = (IDT_Message *) calloc ( 1 , sizeof(IDT_Message));
						int rs;
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
								printf("Aggiornamento dominio eseguito con successo riprovare il comando \n");
							}

						}
						free(buffer);
					}
					
				}
				free(msgD);
			}
			else	
			{
				free(msgD);
				b = 0;
			}
		}
		else
		{
			b = 0;
		}	
		
		free(msg);
		close(sockfd);
	}
	
	return b;
}



// operazione di ls, ritorna: 0 se c'e' un errore , 1 se l'operazione è riuscita
// Parametri -> risorsa  , elemento shell
int op_ls(char *arg , Shell_El *shell)
{
	short int b;
	  
	char temp_res[_DIM_RES];
	memset(temp_res , '\0' , _DIM_RES);
	if(strncmp("/" , arg , 1) == 0)
	{
		strcpy(temp_res , shell->path_dom);
		strcat(temp_res , arg);
	}
	else
	{
		strcpy(temp_res , shell->path_res);
		strcat(temp_res , "/");
		strcat(temp_res , arg);
	}
	
	InitMessage* msg;
	msg = init_InitMessage( _INIT_LS , temp_res , strlen(temp_res) , 0 , 0);
	
	int count_prove = 0;
	int sockfd;
	short int true_con = 0;
	
	while(count_prove < 3 && true_con == 0)
	{
		if( ( sockfd = connessioneClient("127.0.0.1" /*shell->ip*/ ,  shell->port , 3) ) <= 0 ) 
		{
			count_prove++;
			sleep(2 * count_prove);
		}
		else
		{
			true_con  = 1;
		}
	}
	
	if(true_con == 0 )
	{
		short int b;
		b = 1;
		int index_server;
		
		while(b)
		{
			index_server = generatoreRandom(len_list_server);
			if( ( sockfd = connessioneClient(list_server[index_server].ip ,  list_server[index_server].port , 3) ) >= 0 ) 
				b = 0 ;
		}
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

				char *buffer_file;
				buffer_file = writeFile_on_buffer(sockfd , max_line);
				printf("\n%s\n", buffer_file );
				b =1;
			}
		
			if(msgD->pay_desc == _DIAG_ERROR)
			{			
				if(msgD->param_state == _NO_DOM_ERROR)
				{
					IDT_Message* buffer;
					buffer = (IDT_Message *) calloc ( 1 , sizeof(IDT_Message));
					int rs;
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
							printf("Aggiornamento dominio eseguito con successo riprovare il comando \n");
						}
							
					}
					free(buffer);
				}
				
			}
			free(msgD);
		}
		else	
		{
			free(msgD);
			b = 0;
		}
	}
	else
	{
		b = 0;
	}	
	
	free(msg);
	close(sockfd);

	return b;
}



// operazione di search, ritorna: 0 se c'e' un errore , 1 se l'operazione è riuscita
// Parametri -> argomento1 ,argomento2   , elemento shell
int op_search(char *arg1 , char *arg2 , Shell_El *shell)
{
	short int b;
	int opt1 = 0;
	  
	char temp_res[_DIM_RES];
	memset(temp_res , '\0' , _DIM_RES);
	if( strlen(arg2) > 0)
	{
		if(strncmp("/" , arg1 , 1) == 0)
		{
			strcpy(temp_res , shell->path_dom);
			
			if( strlen(arg1) > 1)
				strcat(temp_res , arg1);
		}
		else
		{
			strcpy(temp_res , shell->path_res);
			strcat(temp_res , "/");
			strcat(temp_res , arg1);
		}
		
		opt1 = strlen(temp_res);
		strcat(temp_res , arg2);
	}
	else
	{
			strcpy(temp_res , shell->path_dom);
			opt1 = strlen(temp_res);
			strcat(temp_res , arg1);
	}

	InitMessage* msg;
	msg = init_InitMessage( _INIT_SEARCH , temp_res , strlen(temp_res) , opt1 , 0);
	
	int count_prove = 0;
	int sockfd;
	short int true_con = 0;
	
	while(count_prove < 3 && true_con == 0)
	{
		if( ( sockfd = connessioneClient("127.0.0.1" /*shell->ip*/ ,  shell->port , 3) ) <= 0 ) 
		{
			count_prove++;
			sleep(2 * count_prove);
		}
		else
		{
			true_con  = 1;
		}
	}
	
	if(true_con == 0 )
	{
		short int b;
		b = 1;
		int index_server;
		
		while(b)
		{
			index_server = generatoreRandom(len_list_server);
			if( ( sockfd = connessioneClient(list_server[index_server].ip ,  list_server[index_server].port , 3) ) >= 0 ) 
				b = 0 ;
		}
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
				char *buffer_file;
				buffer_file = writeFile_on_buffer(sockfd , max_line);
				printf("\n%s \n", buffer_file );
				b =1;
			}
		
			if(msgD->pay_desc == _DIAG_ERROR)
			{
				
				if(msgD->param_state == _NO_DOM_ERROR)
				{
					IDT_Message* buffer;
					buffer = (IDT_Message *) calloc ( 1 , sizeof(IDT_Message));
					int rs;
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
							printf("Aggiornamento dominio eseguito con successo riprovare il comando \n");
						}
						
					}
					free(buffer);
				}
				
			}
			free(msgD);
		}
		else	
		{
			free(msgD);
			b = 0;
		}
	}
	else
	{
		b = 0;
	}	
	
	free(msg);
	close(sockfd);

	return b;
}



// operazione di lettura della risorsa, ritorna: 0 se c'e' un errore , 1 se l'operazione è riuscita 
// Parametri -> risorsa  , elemento shell , permette di salvare il file temporaneo
int op_read_save(char *arg , Shell_El *shell , int param)
{
	short int b;

	char temp_res[_DIM_RES];
	memset(temp_res , '\0' , _DIM_RES);

	if(strncmp("/" , arg , 1) == 0)
	{
		strcpy(temp_res , shell->path_dom);
		strcat(temp_res , arg);
	}
	else
	{
		strcpy(temp_res , shell->path_res);
		strcat(temp_res , "/");
		strcat(temp_res , arg);
	}
	
	InitMessage* msg;
	msg = init_InitMessage( _INIT_READ_RESOURCE , temp_res , strlen(temp_res) , 0 , 0);
	
	int count_prove = 0;
	int sockfd;
	short int true_con = 0;
	
	while(count_prove < 3 && true_con == 0)
	{
		if( ( sockfd = connessioneClient("127.0.0.1" /*shell->ip*/ ,  shell->port , 3) ) <= 0 ) 
		{
			count_prove++;
			sleep(2 * count_prove);
		}
		else
		{
			true_con  = 1;
		}
	}
	
	if(true_con == 0 )
	{
		short int b;
		b = 1;
		int index_server;
		while(b)
		{
			index_server = generatoreRandom(len_list_server);
			if( ( sockfd = connessioneClient(list_server[index_server].ip ,  list_server[index_server].port , 3) ) >= 0 ) 
				b = 0 ;
		}
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
				int fd ;
				int dim_temp = strlen(tmp_path) + L_TEMP;
				char path_filetemp[dim_temp];
				fd = create_fileTemp(tmp_path , path_filetemp , dim_temp);
				
				if (fd == -1)
				{
					b = 0;
				}
			
				else
				{				 
					if(write_res_on_res(sockfd  , fd,  max_line) )
					{					
						if(param)
						{
							int i ;
							int count1 = 0 ;
							int count2 = 0;
							char name_file[strlen(arg) +1];
							
							memset(name_file , '\0' , strlen(arg) +1 );
						
							for(i = 0; i < strlen(arg) ; ++i)
							{
								if(arg[i] == '/')
									++count1; 
							}
							
							for(i = 0; i < strlen(arg) ; ++i)
							{
								if(arg[i] == '/')
									++count2; 
								
								if( count2 == count1)
									strncat(name_file , &arg[i] , 1);
							}
							
							char pathname[strlen(name_file) + strlen(in_path) +2];
							
							memset(pathname , '\0' , strlen(name_file) + strlen(in_path) +2 );
							strcat(pathname , in_path );
							strncat(pathname , "/" , 1);
							strcat(pathname , name_file );
							
							int sd;
							sd = open(pathname, O_WRONLY|O_CREAT|O_TRUNC, 0777);
							
							if(sd == -1)
							{
								printf("errore nella creazione del file: %s \n" , name_file );
								b = 0 ;
							}
							else
							{
								lseek(fd, 0L, SEEK_SET); 
								if(write_res_on_res(fd  , sd,  max_line) )
								{
										close(fd);
										close(sd);

										b = 1;
								}
								else
								{
									printf("errore nella creazione del file: %s \n" , name_file );
									b = 0 ;
								}
							}					
						}
						else
						{
							char errore[_MAX_STRING_ERROR];
							memset(errore , '\0' ,_MAX_STRING_ERROR);

							int rc = is_GaFile(path_filetemp , errore);
	
							if(rc == -1)
							{
								printf("%s \n" ,errore);
								close(sockfd);
								return 0;
							}
							if(rc)
							{	
								if(!print_ga_txt(path_filetemp , PATH_GA_TXT , max_line) )
								{
									printf("\e[1;31mErrore di codifica del file Ga \033[m\n");
								}
								else
								{
									printf("\e[1;32mConversione da GA a TXT riuscita :) \033[m \n\n");
									
									int *stato = NULL;
									
									int pid =fork();
									if(pid == 0)
									{
										char comando[501];
										memset(comando , '\0' , 501);

										snprintf( comando , 500*sizeof(char)," cat %s ; bash " , realpath(PATH_GA_TXT, NULL) );

										execlp( "xterm" , "xterm" , "-e ", comando ,NULL);

									}
									waitpid(pid , stato , 0);

								}
							}
							else
							{
								int boolean = 0;
								do
								{
									int *stato = NULL;
									char com[500];
									char risp[10];
									
									memset( com , '\0' , 500 );
									memset( risp , '\0' ,10 );
									 
									printf("Inserisci il nome di unprogramma per aprire il file: ");
									fgets( com , 100 , stdin);
									printf("\n");
									com[strlen(com)-1] = '\0';
									int pid = fork();
									if(pid == 0)
									{
									 execlp( com , com , path_filetemp , NULL);
									}
									waitpid(pid , stato , 0);
									
									setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
									
									puts("\nSi vuole aprire il file con un nuovo programma (y/n)? ");
									fgets( risp , 9 , stdin);
									puts("\n");
									setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
									
									if((strncmp(risp ,"y" , 1)== 0) || (strncmp(risp ,"Y" , 1)== 0) || (strncmp(risp ,"yes" , 3)== 0) ||(strncmp(risp ,"Yes" , 3)== 0))
									{
										boolean = 1;
									}
									else
									{
										boolean = 0;
									}
									
								}
								while(boolean);
								
							}
							
							close(fd);
							b = 1;
						}
					}
					
					else
					{
						b = 0;
						close(fd);
					}
					
					
				}
			}
		
			if(msgD->pay_desc == _DIAG_ERROR)
			{
				
				if(msgD->param_state == _NO_DOM_ERROR)
				{
					IDT_Message* buffer;
					buffer = (IDT_Message *) calloc ( 1 , sizeof(IDT_Message));
					int rs;
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
							printf("Aggiornamento dominio eseguito con successo riprovare il comando \n");
						}

					}
					free(buffer);
				}
				
			}
			free(msgD);
		}
		else	
		{
			free(msgD);
			b = 0;
		}
	}
	else
	{
		b = 0;
	}	
	
	free(msg);
	close(sockfd);
	
	
	return b;
}


// operazione di lettura su un file di tipo ga, ritorna: 1 se l'operazione è riuscita, 0 se c'e' qualche errore
// Parameri-> path del file ga da leggere, eventuale errore restituito
int op_readga(char *arg , char *errore)
{
	if( isFile(arg) == 0)
	{
		strcpy(errore , "Errore risorsa non trovata !!!!");
		return 0;
	}
	
	int rc = is_GaFile(arg , errore);
	
	if(rc == -1)
		return 0;

	if(rc)
	{	
		
		if(!print_ga_txt(arg , PATH_GA_TXT , max_line) )
		{
			printf("\e[1;31mErrore di codifica del file Ga \033[m\n");
		}
		else
		{
			printf("\e[1;32mConversione da GA a TXT riuscita :) \033[m \n\n");
			
			int *stato = NULL;
			
			int pid =fork();
			if(pid == 0)
			{
				char comando[501];
				memset(comando , '\0' , 501);

				snprintf( comando , 500*sizeof(char)," cat %s ; bash " , realpath(PATH_GA_TXT, NULL) );

				execlp( "xterm" , "xterm" , "-e ", comando ,NULL);
				
			}
			waitpid(pid , stato , 0);

		}
	}
	else
	{
		strcpy(errore , "\e[1;31mLa risorsa inserita non è di tipo GA!!!! \033[m \n");
		return 0;
	}
	
	return 1;

}



// operazione generica di scrittura sul server che avvia una transazione, ritorna: 1 se l'operazione è andata a buon fine, 0 se c'e un errore
// Parametri-> il tipo di comando, lista argomenti del comando, elemento shell , la descrizione del tipo di errore 
int op_transaction( int op , TokenArgs *list_arg , Shell_El *shell , char *errore )
{

	short int b;
	InitMessage* msg;
	int id = -1;
	  
	char temp_res_net[_DIM_RES];
	char temp_res_loc[_DIM_RES];
	int cap = -1;
	
	memset(temp_res_net , '\0' , _DIM_RES);
	memset(temp_res_loc , '\0' , _DIM_RES);
	
	
	if(op == _SHELL_RMCAP || op == _SHELL_MODCAP || op == _SHELL_UPLOAD || op == _SHELL_MVCAP)
	{
		if( isFile(list_arg->args[0]) )
		{
			strcpy(temp_res_loc , realpath(list_arg->args[0], NULL) );
		}
		else
		{
			strcpy(errore , "\e[1;31mIl file locale selezionato è inesistente\033[m \n");
			return 0;
		}
		
		int i;
		int fd = open(temp_res_loc, O_RDONLY);
		int n = count_CapGA(fd);
		int short uscita = 0;
		for( i = 2; i < (list_arg->l_arg) ; ++i)
		{
			if(atoi(list_arg->args[i]) > n)
				uscita = 1;
		}
		close(fd);
		
		if(uscita)
		{
			strcpy(errore , "\e[1;31mLe posizioni dei capitoli sono errate\033[m \n");
			return 0;
		}
		
	}
	
	if(op == _SHELL_UPLOAD)
	{
		if(strncmp("/" , list_arg->args[1] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[1]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			
			if(strlen(list_arg->args[1]) > 0)
			{
				strcat(temp_res_net , "/");
				strcat(temp_res_net , list_arg->args[1]);
			}
		}
		strcat(temp_res_net , "/");
		
		char tmp[301] = {0x0};
		get_namefile(tmp, 300, temp_res_loc);
		
		strcat(temp_res_net , tmp);
		
		msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_FILE , OPT2_ISTR_NOIS);
		
	}
	
	if ( op == _SHELL_MODCAP )
	{	
		if(strncmp("/" , list_arg->args[1] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[1]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			strcat(temp_res_net , "/");
			strcat(temp_res_net , list_arg->args[1]);
		}
		
		cap = atoi(list_arg->args[2] );
		
		msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_FILE , OPT2_ISTR_IS);
	}

	if (  op == _SHELL_NEWCAP)
	{	
		if(strncmp("/" , list_arg->args[0] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[0]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			strcat(temp_res_net , "/");
			strcat(temp_res_net , list_arg->args[0]);
		}
		
		if(strlen(list_arg->args[1]) > 0)
			cap = atoi(list_arg->args[1] );
		
		msg = init_InitMessage( _INIT_START_TR, temp_res_net , strlen(temp_res_net) , OPT1_ISTR_FILE , OPT2_ISTR_IS);
	}

	if ( op == _SHELL_MKDIR )
	{
		if(strncmp("/" , list_arg->args[1] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[1]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			
			if(strlen(list_arg->args[1]) > 0)
			{
				strcat(temp_res_net , "/");
				strcat(temp_res_net , list_arg->args[1]);
			}
		}
		strcat(temp_res_net , "/");
		strcat(temp_res_net , list_arg->args[0]);
		msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_DIR , OPT2_ISTR_NOIS);
	} 
	
	if ( op == _SHELL_NEWGA )
	{
		if(strncmp("/" , list_arg->args[1] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[1]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			
			if(strlen(list_arg->args[1]) > 0)
			{
				strcat(temp_res_net , "/");
				strcat(temp_res_net , list_arg->args[1]);
			}
		}
		
		strcat(temp_res_net , "/");
		strcat(temp_res_net , list_arg->args[0]);
		msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_FILE , OPT2_ISTR_NOIS);
	} 
	

	if ( op == _SHELL_RMDIR )
	{	
		if(strncmp("/" , list_arg->args[0] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[0]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			strcat(temp_res_net , "/");
			strcat(temp_res_net , list_arg->args[0]);
		}
		
		msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_DIR , OPT2_ISTR_IS);
	}		

	 if(op == _SHELL_RM)
	 {
	 	 if(strncmp("/" , list_arg->args[0] , 1) == 0)
		 {
			 strcpy(temp_res_net , shell->path_dom);
			 strcat(temp_res_net ,list_arg->args[0]);
		 }
		 else
		 {
			 strcpy(temp_res_net , shell->path_res);
			 strcat(temp_res_net , "/");
			 strcat(temp_res_net , list_arg->args[0]);
		 }
	
		 msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_FILE , OPT2_ISTR_IS);
	 }

	if( op == _SHELL_MVCAP )
	{
		
		if(strncmp("/" , list_arg->args[1] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[1]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			strcat(temp_res_net , "/");
			strcat(temp_res_net , list_arg->args[1]);
		}
		int temp;
		temp = atoi(list_arg->args[2] );
		
		cap = atoi(list_arg->args[3] );
		
		int fd = open(temp_res_loc, O_RDONLY);
		id = get_id_pos(fd, temp);
		close(fd);
		
		msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_FILE , OPT2_ISTR_IS);	
	}

	if(op == _SHELL_RMCAP)
	{
		
		if(strncmp("/" , list_arg->args[1] , 1) == 0)
		{
			strcpy(temp_res_net , shell->path_dom);
			strcat(temp_res_net ,list_arg->args[1]);
		}
		else
		{
			strcpy(temp_res_net , shell->path_res);
			strcat(temp_res_net , "/");
			strcat(temp_res_net , list_arg->args[1]);
		}
		int temp;
		temp = atoi(list_arg->args[2] );
		
		int fd = open(temp_res_loc, O_RDONLY);
		id = get_id_pos(fd, temp);
		close(fd);
		
		msg = init_InitMessage( _INIT_START_TR , temp_res_net , strlen(temp_res_net) , OPT1_ISTR_FILE , OPT2_ISTR_IS);	
	}

	int count_prove = 0;
	int sockfd;
	short int true_con = 0;
	
	while(count_prove < 3 && true_con == 0)
	{
		if( ( sockfd = connessioneClient("127.0.0.1" /*shell->ip*/ ,  shell->port , 3) ) <= 0 ) 
		{
			count_prove++;
			sleep(2 * count_prove);
		}
		else
		{
			true_con  = 1;
		}
	}
	
	if(true_con == 0 )
	{
		short int b;
		b = 1;
		int index_server;
		
		while(b)
		{
			index_server = generatoreRandom(len_list_server);
			if( ( sockfd = connessioneClient(list_server[index_server].ip ,  list_server[index_server].port , 3) ) >= 0 ) 
				b = 0 ;
		}
	}
	
	set_timeout(sockfd  , timeout_init_msg  , 0  , timeout_init_msg  , 0 );
	
	if(send_InitMessage( msg , sockfd ))
	{
		b = init_transaction(sockfd ,temp_res_loc , temp_res_net , cap , id , op , shell , errore);
	}
	else
	{
		b = 0;
	}	
	
	free(msg);
	close(sockfd);

	return b;
}
