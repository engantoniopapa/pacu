#include "header.h"



// inizializzazione delle variabili di configurazione se si accede come client
void conf_client()
{
	int rc;
	char *stringa_errore;
	DB *db_conf; // descrittore del DB Conf

	db_conf = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	port_client_pp =  ( int *) calloc (1,sizeof(int )); // porta per la comunicazione client-server relativa allo scambio ping-pong
	
	rc = open_db_ro( db_conf , PATH_DB_CONF , stringa_errore);
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
  
	timeout_boot = select_conf_param(db_conf , "timeout_boot" , stringa_errore , &rc); 
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
	
	tentativi_boot = select_conf_param(db_conf , "tentativi_boot" , stringa_errore , &rc); 
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
  
  tr_max_cap = select_conf_param(db_conf , "max_cap" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	in_path = select_conf_charparam(db_conf , "in_path" , stringa_errore , &rc); 
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

	select_conf_vers(db_conf , &version_dom , &version_serv , stringa_errore);
	
	iterface_network = select_conf_charparam(db_conf , "interf_network" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  get_myip(iterface_network , my_ip , _DIM_IP);

	rc = close_db(db_conf, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(stringa_errore);
	free(db_conf);
}



// inizializzazione delle variabili di configurazione se si accede come amministratore
void conf_am()
{
	int rc;
	char *stringa_errore;
	
	char *sql_com	 = "select * from ListServer";
	char *risultato_string;
	DB *db_conf; // descrittore del DB Conf
	DB *db_server; // descrittore del DB Primary

	db_conf = ( DB *) malloc (sizeof(DB) ) ;
	db_server = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	port_client_pp =  ( int *) calloc (1,sizeof(int )); // porta per la comunicazione client-server relativa allo scambio ping-pong
	
	rc = open_db_ro( db_conf , PATH_DB_CONF , stringa_errore);
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
	
	*port_client_pp = select_conf_param(db_conf , "port_cl_pp" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	timeout_am = select_conf_param(db_conf , "timeout_am" , stringa_errore , &rc); 
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

	max_line = select_conf_param(db_conf , "max_line" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	tentativi_boot = select_conf_param(db_conf , "tentativi_boot" , stringa_errore , &rc); 
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
  
  in_path = select_conf_charparam(db_conf , "in_path" , stringa_errore , &rc); 
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

	select_conf_vers(db_conf , &version_dom , &version_serv , stringa_errore);

	
	char ip_boot[_DIM_IPV4];
	uint16_t port_boot;
	rc = select_boot_server(db_conf , ip_boot ,  &port_boot , 0);

	printf("Parametri caricati nel sistema:\n");
	printf("%sServer Boot ->  IP:\033[m %s %s Port:\033[m %d \n" ,color , ip_boot , color , port_boot );
	printf("%stimeout_am \033[m %d \n" , color , timeout_am );
	printf("%stimeout_init_msg\033[m %d \n" , color , timeout_init_msg );
	printf("%stimeout_tr\033[m %d \n" , color , timeout_tr );
	printf("%stimeout_pp\033[m %d \n" , color , timeout_pp );
	printf("%smax_line \033[m %d \n" , color , max_line );
	printf("%smax cap modificabili in una transazione \033[m %d \n" , color , tr_max_cap);
	printf("%stentativi_boot\033[m %d \n" , color , tentativi_boot );
	printf("%sporta Ping-Pong \033[m %d \n" , color , *port_client_pp);
	printf("%spath download\033[m %s \n" , color , in_path );
	printf("%spath temp\033[m %s \n" , color , tmp_path );
	printf("%sinterfaccia di rete\033[m %s \n" , color , iterface_network );
	printf("%sIP:\033[m %s \n" , color , my_ip );
	printf("Parametri Version caricati nel sistema:\n");
	printf("%sversione domini \033[m %f \n", color , version_dom );
	printf("%sversione server \033[m %f \n", color , version_serv );
	printf("\n\n");

	risultato_string = select_tb(db_server, sql_com, stringa_errore, &rc);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	printf("Lista Server Primary:\n");
	printf("%s\n\n" , risultato_string);
	free(risultato_string);

	rc = close_db(db_conf, stringa_errore);
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
	free(db_conf);
	free(db_server);

}



//stampa la configurazione del sistema
void print_conf()
{
	int rc;
	char *stringa_errore;

	char *sql_com	 = "select * from ListServer";
	char *risultato_string;
	DB *db_conf; // descrittore del DB Conf
	DB *db_server; // descrittore del DB Primary

	db_conf = ( DB *) malloc (sizeof(DB) ) ;
	db_server = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	port_client_pp =  ( int *) calloc (1,sizeof(int )); // porta per la comunicazione client-server relativa allo scambio ping-pong

	rc = open_db_ro( db_conf , PATH_DB_CONF , stringa_errore);
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

	*port_client_pp = select_conf_param(db_conf , "port_cl_pp" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
	timeout_boot = select_conf_param(db_conf , "timeout_boot" , stringa_errore , &rc); 
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

	max_line = select_conf_param(db_conf , "max_line" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	tentativi_boot = select_conf_param(db_conf , "tentativi_boot" , stringa_errore , &rc); 
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

  tr_max_cap = select_conf_param(db_conf , "max_cap" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	in_path = select_conf_charparam(db_conf , "in_path" , stringa_errore , &rc); 
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

	select_conf_vers(db_conf , &version_dom , &version_serv , stringa_errore);

	char ip_boot[_DIM_IPV4];
	uint16_t port_boot;
	rc = select_boot_server(db_conf , ip_boot ,  &port_boot , 0);
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	printf("Parametri del sistema:\n");
	printf("%stimeout_boot \033[m %d \n" , color, timeout_boot );
	printf("%smax_line \033[m %d \n"  , color, max_line );
	printf("%stentativi_boot \033[m %d \n" , color , tentativi_boot);
	printf("%stimeout_init_msg \033[m %d \n" , color , timeout_init_msg);
	printf("%stimeout_domlist \033[m %d \n" , color , timeout_init_msg);
	printf("%stimeout_tr \033[m %d \n" , color , timeout_tr);
	printf("%stimeout_pp \033[m %d \n" , color , timeout_pp);
	printf("%smax cap modificabili in una transazione \033[m %d \n" , color , tr_max_cap);
	printf("%sporta Ping-Pong \033[m %d \n" , color , *port_client_pp);
	printf("%sServer Boot ->  IP:\033[m %s %s Port:\033[m %d \n" ,color , ip_boot , color , port_boot );
	printf("%spath download\033[m %s \n" , color , in_path );
	printf("%spath temp\033[m %s \n" , color , tmp_path );
	printf("%sinterfaccia di rete\033[m %s \n" , color , iterface_network );
	printf("%sIP:\033[m %s \n" , color , my_ip );
	printf("\n");
	printf("Parametri Version caricati nel sistema:\n");
	printf("%sversione domini \033[m %f \n", color , version_dom );
	printf("%sversione server \033[m %f \n", color , version_serv );
	printf("\n\n");
	risultato_string = select_tb(db_server, sql_com, stringa_errore, &rc);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	printf("Lista Server Primary:\n");
	printf("%s\n\n" , risultato_string);
	free(risultato_string);


	rc = close_db(db_conf, stringa_errore);
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
	free(db_conf);
	free(db_server);
	free(tmp_path);
	free(in_path);
	free(port_client_pp);
	free(iterface_network);
	
	while(getchar() != '\n'); // per svuotare l'stdin
	printf("Premi un tasto per continuare ...... \n");
	getc(stdin);

}



//aggiorna la version del client, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri->la versione primary e domini con cui aggiornare la versione client
int update_version(double primary , double domini)
{
	short int b;
	
	if(primary == 0 && domini == 0 )
	{
		b = 1;
	}
	else
	{
		DB *db_conf; // descrittore del DB Conf
		char *stringa_errore;
		int rc;

		int i =0 ;
		int count = 0;
		short int virgola =  0 ;
		char *string_query1 = "Update Version set";
		char *string_query2 = " where id = 0 ";
		char *query;
		char str_primary[50];
		char str_dom [50];
	
		db_conf = ( DB *) malloc (sizeof(DB) ) ;
		stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
		if(primary != 0)
		{
			sprintf(str_primary, " v_primary = %f",  primary );
			virgola = 1;
			version_serv =  primary;
		}

		if(domini != 0)
		{

			if(virgola)
				sprintf(str_dom , " , v_dom = %f",  domini );
			else
				sprintf(str_dom , " v_dom = %f",  domini );

			version_dom = domini;
		}
		
		count = strlen(string_query1)+strlen(string_query2)+strlen(str_primary)+strlen(str_dom)+1;
		query = calloc( count , sizeof(char) );
		strcpy(query, string_query1);

		if(primary != 0)
			strcat(query, str_primary);
		if(domini != 0)
			strcat(query, str_dom);

		strcat(query, string_query2);

		rc = open_db_rw( db_conf , PATH_DB_CONF , stringa_errore);
		if( !rc )
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  return 0;
		}

		generic_op_db(db_conf , query , stringa_errore);
		if( !rc )
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  return 0;
		}

		rc = close_db(db_conf, stringa_errore);
  	if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    return 0;
 		}

		free(query);
		free(db_conf);
		free(stringa_errore);
		b = 1;
	}

	return b ;
}


//  crea la lista dei server primary del sistema contattabili dal client, ritorna: la lista server oppure NULL se c'e' un errore
//  Parametri -> Desrcittore del db , stato dell'operazione al termine della funzione, nome tabella , condizioni della query , stringa di errore
Server * create_list_server(DB *db , int *rc , char* tabella , char *cond, char *stringa_errore )
{ 
	Server *list; 

	len_list_server = count_tb(db , tabella , cond , stringa_errore);

	if( len_list_server == -1 )
	{
    fprintf(stderr, "%s\n", stringa_errore);
		*rc = 0;
    return NULL;
  }

	list = calloc( len_list_server , sizeof(Server));
	
	char string_query[_DIM_QUERY_COND];
	memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
	snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "select name , ip , port from %s %s" , tabella , cond);

	ResultQuery *struct_list;
	
	struct_list = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );
	struct_list->object = (void*) list;
	struct_list->count = len_list_server;
	struct_list->l_row = 0;

	if(!select_on_bufferServer(db , string_query , struct_list , stringa_errore ) )
	{
    fprintf(stderr, "%s\n", stringa_errore);
		*rc = 0;
    return NULL;
  }

	free(struct_list);
	*rc = 1;
	return list;

}



//  dealloca la struttura
void free_list_server()
{
	if( len_list_server > 0 )
		free(list_server);
}



//  crea la lista dei domini del sistema contattabili dal client, ritorna: la lista domini oppure NULL se c'e' un errore
//  Parametri -> Desrcittore del db , stato dell'operazione al termine della funzione, nome tabella , condizioni della query , stringa di errore
Dominio * create_list_domini(DB *db , int *rc , char* tabella , char *cond, char *stringa_errore )
{ 
	Dominio *list; 
	
	len_list_domini = count_tb(db , tabella , cond , stringa_errore);

	if( len_list_domini == -1 )
	{
    fprintf(stderr, "%s\n", stringa_errore);
		*rc = 0;
    return NULL;
  }

	list = calloc( len_list_domini , sizeof(Dominio));
	
	char string_query[_DIM_QUERY_COND];
	memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
	snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "select dominio from %s %s" , tabella , cond);

	ResultQuery *struct_list;
	
	struct_list = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );
	struct_list->object = (void*) list;
	struct_list->count = len_list_domini;
	struct_list->l_row = 0;

	if(!select_on_bufferDomini(db , string_query , struct_list , stringa_errore ) )
	{
    fprintf(stderr, "%s\n", stringa_errore);
		*rc = 0;
    return NULL;
  }

	free(struct_list);
	*rc = 1;
	return list;

}



//  dealloca la struttura
void free_list_domini()
{
	if( len_list_domini > 0 )
		free(list_domini);
}



//  analizza sintatticamente la lista formattata, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//  Parametri -> lista
int anal_sint_list(char *list)
{

	int i ;
	int count = 0;

	if( strlen(list) == 0 )
		return 0;

	for(i = 0 ; i < strlen(list) ; i++ ) 	
	{
		if(list[i] == '^')
			count++;
	}	

	int array_sint[count];
	
	for(i = 0 ; i < count ;i++)
	{
		array_sint[i] = 0 ;
	}
	
	count = 0 ;				
	for( i = 0 ; i < strlen(list) ; i++)
	{
		
		if(list[i] == '^')
			count++;
		
		if(list[i] == '|')
			array_sint[count]++ ;
	}
	for( i = 1 ; i < count ; i++)
	{
		if( array_sint[0] != array_sint[i])
		{
			return 0;
		} 
	}
	
	return 1 ;
}


// aggiorna il dominio con il nuovo primary
int update_dom_primary( char *dom , char *server  , char* ip , int port , char *path_serv , char *path_dom)
{
	int i ;
	short int b = 0;
	char *stringa_errore;
	int rc;
	DB *db_dom; // descrittore del DB Domini
	DB *db_serv; // descrittore del DB Domini
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	db_serv = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	
	rc = open_db_rw( db_dom ,  path_dom , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  
  rc = open_db_rw( db_serv ,  path_serv , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
  char query[500];
  char str_cond[_DIM_COND];
  memset( str_cond , '\0' , _DIM_COND);
  
  snprintf(str_cond , _DIM_COND , " where name = '%s' " ,  server);
	
  b = count_tb(db_serv , "ListServer" , str_cond , stringa_errore) ;
  
	
  
  if(b == 0)
  {	
  	
  	memset( query , '\0' , 500);
  	memset( stringa_errore , '\0' , _MAX_STRING_ERROR);
  	
  	snprintf(query , 500 , " insert into ListServer values( '%s' , '%s' , %d)  " ,  server ,ip ,port);
  	
  	if( insert_db(db_serv , query , stringa_errore) )
  	{
  			b = 1;
  	}
  	else
  	{
  		printf("%s \n" , stringa_errore );
  		b = 0;
  	}	
  }
  
  if( b == -1)
  {
  	printf("%s \n" , stringa_errore );
  	b = 0;
  }
  
  if( b == 1 )
  {
  	memset( str_cond , '\0' , _DIM_COND);
  
  	snprintf(str_cond , _DIM_COND , " where dominio = '%s' " ,  dom);

		int c ;
  	c = count_tb(db_dom , "Dom" , str_cond , stringa_errore) ;
  	
  	
  	if( c == 1 )
  	{
  		memset( query , '\0' , 500);
  		memset( stringa_errore , '\0' , _MAX_STRING_ERROR);
  	
  		snprintf(query , 500 , " Update Dom set server = '%s' where dominio = '%s'  " ,  server , dom);
  		
  		if( generic_op_db(db_dom , query , stringa_errore) )
  		{
  			b = 1; 
  		}
  		
  		else
  		{
  			printf("%s \n" , stringa_errore );
  			b = 0;
  		}		
  	}
  	 
  	if(c == 0)
  	{
  		memset( query , '\0' , 500);
  		memset( stringa_errore , '\0' , _MAX_STRING_ERROR);
  	
  		snprintf(query , 500 , " insert into Dom values( '%s' , '%s' )  " ,  server , dom);
  	
  		if( insert_db(db_dom , query , stringa_errore) )
  		{
  			b = 1;
  		}
  		
  		else
  		{
  			printf("%s \n" , stringa_errore );
  			b = 0;
  		}		
  	}	
  	
  	if( c == -1)
		{
				printf("%s \n" , stringa_errore );
				b = 0;
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
  
  free(db_dom);
  free(db_serv);
  free(stringa_errore);
  return b ;
}



