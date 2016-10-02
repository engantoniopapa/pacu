#include "header.h"


int menu()
{	
	int i ;
	int l_news = 500;
	char news[l_news];
	
	memset( news , '\0' , l_news );
	
	while(1)
	{
	//system("clear");
	
	if(strlen(news) == 0)
		figlet1();
	else
		printf("\n%s" , news);
	
	printf("\n\n\n");
	printf("+-+--+-+\n");
	printf("+ Menu +\n");
	printf("+-+--+-+\n");

	memset(news , '\0' , l_news);

	printf("\n");
	printf("1- Start \n");
	printf("2- Setting \n");
	printf("3- Exit \n\n");
	
	 if(scanf("%d",&i) == 0)
	 {
	 	 while(getchar() != '\n'); // per svuotare l'stdin
	 	 i = 0;
	 }

	
		switch (i) 
		{
			/* 
			 * Handling options 
			 */ 
			case 1:  /* Start */
			//system("clear");
			return 1;


			case 2:  /* Setting */
			{
				int dim = 500;
				char notice[dim];
				memset(notice , '\0' , dim);
				while(setting(notice , dim) != 4) ;
			}break;

			case 3:  /* Exit */
			system("clear");
			figlet2();
			exit(0);


			case 66: /* Amministrator mode */
			{
				system("clear");
				printf("\n");
				printf("+-+-+-+-+-+-+-+-+-+-+\n");
				printf("+ Salve mio Signore +\n");
				printf("+-+-+-+-+-+-+-+-+-+-+\n\n");
				return 66;
			}
			break;

			default:
			{
				strcpy(news ,"\e[0;31mSelezione errata \033[m");
			}
			break;
		}
	}
}


int setting(char * notice , int dim)
{
	int i ;
	system("clear");
	
	if( strlen(notice) == 0)
	{;}
	else
		printf("\n%s " , notice);
		 	
	printf("\n\n\n");
	printf("+-+-+-+-+-+\n");
	printf("+ Setting +\n");
	printf("+-+-+-+-+-+\n");

	memset(notice , '\0' , dim);

	printf("\n");
	printf("1- Visualizza la configurazione di sistema \n");
	printf("2- Modifica configurazione \n");
	printf("3- Carica le impostazioni di default  \n");
	printf("4- Torna al menu \n\n");
	
	if(scanf("%d",&i) == 0)
	{
		while(getchar() != '\n'); // per svuotare l'stdin
		i = 0;
	}

	switch (i) 
	{
		/* 
		 * Handling options 
		 */ 
		case 1:  /* Visualizza la configurazione di sistema */
		{
			system("clear");	
			print_conf();
			strcpy(notice , " ");
			return 1 ;
		}
		break;			

		case 2:  /* Modifica configurazione */
		{
			if( mod_setting() )
			{
				;
			}
			else
			{
			  ;
			}
			return 2 ;
		}
		break;

		case 3:  /* Carica impostazioni di default */
		{
			char scelta[10];
			memset( scelta , '\0' , 10);
		
			printf("\nCaricando le impostazioni di default si perderanno le modifiche alla configurazione.\nSicuri di voler continuare (y/n)?\n");
			while(getchar() != '\n'); // per svuotare l'stdin
		
			scanf( "%s" , scelta);
		
			if((strncmp(scelta ,"y" , 1) == 0) || (strncmp(scelta , "Y" , 1) == 0) || (strncmp(scelta , "yes" , 3) == 0) ||(strncmp(scelta , "Yes" , 3) == 0))
			{
				if(!cp_rewrite_file(PATH_DB_DEFCONF , PATH_DB_CONF , 1024 ) )
					exit(0);
				
				strcpy(notice , "\e[1;32mConfigurazione di sistema caricata correttamente :) \033[m");
			}
		
			return 3 ;
		}
		break;

		case 4:  /* Torna al menu */
		{
			return 4 ;
		}
		break;
	
		default:
		{
			strcpy(notice , "\e[1;31mSelezione errata \033[m");
			return 0 ;
		}
		break;
	}
	return 0;
}



