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
  
  time_helper = select_conf_param(db_conf , "time_helper" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  time_helper_pp = time_helper;
	time_helper_c = time_helper;
	time_helper_s = time_helper * 2 ;
	
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

	list_server = create_list_server(db_server ,  &rc , "ListServer" , cond , stringa_errore );
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  memset( cond , '\0' , _DIM_COND );
	strcpy(cond ," where type_server = 1 ");
	
  string_dom_serv = create_string_dom_serv(db_dom ,  &rc , "Dom" , cond , stringa_errore );
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
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
	for( g = 0 ; g < 5 ; g++)
		printf( "%d - name = %s  , ip = %s , port = %d \n" , g , list_server[g].name , list_server[g].ip , list_server[g].port );

	printf("\nParametri caricati nel sistema:\n");
	printf("%s<<<<< --- COLORE --- >>>>>  \033[m \n" , color );
	printf("%sbacklog_client  \033[m %d \n", color , backlog_client );
	printf("%sbacklog_server  \033[m %d \n", color , backlog_server );
	printf("%sport_client \033[m %d \n", color , ntohs(*port_client) );
	printf("%sport_server \033[m %d \n", color , ntohs(*port_server) );
	printf("%sport_client ping-pong \033[m %d \n", color , ntohs(*port_client_pp) );
	printf("%sport_server ping-pong\033[m %d \n", color , ntohs(*port_server_pp) );
	printf("%sthread_helper_client \033[m %d \n", color , thread_helper_c );
	printf("%sthread_helper_server \033[m %d \n", color , thread_helper_s);
	printf("%sthread_ppCl \033[m %d \n", color , thread_ppCl);
	printf("%sthread_ppSrv \033[m %d \n", color , thread_ppSrv);
	printf("%smax_line \033[m %d \n" , color , max_line );
	printf("%stime_helper_c \033[m %d \n" , color , time_helper_c );
	printf("%stime_helper_s \033[m %d \n" , color , time_helper_s );
	printf("%stime_helper_pp \033[m %d \n" , color , time_helper_pp );
	printf("%stimeout_init_msg \033[m %d \n" , color , timeout_init_msg );
	printf("%stimeout_transaction \033[m %d \n" , color ,timeout_tr );
	printf("%stimeout_pp \033[m %d \n" , color ,timeout_pp );
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



// crea la vista dei server per un dato dominio, ritorna: la stringa formattata domini - server primary
// Parametri -> nome dominio , path del db , lunghezza struttura , eventuale stringa di errore
Server *vistaServer(char *dom , char *path_db ,	int *len , char *stringa_errore )
{
	DB *db_dom; // descrittore del DB Domini
	char string_query[_DIM_QUERY_COND];
	char cond[_DIM_COND];
	Server *list; 
	ResultQuery *struct_list;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	memset(stringa_errore , '\0', _MAX_STRING_ERROR *sizeof(char) );
	
	
	 
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
				return NULL;
			}

			list = calloc( *len , sizeof(Server));
	

			struct_list->object = (void*) list;
			struct_list->count = *len;
			struct_list->l_row = 0;

			if(!select_on_bufferServer(db_dom , string_query , struct_list , stringa_errore ) )
			{
				fprintf(stderr, "%s\n", stringa_errore);
				*len = 0;
				return NULL;
			}
	unlock_WR_Reader(sem_vista);
	
	free(struct_list);

  if( !close_db(db_dom, stringa_errore)){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(db_dom);

	return list;
}



// verifica se il server è primary della risorsa, ritorna: 1 se il server non è primary, 0 se non è primary, -1 se c'e' un errore
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

