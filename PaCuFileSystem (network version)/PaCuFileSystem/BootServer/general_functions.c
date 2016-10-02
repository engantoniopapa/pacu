#include "header.h"

// inizializzazione delle variabili di configurazione
void get_conf()
{
	int rc;
	char *stringa_errore;
	char *sql_com	 = "select * from ListServer";
	char *risultato_string;
	DB *db_conf; // descrittore del DB Conf
	DB *db_server; // descrittore del DB Primary


	db_conf = ( DB *) malloc (sizeof(DB) ) ;
	db_server = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char));
	
	color = (char*) malloc ( sizeof(char) * 9 );
	strcpy( color , "\e[1;34m" );

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



	backlog_client = select_conf_param(db_conf , "backlog" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

  *port_client = htons( select_conf_param(db_conf , "port_client" , stringa_errore , &rc) );
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

  *port_am = htons( select_conf_param(db_conf , "port_am" , stringa_errore , &rc) );
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

  thread_helper = select_conf_param(db_conf , "thread_helper" , stringa_errore , &rc); 
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

	timeout_ping = select_conf_param(db_conf , "timeout_ping" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }	
	
	timeout_pong = select_conf_param(db_conf , "timeout_pong" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }	
	
	timeout_error = select_conf_param(db_conf , "timeout_error" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
		
	timeout_req_list = select_conf_param(db_conf , "timeout_req_list" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	timeout_send_servlist = select_conf_param(db_conf , "timeout_send_servlist" , stringa_errore , &rc); 
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

	max_line = select_conf_param(db_conf , "max_line" , stringa_errore , &rc); 
	if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	select_conf_vers(db_conf , &version_dom , &version_serv , stringa_errore);

	printf("\nParametri caricati nel sistema:\n");
	printf("%s<<<<< --- COLORE --- >>>>>  \033[m \n" , color );
	printf("%sbacklog_client  \033[m %d \n", color , backlog_client );
	printf("%sport_client \033[m %d \n", color , ntohs(*port_client) );
	printf("%sport_am \033[m %d \n", color , ntohs(*port_am) );
	printf("%sthread_helper \033[m %d \n", color , thread_helper );
	printf("%stime_helper \033[m %d \n", color  , time_helper ); 
	printf("%stimeout_ping \033[m %d \n" , color , timeout_ping );
	printf("%stimeout_pong \033[m %d \n" , color , timeout_pong );
	printf("%stimeout_error \033[m %d \n" , color , timeout_error );
	printf("%stimeout_req_list \033[m %d \n" , color , timeout_req_list );
	printf("%stimeout_send_servlist \033[m %d \n" , color , timeout_send_servlist );
	printf("%stimeout_am \033[m %d \n" , color , timeout_am );
	printf("%smax_line \033[m %d \n" , color , max_line );
	printf("\n");
	printf("Parametri Version caricati nel sistema:\n");
	printf("%sversione domini \033[m %f \n", color , version_dom );
	printf("%sversione server \033[m %f \n", color , version_serv );
	printf("\n\n\n");
	


	risultato_string = select_db_am(db_server, sql_com, stringa_errore, &rc);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	printf("Lista Server Primary:\n");
	printf("%s\n\n" , risultato_string);
	free(risultato_string);

	list_primary = select_listserver(db_server, "select name, ip , port from ListServer" ,stringa_errore, &rc);
	if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	printf("Lista Server Formattata:\n");
	printf("%s\n\n" , list_primary);


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



//	crea la password in md5 (amministratore)
//	Parametri-> password hashata che viene restituita per riferimento, password originale, dimensione password originale
void create_password( char * hash , char* text , int dim)
{
	md5_state_t hash_state;
	md5_init(&hash_state);
  md5_append(&hash_state, (const md5_byte_t *)text, strlen(text));
	md5_finish(&hash_state, (md5_byte_t *) hash);
}



//	seleziona il DB su cui operare (Amministratore)
//	Parametri-> descrittore del DB, messaggio contenente la scelta del db, stringa che prende l'eventuale errore	
int scelta_db(DB *data_base , BootAM *buff , char* str_error)
{
	int rc = 0;
	memset(str_error , '\0' , _MAX_STRING_ERROR);	

	if( memcmp(buff->query , "Conf" , strlen("Conf") ) == 0 )
	{
		rc = open_db_rw(data_base, PATH_DB_CONF , str_error);
		return rc;
  }
	if( memcmp(buff->query , "Primary" , strlen("Primary") ) == 0)
	{
		rc = open_db_rw(data_base, PATH_DB_PRIMARY, str_error);
		return rc;
  }

	strcpy(str_error, "DB inesistente");	
	return rc;
}



//  libero la memoria allocata dall'applicazione 
void free_all()
{
	free(list_primary);
	free(port_client);
	free(port_am);
	free_semaphore(sem_accept);
	free_semaphore(sem_exit_disp);
}


