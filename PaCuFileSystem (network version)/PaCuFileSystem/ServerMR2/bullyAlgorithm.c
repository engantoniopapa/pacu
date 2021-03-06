#include "header.h"

void *bully_function(void *l);
void *coordinator_function(void *l);


//inizializza la struttura electionStatus, ritorna per riferimento la sua lunghezza
electionStatus *init_electionStatus( int *len)
{
	Dominio * dom_list_th ;
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore ;
	char cond[_DIM_COND];

	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char) );
	 
	if( !open_db_ro( db_dom , PATH_DB_TMPDOM , stringa_errore))
	{
	  fprintf(stderr, "%s\n", stringa_errore);
	  exit(1);
	}
	
	memset( cond , '\0' , _DIM_COND );
	
	dom_list_th = create_list_domini(db_dom , len , cond, stringa_errore );
	
	if( !close_db(db_dom, stringa_errore))
	{
  	fprintf(stderr, "%s\n", stringa_errore);
  	exit(1);
	}

	free(db_dom);
	free(stringa_errore);

	electionStatus *list_bully;
	
	list_bully = (electionStatus *) calloc( *len , sizeof(electionStatus));
	
	int i;
	for (i = 0; i < *len; i++)
	{
		strncpy(list_bully[i].nomeDominio, dom_list_th[i].name ,_DIM_DOM);
		list_bully[i].status = 0;
	}
	
	return list_bully;
}



