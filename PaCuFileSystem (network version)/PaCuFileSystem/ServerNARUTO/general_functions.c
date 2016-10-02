#include "header.h"



// inizializzazione delle variabili di configurazione
void get_conf()
{
	int rc;
	char *stringa_errore;
	char *risultato_string;
	DB *db_conf; // descrittore del DB Conf
	DB *db_dom; // descrittore del DB Domini
	DB *db_server; // descrittore del DB Server

	char *sql_com	 = "select * from Dom";
	char cond[_DIM_COND];

	db_conf = ( DB *) malloc (sizeof(DB) ) ;
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	db_server = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	color = (char*) calloc ( 1 , sizeof(char) * 9 );	
	port_client =  ( in_port_t *) calloc (1,sizeof(in_port_t )); // porta per la comunicazione client - server
	port_server =  ( in_port_t *) calloc (1,sizeof(in_port_t )); // porta per la comunicazione server - server
	port_client_pp =  ( int *) calloc (1,sizeof(int )); // porta per la comunicazione client-server relativa allo scambio ping-pong
	port_server_pp =  ( int *) calloc (1,sizeof(int)); // porta per la comunicazione server-server relativa allo scambio ping-pong
	port_server_pr =  ( int *) calloc (1,sizeof(int)); // porta per la comunicazione server-server relativa allo scambio ping-pong
	name_server = (char *)calloc(1 , (_DIM_NAME_SERVER) * sizeof(char) ); // nome del server il + il terminatore
	
	strcpy( color , "\e[1;34m" );

	rc = open_db_ro( db_conf , PATH_DB_CONF , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	rc = open_db_ro( db_dom , PATH_DB_DOMINI , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	rc = open_db_ro( db_server , PATH_DB_PRIMARY , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	crash = select_conf_param(db_conf , "crash" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	*port_client = htons( select_conf_param(db_conf , "port_client" , stringa_errore , &rc)); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	*port_server = htons( select_conf_param(db_conf , "port_server" , stringa_errore , &rc) ); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	*port_client_pp = select_conf_param(db_conf , "port_cl_pp" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	*port_server_pp = select_conf_param(db_conf , "port_serv_pp" , stringa_errore , &rc) ; 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
 
  *port_server_pr = select_conf_param(db_conf , "porta_pr" , stringa_errore , &rc) ; 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	backlog_client = select_conf_param(db_conf , "backlog_client" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	backlog_server = select_conf_param(db_conf , "backlog_server" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

  thread_helper_c = select_conf_param(db_conf , "thread_helper_client" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	thread_helper_s = select_conf_param(db_conf , "thread_helper_server" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	thread_ppSrv = select_conf_param(db_conf , "thread_ppSrv" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	thread_ppCl = select_conf_param(db_conf , "thread_ppCl" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	thread_prSrv = select_conf_param(db_conf , "thread_helper_pr" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	max_line = select_conf_param(db_conf , "max_line" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	time_helper = select_conf_param(db_conf , "time_helper" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	tr_max_cap = select_conf_param(db_conf , "max_cap" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	timeout_init_msg = select_conf_param(db_conf , "timeout_init_msg" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	timeout_domlist = select_conf_param(db_conf , "timeout_domlist" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  timeout_tr = select_conf_param(db_conf , "timeout_tr" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  timeout_pp = select_conf_param(db_conf , "timeout_pp" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  timeout_pr = select_conf_param(db_conf , "timeout_pr" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  timeout_bully = select_conf_param(db_conf , "timeout_bully" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  time_helper = select_conf_param(db_conf , "time_helper" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  time_helper_pp = time_helper;
	time_helper_c = time_helper;
	time_helper_s = time_helper * 2 ;
	
	wait_rep = select_conf_param(db_conf , "wait_rep" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  wait_pri = select_conf_param(db_conf , "wait_pri" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  ciclo_bully_static = select_conf_param(db_conf , "ciclo_bully_f" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  ciclo_bully_random = select_conf_param(db_conf , "ciclo_bully_r" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	memset(name_server ,'\0' , _DIM_NAME_SERVER );
	id_server = 0 ;
	rc = select_server_NameId(db_conf , name_server , &id_server, stringa_errore );	
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }


	memset( cond , '\0' , _DIM_COND );
	snprintf(cond , _DIM_COND*sizeof(char) ," where name <> '%s' " , name_server );
  
  lock_WR_Writers(sem_list_serv);
		list_server = create_list_server(db_server ,  &rc , "ListServer" , cond , stringa_errore );
		if( !rc )
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  exit(1);
		}
  unlock_WR_Writers(sem_list_serv);

  memset( cond , '\0' , _DIM_COND );
	strcpy(cond ," where type_server = 1 ");
	
	lock_WR_Writers(sem_list_dom_serv);
		string_dom_serv = create_string_dom_serv(db_dom ,  &rc , "Dom" , cond , stringa_errore );
		if( !rc )
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  exit(1);
		}
  unlock_WR_Writers(sem_list_dom_serv);
  
  fs_path = select_conf_charparam(db_conf , "fs_path" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  tmp_path = select_conf_charparam(db_conf , "tmp_path" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	iterface_network = select_conf_charparam(db_conf , "interf_network" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  get_myip(iterface_network , my_ip , _DIM_IP);

	int g;
	lock_WR_Reader(sem_list_serv);
		for( g = 0 ; g < 5 ; g++)
			printf( "%d - name = %s  , ip = %s , port = %d \n" , g , list_server[g].name , list_server[g].ip , list_server[g].port );
	unlock_WR_Reader(sem_list_serv);
	
	printf("\nParametri caricati nel sistema:\n");
	printf("%s<<<<< --- COLORE --- >>>>>  \033[m \n" , color );
	printf("%scrash =  \033[m %d \n", color , crash);
	printf("%sbacklog_client  \033[m %d \n", color , backlog_client );
	printf("%sbacklog_server  \033[m %d \n", color , backlog_server );
	printf("%sport_client \033[m %d \n", color , ntohs(*port_client) );
	printf("%sport_server \033[m %d \n", color , ntohs(*port_server) );
	printf("%sport_client ping-pong \033[m %d \n", color , *port_client_pp );
	printf("%sport_server ping-pong\033[m %d \n", color , *port_server_pp );
	printf("%sport_server PR\033[m %d \n", color , *port_server_pr );
	printf("%sthread_helper_client \033[m %d \n", color , thread_helper_c );
	printf("%sthread_helper_server \033[m %d \n", color , thread_helper_s);
	printf("%sthread_ppCl \033[m %d \n", color , thread_ppCl);
	printf("%sthread_ppSrv \033[m %d \n", color , thread_ppSrv);
	printf("%sthread_prSrv \033[m %d \n", color , thread_prSrv);
	printf("%smax_line \033[m %d \n" , color , max_line );
	printf("%stime_helper_c \033[m %d \n" , color , time_helper_c );
	printf("%stime_helper_s \033[m %d \n" , color , time_helper_s );
	printf("%stime_helper_pp \033[m %d \n" , color , time_helper_pp );
	printf("%stimeout_init_msg \033[m %d \n" , color , timeout_init_msg );
	printf("%stimeout_transaction \033[m %d \n" , color ,timeout_tr );
	printf("%stimeout_pp \033[m %d \n" , color ,timeout_pp );
	printf("%stimeout_pr \033[m %d \n" , color ,timeout_pr );
	printf("%stimeout_bully \033[m %d \n" , color ,timeout_bully );
	printf("%smax cap modificabili \033[m %d \n" , color ,tr_max_cap );
	printf("%stime wait replica \033[m %d \n" , color ,wait_rep );
	printf("%stime wait primary \033[m %d \n" , color ,wait_pri );
	printf("%stime ciclo di bully fisso \033[m %d \n" , color, ciclo_bully_static );
	printf("%stime ciclo di bully random \033[m %d \n" , color ,ciclo_bully_random );
	printf("%snome server \033[m %s \n" , color , name_server );
	printf("%sid_server \033[m %d \n" , color , id_server );
	printf("%spath filesystem \033[m %s \n" , color , fs_path );
	printf("%spath temp\033[m %s \n" , color , tmp_path );
	printf("%sinterfaccia di rete\033[m %s \n" , color , iterface_network );
	printf("%sIP:\033[m %s \n" , color , my_ip );
	printf("\n\n\n");


	risultato_string = select_db_am(db_dom, sql_com, stringa_errore, &rc);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	printf("Lista Domini-Server :\n");
	printf("%s\n\n" , risultato_string);
	

	rc = close_db(db_conf, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	rc = close_db(db_server, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(stringa_errore);
	free(risultato_string);
	free(db_conf);
	free(db_dom);
	free(db_server);
	
	
	lista_File = allocForLocking( thread_helper_c  , lista_File , list_mutex ); //alloco la lista file e il mutex per l'accesso alla suddetta lista
}



// crea la lista dei server, ritorna: la struttura contenente la lista server 
// Parametri -> descrittore DB , esito operazione , nome tabella , condizione query db , eventual stringa di errore
Server * create_list_server(DB *db , int *rc , char* tabella , char *cond, char *stringa_errore )
{ 
	Server *list_server; 

	len_list_server = count_tb(db , tabella , cond , stringa_errore);

	if( len_list_server == -1 )
	{
    fprintf(stderr, "%s\n", stringa_errore);
		*rc = 0;
    return NULL;
  }

	list_server = calloc( len_list_server , sizeof(Server));
	
	char string_query[_DIM_QUERY_COND];
	memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
	snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "select name , ip , port from %s %s" , tabella , cond);
	
	ResultQuery *struct_list;
	
	struct_list = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );
	struct_list->object = (void*) list_server;
	struct_list->count = len_list_server;
	struct_list->l_row = 0;

	if(!select_on_bufferServer(db , string_query , struct_list , stringa_errore ) )
	{
    fprintf(stderr, "%s\n", stringa_errore);
		*rc = 0;
    return NULL;
  }

	free(struct_list);

	return list_server;

}



// crea la stringa formattata domini - server primary, ritorna: la stringa formattata domini - server primary
// Parametri -> descrittore DB , esito operazione , nome tabella , condizione query db , eventuale stringa di errore
char *create_string_dom_serv(DB *db , int *rc , char* tabella , char *cond, char *stringa_errore )
{
	char string_query[_DIM_QUERY_COND];
	memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
	snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "select dominio , server from %s %s" , tabella , cond);
	
	char* list;
	 list = select_dom_serv(db , string_query , stringa_errore , rc);
		
	return list ;
}



// crea la vista dei server per un dato dominio escluso il server corrente, ritorna: la stringa formattata domini - server primary
// Parametri -> nome dominio , path del db , lunghezza struttura , eventuale stringa di errore
Server *vistaServer(char *dom , char *path_db ,	int *len )
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
	snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "select server , ip , port from Dom  where dominio = '%s' AND server <> '%s' ", dom , 					   name_server);

	memset( cond , '\0' , _DIM_COND );
	snprintf(cond , _DIM_COND*sizeof(char) ," where dominio = '%s' AND server <> '%s' ", dom , name_server);
	
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



// verifica se il server è primary della risorsa, ritorna: 1 se il server è primary, 0 se non è primary, -1 se c'e' un errore
// Parametri -> pathrisorsa , dimensione path risorsa, nome server
int is_primary( char *path , int dim  , char *nomeServ)
{
	int i ;
	char str_dom[_DIM_DOM+1];
	short int b = 0;
	memset( str_dom , '\0' , _DIM_DOM+1);
	
	for( i = 1 ; i < dim; ++i)
	{
		if(path[i] != '/')
			strncat(str_dom , &path[i] , 1);
		else
			i = dim;
	}
	
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore;
	int rc;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	
	rc = open_db_ro( db_dom ,  PATH_DB_TMPDOM , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  char str_cond[_DIM_COND];
  memset( str_cond , '\0' , _DIM_COND);
  
  snprintf(str_cond , _DIM_COND , " where dominio = '%s' and type_server = 1 and server = '%s' " , str_dom , nomeServ);

  b = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
  
  if( b == -1)
  {
  	printf("%s \n" , stringa_errore );
  }
  
  rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  free(db_dom);
  free(stringa_errore);
  return b ;
}

// verifica se il server è primary o replica della risorsa, ritorna: 1 se il server lo è, 0 se non lo è, -1 se c'e' un errore
// Parametri -> pathrisorsa , dimensione path risorsa, nome server
int is_Primary_OR_Replica( char *path , int dim  , char *nomeServ)
{
	int i ;
	char str_dom[_DIM_DOM+1];
	short int b = 0;
	memset( str_dom , '\0' , _DIM_DOM+1);
	
	for( i = 1 ; i < dim; ++i)
	{
		if(path[i] != '/')
			strncat(str_dom , &path[i] , 1);
		else
			i = dim;
	}
	
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore;
	int rc;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	
	rc = open_db_ro( db_dom ,  PATH_DB_TMPDOM , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  char str_cond[_DIM_COND];
  memset( str_cond , '\0' , _DIM_COND);
  
  snprintf(str_cond , _DIM_COND , " where dominio = '%s' and server = '%s' " , str_dom , nomeServ);

  b = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
  
  if( b == -1)
  {
  	printf("%s \n" , stringa_errore );
  }
  
  rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  free(db_dom);
  free(stringa_errore);
  return b ;
}

// verifica se il server è replica della risorsa, ritorna: 1 se il server è replica, 0 se non è replica, -1 se c'e' un errore
// Parametri -> pathrisorsa , dimensione path risorsa, nome server
int is_replica( char *path , int dim  , char *nomeServ)
{
	int i ;
	char str_dom[_DIM_DOM+1];
	short int b = 0;
	memset( str_dom , '\0' , _DIM_DOM+1);
	
	for( i = 1 ; i < dim; ++i)
	{
		if(path[i] != '/')
			strncat(str_dom , &path[i] , 1);
		else
			i = dim;
	}
	
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore;
	int rc;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	
	rc = open_db_ro( db_dom ,  PATH_DB_TMPDOM , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  char str_cond[_DIM_COND];
  memset( str_cond , '\0' , _DIM_COND);
  
  snprintf(str_cond , _DIM_COND , " where dominio = '%s' and type_server = 2 and server = '%s' " , str_dom , nomeServ);

  b = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
  
  if( b == -1)
  {
  	printf("%s \n" , stringa_errore );
  }
  
  rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  free(db_dom);
  free(stringa_errore);
  return b ;
}



// verifica se esiste la risorsa, ritorna: 1 se esiste la risorsa, 0 se non esiste, -1 se c'e' un errore
// Parametri -> pathrisorsa , dimensione path risorsa, 
int is_dominio (char *path , int dim  )
{
	int i ;
	char str_dom[_DIM_DOM+1];
	short int b = 0;
	memset( str_dom , '\0' , _DIM_DOM+1);
	
	for( i = 1 ; i < dim; ++i)
	{
		if(path[i] != '/')
			strncat(str_dom , &path[i] , 1);
		else
			i = dim;
	}
	
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore;
	int rc;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	
	rc = open_db_ro( db_dom ,  PATH_DB_TMPDOM , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  char str_cond[_DIM_COND];
  memset( str_cond , '\0' , _DIM_COND);
  
  snprintf(str_cond , _DIM_COND , " where dominio = '%s' and type_server = 1 " , str_dom );

  b = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
  
  if( b == -1)
  {
  	printf("%s \n" , stringa_errore );
  }
  
  rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  free(db_dom);
  free(stringa_errore);
  return b ;
}



// cerca il primary della risorsa, ritorna: 
// Parametri -> pathrisorsa , dimensione path risorsa, nome server
int whois_primary( char *path , int dim  , char *server , char *ip , int* port)
{
	int i ;
	char str_dom[_DIM_DOM+1];
	short int b = 0;
	memset( str_dom , '\0' , _DIM_DOM+1);
	
	for( i = 1 ; i < dim; ++i)
	{
		if(path[i] != '/')
			strncat(str_dom , &path[i] , 1);
		else
			i = dim;
	}
	
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore;
	int rc;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	
	rc = open_db_ro( db_dom ,  PATH_DB_TMPDOM , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  if( info_primary_dominio(db_dom, str_dom , server , ip , port) )
  	b = 1;
  else
  	b= 0;
  
  rc = close_db(db_dom, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  free(db_dom);
  return b ;
}



//  crea la lista dei domini, ritorna: la lista domini oppure NULL se c'e' un errore
//  Parametri -> Desrcittore del db , lunghezza struttura, nome tabella , condizioni della query , stringa di errore
Dominio * create_list_domini(DB *db , int *len_list_domini , char *cond, char *stringa_errore )
{ 
	Dominio *list; 
	
	*len_list_domini = count_tb(db , "Dom" , cond , stringa_errore);

	list = (Dominio *)calloc( *len_list_domini , sizeof(Dominio));
	
	char string_query[_DIM_QUERY_COND];
	memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
	snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "select DISTINCT dominio from Dom %s" ,  cond);

	ResultQuery *struct_list;
	
	struct_list = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );
	struct_list->object = (void*) list;
	struct_list->count = *len_list_domini;
	struct_list->l_row = 0;

	if(!select_on_bufferDomini(db , string_query , struct_list , stringa_errore ) )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    return NULL;
  }

	free(struct_list);
	return list;

}



// restituisce l'id incrementato
int get_count( )  
{
	int r ;
	
	lock_Two_rooms(sem_count);
		if( id_count < _MAX_COUNT)
		{
			++id_count;
		}
		else
		{
			id_count = 1;
		}
		r = id_count;
	unlock_Two_rooms(sem_count);
	
	return r ;
}



// restituisce un id transaction valido
void get_idTr( int *id_p , int *id_c)
{

	*id_p = id_server ;

	*id_c = get_count();

} 



// il primary verifica se i server replica di un dominio sono attivi, restituisce: 1 se tutti i server sono attivi, 
// 0 se almeno un server non è attivo modificando la lista dei domini-server
// Parametri: lista server della vista , lunghezza , posizione del DB
int control_replic(Server * list , int *pt_len , char *path_db , char * nome_dominio)
{
	int i ;
	int len = *pt_len ;
	int index_down[len] ;
	int len_down = 0 ;
	pthread_t threads[len];
	Th_CtrlRep ctrl_rep[len];
	char str_cond[_DIM_COND];

	for( i = 0 ; i < len ; ++i)
	{		
		index_down[i] = -1;
	}
	
	
	for( i = 0 ; i < len ; ++i)
	{
		ctrl_rep[i].timeout = timeout_pp;
		ctrl_rep[i].tentativi = _TENTATIVI_PP;
		memset(ctrl_rep[i].remote_ip , '\0' , _DIM_IP);
		strcpy(ctrl_rep[i].remote_ip , list[i].ip);
		ctrl_rep[i].port = (int) *port_server_pp;
		ctrl_rep[i].state = -1;
	}
	
	for(i=0;i < len;i++)
	{
		int rc ;
  	rc = pthread_create (&threads[i], NULL, func_helper_ctrlRep, (void *)&(ctrl_rep[i])  );
  	if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
  	}
	}
	
	for(i=0;i < len ;i++)
	{	
  	pthread_join(threads[i], NULL );
	}		

	for( i = 0 ; i < len ; ++i)
	{			
		if( ctrl_rep[i].state == 0 )
		{			
			++len_down ;
			index_down[len_down-1] = i ;
		}
	}

	if( len_down > 0)
	{
		DB *db_dom; // descrittore del DB Domini
		char string_query[_DIM_QUERY_COND];
		char *stringa_errore;
		
		db_dom = ( DB *) malloc (sizeof(DB) ) ;
		stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char) );
		 
		if( !open_db_rw( db_dom , path_db , stringa_errore))
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  exit(1);
		}

		for( i = 0 ; i < len_down ; ++i)
		{	
			int count ;
			int index = index_down[i];
			memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
			snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from Dom  where server = '%s' AND dominio ='%s'" ,list[index].name , 			             nome_dominio);
	
			lock_WR_Writers(sem_vista);
			
				if( generic_op_db(db_dom , string_query , stringa_errore) == -1)
				{
					printf("%s \n" , stringa_errore );
				}
				
  			memset( str_cond , '\0' , _DIM_COND);
  			snprintf(str_cond , _DIM_COND , " where server = '%s' " , list[index].name );

  			count = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
				 
			unlock_WR_Writers(sem_vista);
			
			if( count == 0) // se un server nella lista domini non compare + viene cancellato anche dalla lista server del server
			{
				DB *db_server; // descrittore del DB Domini
				db_server = ( DB *) malloc (sizeof(DB) ) ;
				
				if( !open_db_rw( db_server , path_db , stringa_errore))
				{
					fprintf(stderr, "%s\n", stringa_errore);
					exit(1);
				}
				
				memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
				snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "delete  from ListServer  where name = '%s'" ,list[index].name );
  			
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
		
		if(len_down > 0)
		{	
			PR_Message *msg;
			msg  = init_PR_Message( 0 , "\0" , "\0" , 0 , 0 , "\0" );
			
			lock_WR_Reader(sem_list_serv);
			for( i = 0 ; i < len_down ; ++i)
			{		
				int j ;
				for( j= 0 ; j < len_list_server ; ++j)
				{	
					int w;
					int enter = 1;
					for( w = 0; w < len_down ; ++w)
					{
						if(strcmp(list_server[j].name , list[index_down[w]].name) == 0)
							enter = 0;
					}	
					
					if(enter)
					{			
						int sockfd;
						if( ( sockfd = connect_ServerServer(list_server[j].ip , *port_server_pr , 2) ) > 0 ) 
						{
							set_PR_Message(msg ,_PR_DEL_SERVDOM , list[index_down[i]].name ,  nome_dominio , 0 , 0 , "/0" );
							set_timeout(sockfd  , timeout_pr  , 0  , timeout_pr  , 0 );	
						
							if(send_PR_Message(sockfd , msg ))
							{
								PR_Message *tmp_msg;
								tmp_msg= (PR_Message *) calloc (  1 , sizeof(PR_Message)  );
							
								recv_PR_Message(sockfd , tmp_msg);
								free(tmp_msg);
							}
							close(sockfd);
						}
					}
				}
			}
			unlock_WR_Reader(sem_list_serv);
			free(msg);		
		}
  	return 0;
	}
	return 1;
}



//funzione helper di control_replic
void *func_helper_ctrlRep(void *l)
{

	Th_CtrlRep *c_rep;
	
	c_rep = (Th_CtrlRep *)l;

	if( isAlive_pp(listServ_pp, sem_listServ_pp, c_rep->timeout ,c_rep->tentativi ,my_ip ,c_rep->remote_ip ,c_rep->port) <= 0 )
	{			
		sleep(wait_rep);

		if( isAlive_pp(listServ_pp, sem_listServ_pp, c_rep->timeout ,c_rep->tentativi ,my_ip ,c_rep->remote_ip ,c_rep->port) <= 0  )
		{
			c_rep->state = 0;
		}
	}

	if(c_rep->state != 0)
		c_rep->state = 1;

	pthread_exit(NULL);
}