int mod_setting()
{

	DB *db_conf; // descrittore del DB Conf
	int rc;
	char *stringa_errore;
	int i;
	
	db_conf = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	
	rc = open_db_rw( db_conf , PATH_DB_CONF , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  
	int l_news = 2000;
	char news[l_news];
	
	memset( news , '\0' , l_news );
	
	while(1)
	{
		//system("clear");
	
		if( strlen(news) == 0)
		{;}
		else
		printf("\n%s " , news);
		
		printf("\n\n\n");
  	printf("<<< Configurazione generale: >>>\n");
		printf("1)Modifica il percorso della cartella dei file scaricati \n");
		printf("2)Modifica il percorso della cartella dei file temporanei \n");
  	printf("\n<<< Configurazione avanzata: >>>\n");
  	printf("3) Modifica IP e Porta del Server di Boot \n");
  	printf("4) Modifica il numero di tentativi di connessione al Server di Boot \n");
  	printf("5) Modifica la dimensione del blocco lettura/scrittura in byte (solo per utenti esperti !!!) \n") ;
  	printf("6) Modifica l'interfaccia di rete \n") ;
  	printf("\n<<< Torna a Setting: >>>\n");
  	printf("7) Vai al menu precedente \n") ;
  	printf("\n\n");
  	
  	memset( news , '\0' , l_news );
  	
		if(scanf("%d",&i) == 0)
		{
			while(getchar() != '\n'); // per svuotare l'stdin
			i = 0;
		}
  	
  	switch (i) 
		{
  		case 1:
  		{
  			char query[1501];
  			char valore[1000];
  			
  			memset(valore , '\0' , 1000);
  			memset(query , '\0' , 1501);
  			
  			while(getchar() != '\n'); // per svuotare l'stdin
  			printf("Inserisci il percorso della cartella\n");
  			scanf("%[^\n]", valore);
  			
  			snprintf(query , 1500*sizeof(char) , "Update CharParam set valore = '%s' where nome = 'in_path' " , valore);
  			
  			if( generic_op_db(db_conf , query , stringa_errore) )
  			{
  				strcpy(news , "\e[1;32mOperazione riuscita :) \033[m");
  			}
  			else
  			{
  				strcpy(news , "\e[1;31m");
  				strcat(news , stringa_errore);
  				strcat(news , "\033[m");		
  			}
  			
  		}break;
  		
  		case 2:
  		{
  			char query[1501];
  			char valore[1000];
  			
  			memset(valore , '\0' , 1000);
  			memset(query , '\0' , 1501);
  			
  			while(getchar() != '\n'); // per svuotare l'stdin
  			printf("Inserisci il percorso della cartella\n");
  			scanf("%[^\n]", valore);
  			
  			snprintf(query , 1500*sizeof(char) , "Update CharParam set valore = '%s' where nome = 'tmp_path' " , valore);
  			
  			if( generic_op_db(db_conf , query , stringa_errore) )
  			{
  				strcpy(news , "\e[1;32mOperazione riuscita :) \033[m");
  			}
  			else
  			{
  				strcpy(news , "\e[1;31m");
  				strcat(news , stringa_errore);
  				strcat(news , "\033[m");		
  			}
  		}break;
  		
  		case 3:
  		{
  		
  			char query[501];
  			char ip[_DIM_IPV4];
  			int porta;
  			
  			memset(ip , '\0' , _DIM_IPV4);
  			memset(query , '\0' , 501);
  			
  			while(getchar() != '\n'); // per svuotare l'stdin
  			printf("Inserisci l'ip del Boot Server: ");
  			scanf("%s", ip);
  			printf("\nInserisci la porta del Boot Server: ");
  			scanf("%d", &porta);
  			printf("\n");
  			
  			snprintf(query , 500*sizeof(char) , "Update BootServer set ip = '%s' , port = %d where id_server = 0 " , ip , porta);
  			
  			if( generic_op_db(db_conf , query , stringa_errore) )
  			{
  				strcpy(news , "\e[1;32mOperazione riuscita :) \033[m");
  			}
  			else
  			{
  				strcpy(news , "\e[1;31m");
  				strcat(news , stringa_errore);
  				strcat(news , "\033[m");		
  			}
  		
  		}break;
  		
  		case 4:
  		{
  		
  			char query[501];
  			int tentativi;
  			
  			memset(query , '\0' , 501);
  			
  			while(getchar() != '\n'); // per svuotare l'stdin
  			printf("Inserisci il numero di tentativi di conessione con il Boot Server: ");
  			scanf("%d", &tentativi);
  			printf("\n");
  			
  			snprintf(query , 500*sizeof(char) , "Update Parametri set valore = '%d' where nome = 'tentativi_boot' " , tentativi);
  			
  			if( generic_op_db(db_conf , query , stringa_errore) )
  			{
  				strcpy(news , "\e[1;32mOperazione riuscita :) \033[m");
  			}
  			else
  			{
  				strcpy(news , "\e[1;31m");
  				strcat(news , stringa_errore);
  				strcat(news , "\033[m");		
  			}
  		
  		}
  		break;
  		
  		case 5:
  		{
  		  char query[501];
  			int dim;
  			
  			memset(query , '\0' , 501);
  			
  			while(getchar() != '\n'); // per svuotare l'stdin
  			printf("Inserisci la nuova dimensione in byte del blocco lettura-scrittura: ");
  			scanf("%d", &dim);
  			printf("\n");
  			
  			snprintf(query , 500*sizeof(char) , "Update Parametri set valore = '%d' where nome = 'max_line' " , dim);
  			
  			if( generic_op_db(db_conf , query , stringa_errore) )
  			{
  				strcpy(news , "\e[1;32mOperazione riuscita :) \033[m");
  			}
  			else
  			{
  				strcpy(news , "\e[1;31m");
  				strcat(news , stringa_errore);
  				strcat(news , "\033[m");		
  			}
  		}
  		break;
  		
  		case 6:
  		{
  			char query[1501];
  			char valore[1000];
  			
  			memset(valore , '\0' , 1000);
  			memset(query , '\0' , 1501);
  			
  			while(getchar() != '\n'); // per svuotare l'stdin
  			printf("Inserisci l'interfaccia di rete\n");
  			scanf("%[^\n]", valore);
  			
  			snprintf(query , 1500*sizeof(char) , "Update CharParam set valore = '%s' where nome = 'interf_network' " , valore);
  			
  			if( generic_op_db(db_conf , query , stringa_errore) )
  			{
  				strcpy(news , "\e[1;32mOperazione riuscita :) \033[m");
  			}
  			else
  			{
  				strcpy(news , "\e[1;31m");
  				strcat(news , stringa_errore);
  				strcat(news , "\033[m");		
  			}
  			
  		}break;
  		
  		case 7:
  		{
  			rc = close_db(db_conf, stringa_errore);
  			if( !rc )
  			{
    			fprintf(stderr, "%s\n", stringa_errore);
    			exit(1);
  			}
	
				free(stringa_errore);
				free(db_conf);
  			
  			return 7;
  		}break;
  		
			default:
			{
				strcpy(news , "\e[1;31mSelezione errata \033[m");
			}break;
		}
	}

	return 0;

}




void figlet1()
{
printf("\e[1;34m");
printf("                      ____        ____        _____ ____  \n");
printf("                     |  _ \\ __ _ / ___|   _  |  ___/ ___|  \n");
printf("                     | |_) / _` | |  | | | | | |_  \\___ \\ \n");
printf("                     |  __/ (_| | |__| |_| | |  _|  ___) |   \n");
printf("                     |_|   \\__,_|\\____\\__,_| |_|   |____/ \n");
printf("\033[m \n");
	
}


void figlet2()
{
printf("\e[1;34m");
printf(" \n");
printf("                      ____ 							\n");            
printf("                     | __ ) _   _  ___ 	\n");
printf("                     |  _ \\| | | |/ _ \\ \n");
printf("                     | |_) | |_| |  __/  \n");
printf("                     |____/ \\__, |\\___|  \n");
printf("                            |___/      \n");
printf("\033[m \n");
printf("\e[1;35m@Copyright: Pacu Software House :) \033[m\n\n");
	

}