int start_bully(char *nomeServer , char *oldPrimary , char *dominio , Server *vista , int len_vista)
{//printf("sono nella funzione ;P \n");
	char vecchio_primary[60] ={0x0};
	strncpy(vecchio_primary , oldPrimary , 59);
	int i, countServer;
	int countOk_bully;
	countServer = 0;
	countOk_bully = 0;
	//printf("1 \n");
	lock_Two_rooms(sem_bully);
		for ( i = 0; i < len_primariesElection ; i++)
		{
			if(strcmp(primariesElection[i].nomeDominio, dominio) == 0)
			{
				primariesElection[i].status = 1;
				break;
			}
		}
	unlock_Two_rooms(sem_bully);
		//printf("2 \n");
	pthread_t bullyServer[len_vista];
	ContainerBully_th container[len_vista];
		//printf("3 \n");
	for(i = 0; i < len_vista; i++)
	{
		//printf("4 \n  -- nomeserver = %s , len = %d ,vista[i].name %s \n" ,nomeServer,len_vista, vista[i].name );
		if(strcmp(nomeServer, vista[i].name) < 0)
		{
			//printf("dentro l'if \n");
			
			container[i].server = &vista[i] ;
			container[i].dominio = dominio;
			container[i].countOK = &countOk_bully;
			container[i].primary = vecchio_primary;
			//printf("creo il thread giudice %d \n", countServer);
			pthread_create( &bullyServer[countServer], NULL, bully_function, (void *)&container[i]);
			countServer++;
		}
	} 
	

	for(i=0; i < countServer; i++)
	{
		pthread_join( bullyServer[i], NULL); 
	}

   //printf("finito di aspettare i thread giudici \n");
   
	if(countOk_bully == 0) // sono il nuovo primary
	{
		   //printf("procedura elezione di me stesso come primary \n");
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
		snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from Dom  where server = '%s' AND dominio ='%s'" , vecchio_primary , dominio);
		
		memset( string_query2 , '\0' , _DIM_QUERY_COND*sizeof(char) );
		snprintf(string_query2 , _DIM_QUERY_COND*sizeof(char) , "update Dom  set type_server = 1 where server = '%s' AND dominio ='%s'" , name_server , dominio);

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
			snprintf(str_cond , _DIM_COND , " where server = '%s' " , vecchio_primary );

			tmp_count = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
			 
		unlock_WR_Writers(sem_vista);
		
		//printf(" 1) procedura primary \n ");
		if( tmp_count == 0) // se un server nella lista domini non compare + viene cancellato anche dalla lista server del server
		{		//printf(" 2) procedura primary \n ");
			DB *db_server; // descrittore del DB Domini
			db_server = ( DB *) malloc (sizeof(DB) ) ;
			
			if( !open_db_rw( db_server , PATH_DB_PRIMARY , stringa_errore))
			{
				fprintf(stderr, "%s\n", stringa_errore);
				exit(1);
			}
			
			memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
			snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from ListServer  where name = '%s'" ,vecchio_primary );
			
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
		//printf(" 3) procedura primary \n ");
				
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
  			//printf(" 4) procedura primary \n ");
		free(db_dom);
		free(stringa_errore);
				//printf(" 5) procedura primary \n ");
				
		lock_WR_Reader(sem_list_serv);
		
		int tmp_len_server = len_list_server;
		pthread_t bullyCord[tmp_len_server];
		ContainerBully_th container_cord[tmp_len_server];
					//printf(" 6) procedura primary - lunghezza lista server = %d \n ", tmp_len_server);
					

		for(i = 0; i < tmp_len_server; i++)
		{
			container_cord[i].server = &list_server[i] ;
			//printf(" 6.1) procedura primary --- %s--\n " , vecchio_primary);
			container_cord[i].dominio = dominio;
			container_cord[i].countOK = NULL ;
			container_cord[i].primary = vecchio_primary;
			//printf(" 6.2) procedura primary \n ");
			pthread_create( &bullyCord[i], NULL, coordinator_function, (void *)&(container_cord[i]));
		}

		
		//printf(" 6.5) procedura primary \n ");
		for(i=0; i <  tmp_len_server; i++)
		{
			 pthread_join( bullyCord[i], NULL); 
			 //printf(" test join\n ");
		}		
		unlock_WR_Reader(sem_list_serv);
						//printf(" 7) procedura primary \n ");
		lock_Two_rooms(sem_bully);
			for ( i = 0; i < len_primariesElection ; i++)
			{
				if(strcmp(primariesElection[i].nomeDominio, dominio) == 0)
				{
					//printf(" 7.1) procedura primary \n ");
					primariesElection[i].status = 0;
					break;
				}
			}
		unlock_Two_rooms(sem_bully);
						//printf(" 8) procedura primary \n ");
		resolv_pronto_newprimary(dominio); 
						//printf(" 9) procedura primary \n ");
		return 1;
	}
	
	//printf("non sono il nuovo primary \n");
	return 0;   
}

void *bully_function(void *l)
{
//printf("nel thread giudice \n ");
	ContainerBully_th *container ;
	container = (ContainerBully_th *) l;
	
	int connsd;
	
	connsd = connect_ServerServer(container->server->ip , *port_server_pr , 2) ;
	//printf("connessione bully , consd = %d , ip = %s , port = %d \n" , connsd , container->server->ip , *port_server_pr );
	
	if( connsd > 0)
	{	
		PR_Message* msg;
		msg = init_PR_Message( _PR_BULLY_ELECT , container->server->name ,  container->dominio , 0 , 0 , container->primary);
		set_timeout(connsd  , timeout_pr  , 0  , timeout_pr  , 0 );	
		
		if(send_PR_Message(connsd , msg ) != 1 )
		{
			close(connsd);
			free(msg);
			pthread_exit(NULL);
		}
		
		free(msg);
		msg = (PR_Message*) calloc( 1 , sizeof(PR_Message) );
		
		if(recv_PR_Message(connsd , msg ) != 1 )
		{
			close(connsd);
			free(msg);
			pthread_exit(NULL);
		}
		
		if(msg->pay_desc == _PR_ACK)
		{
			//puts("Ottenuto OK");
			(*(container->countOK))++;
		}

		close(connsd);
	}
	pthread_exit(NULL);
}

void *coordinator_function(void *l)
{


	ContainerBully_th *container ;
	container = (ContainerBully_th *) l;
//printf("connessione bullycoordinator  , container->server->name = %s\n",container->server->name);	
	int connsd;

	connsd = connect_ServerServer(container->server->ip , *port_server_pr , 2) ;
		//printf("connessione bullycoordinator , consd = %d , ip = %s , port = %d \n" , connsd , container->server->ip , *port_server_pr );
	if( connsd > 0)
	{	
		PR_Message* msg;
		msg = init_PR_Message( _PR_BULLY_PRIMARY, name_server ,  container->dominio , 0 , 0 , container->primary);
		set_timeout(connsd  , timeout_pr  , 0  , timeout_pr  , 0 );
		
		if(send_PR_Message(connsd , msg ) != 1 )
		{
			close(connsd);
			free(msg);
			pthread_exit(NULL);
		}
		
		close(connsd);
		free(msg);
		//printf("-- %d , %s , %s , %s \n" , _PR_BULLY_PRIMARY ,container->server->name ,  container->dominio , container->primary);
		pthread_exit(NULL);
	}
	
	pthread_exit(NULL);
}



// crea la vista Bully per un dato dominio, ritorna: la stringa formattata domini - server primary
// Parametri -> nome dominio , path del db , lunghezza struttura 
Server *vistaBully(char *dom , char *path_db ,	int *len )
{
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore ;
	char string_query[_DIM_QUERY_COND];
	char cond[_DIM_COND];
	Server *list; 
	ResultQuery *struct_list;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char) );
	
	
	 
  if( !open_db_ro( db_dom , path_db , stringa_errore))
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
	snprintf(string_query ,_DIM_QUERY_COND*sizeof(char),"select server , ip , port from Dom  where dominio = '%s' AND type_server = 2 AND server <> '%s' ", dom , name_server);

	memset( cond , '\0' , _DIM_COND );
	snprintf(cond , _DIM_COND*sizeof(char) ," where dominio = '%s' AND type_server = 2 AND server <> '%s' ", dom , name_server);
	
	struct_list = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );

	lock_WR_Reader(sem_vista);
			*len = count_tb( db_dom , "Dom" , cond , stringa_errore);	

			if( *len == -1 )
			{
				fprintf(stderr, "%s\n", stringa_errore);
				list = NULL;
			}
			else
			{
				if( *len  == 0)
				{
					list = NULL;
				}
				else
				{
					list = calloc( *len , sizeof(Server));
	
					struct_list->object = (void*) list;
					struct_list->count = *len;
					struct_list->l_row = 0;

					if(!select_on_bufferServer(db_dom , string_query , struct_list , stringa_errore ) )
					{
						fprintf(stderr, "%s\n", stringa_errore);
						*len = 0;
						free(list);
						list = NULL;;
					}
				}
			}
	unlock_WR_Reader(sem_vista);
	
	free(struct_list);

  if( !close_db(db_dom, stringa_errore)){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(db_dom);
	free(stringa_errore);

	return list;
}

