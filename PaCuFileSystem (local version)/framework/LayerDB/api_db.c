#include "api_db.h"


// funzione di connessione al DB in sola lettura ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri-> descrittore db sqlite3, un puntatore char con il nome del db, un puntatore char per "restituire" l'eventuale errore
int open_db_ro( DB* data_base , char* nome_db , char* string_error)
{

	int rc;
	
	rc = sqlite3_open_v2(nome_db, &(data_base->db), SQLITE_OPEN_READONLY, NULL);

  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Can't open database: ") ;
		strcat( string_temp ,sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);

    sqlite3_close(data_base->db);
    return 0;
  }
	return 1;
}



// funzione di connessione al DB in lettura&Scrittura ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri-> descrittore db sqlite3, un puntatore char con il nome del db, un puntatore char per "restituire" l'eventuale errore
int open_db_rw(  DB* data_base , char* nome_db , char* string_error)
{

	int rc;
	rc = sqlite3_open_v2(nome_db, &(data_base->db), SQLITE_OPEN_READWRITE, NULL);



  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Can't open database: ") ;
		strcat( string_temp , sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);

    sqlite3_close(data_base->db);
    return 0;
  }
	return 1;
}



// funzione di chiusura del DB; ritorna 0 se c'e' un errore, 1 se l'operazione va a buon fine 
// Parametri-> descrittore db sqlite3, un puntatore char per "restituire" l'eventuale errore
int close_db(DB* data_base , char* string_error)
{

	int rc;
	rc = sqlite3_close(data_base->db);

  if( rc!=SQLITE_OK )
	{	
	
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Can't close database: ") ;
		strcat( string_temp , sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);

   	return 0;
  }
	return 1;
}



// insert sul DB: ritorna 0 se c'e' un errore, 1 se l'operazione va a buon fine 
// Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" l'eventuale errore
int insert_db(DB* data_base , char* string_query , char* string_error)
{	
	int rc;
	char *temp_error;

	rc = sqlite3_exec(data_base->db, string_query , 0 , 0, &temp_error);
	
  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR);
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );

		return 0;
  }
	return 1;
}



// count(*) su una tabella: ritorna -1 se c'e' un errore, il numero di row se l'operazione va a buon fine 
// Parametri-> descrittore db sqlite3, un puntatore char con il nome della tabella, un puntatore char per le eventuali condition, un puntatore char per "restituire" l'eventuale errore
int count_tb(DB* data_base , char* string_tb , char* condition , char* string_error)
{	
	int l;
	int rc;
	sqlite3_stmt* result;


	char string_query[300];
	memset( string_query , '\0' , 300*sizeof(char) );
	snprintf(string_query , 300*sizeof(char) , " select count(*) from %s %s " , string_tb , condition);

	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			l = sqlite3_column_int(result, 0);
		}
		sqlite3_finalize(result);
	}
	else
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Errore nella count: ") ;
		strcat( string_temp , sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);
		l = -1;
	}
	return l;
}



// delete sul DB: ritorna 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" l'eventuale errore
int delete_db(DB* data_base , char* string_query , char* string_error)
{
	int rc;
	char *temp_error;

	rc = sqlite3_exec(data_base->db, string_query , 0 , 0, &temp_error);
	
  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR);
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );

		return 0;
  }
	return 1;
}



// Operazione che non restituisce un risultato sul DB: ritorna 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" l'eventuale errore
int generic_op_db(DB* data_base , char* string_query , char* string_error)
{
	int rc;
	char *temp_error;

	rc = sqlite3_exec(data_base->db, string_query , 0 , 0, &temp_error);
	
  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR);
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );

		return 0;
  }
	return 1;
}



// (Serverboot)funzione usata per eleborare ogni risultato della funzione sqlite3_exec(), viene lanciata per ogni record avuto come risultato
// Parametri-> Struttura usata per tenere memoria del risultato, numeri di campi del record avuto come risultato, array di stringhe con i valori 
//						 dei campi del record, array di stringhe con i nomi dei campi del record
int callback_am(void* risultato, int argc, char **argv, char **azColName)
{
	
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery *) risultato;
	char *string_result ;
	string_result = (char *) temp_risultato->object;
				
	int i ;
	int length ;
	char *color = "\e[1;34m" ;
	
	if(temp_risultato->l_row == 0 )
	{
		length = 20; // partiamo da 20 per l'aggiunta dei colori
		for(i=0; i<argc ; i++)
		{
			length = length + 1 + strlen (azColName[i]) ;
		}
		++length;
		temp_risultato->count = temp_risultato->count  + length ;
		string_result = (char *) calloc (temp_risultato->count , sizeof(char) );

		strcat( string_result , color ); // colori campi tabella
		for(i=0; i<argc ; i++)
		{
			strcat( string_result ,"|" );
  		strcat( string_result , azColName[i]);
		}
		strcat( string_result ,"\033[m"); // colori campi tabella
		strcat( string_result ,"\n");

		++temp_risultato->l_row;

	}

	char *pt_temp = string_result;
	int temp_count = temp_risultato->count ;


	length = 0;
	for(i=0; i<argc ; i++){
	length = length + 1 + strlen (argv[i] ? argv[i] : "NULL") ;
	}
	length += 1;
	
	temp_risultato->count = temp_risultato->count  + length ;
	string_result = (char *) calloc ( temp_risultato->count , sizeof(char) );	

	memcpy( string_result , pt_temp , sizeof(char) * temp_count );	

  for(i=0; i<argc; i++)
	{
		strcat( string_result ,"|" );
    strcat( string_result , (argv[i] ? argv[i] : "NULL") );
  }
  strcat( string_result ,"\n");

	free(pt_temp);
	++temp_risultato->l_row;

  temp_risultato->object = (void *) string_result ;
	return 0;
}



// (Serverboot)select sul DB ritorna: il risultato della query in una stringa (puntatore a char) 
// Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" l'eventuale errore, varibile "booleana" 
//						 per segnalare l'errore
char* select_db_am(DB* data_base , char* string_query , char* string_error , int* no_error)
{
	int rc;
	ResultQuery *risultato;
	risultato = (ResultQuery * ) malloc ( sizeof(ResultQuery)); 
	char *temp_error;
	risultato->l_row = 0 ;
	risultato->count = 0; 
	risultato->object = NULL ;

	rc = sqlite3_exec(data_base->db, string_query , callback_am , (void*)risultato , &temp_error);

  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );


		*no_error = 0 ;
		return NULL;
  }
	
	char *temp_string;
	*no_error = 1;
	temp_string = (char*)risultato->object ;
	free(risultato);
	return temp_string;
}



// (Serverboot)funzione usata per eleborare ogni risultato della funzione sqlite3_exec(), viene lanciata per ogni record avuto come risultato
// Parametri-> Struttura usata per tenere memoria del risultato, numeri di campi del record avuto come risultato, array di stringhe con i valori 
//						 dei campi del record, array di stringhe con i nomi dei campi del record
int callback_listserver(void* risultato, int argc, char **argv, char **azColName)
{
	
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery *) risultato;
	char *string_result;
	string_result = (char *) temp_risultato->object;

	int temp_count = temp_risultato->count ;				

	int i ;
	int length = 0;
	char *pt_temp;

	if( temp_risultato->l_row > 0)
	pt_temp = string_result;

	for(i=0; i<argc ; i++){
	length = length + strlen (argv[i] ? argv[i] : "NULL") ;
	}
	length = length + 3 ;
	
	
	temp_risultato->count = temp_risultato->count  + length ;

	string_result = (char *) calloc ( (temp_risultato->count +1 ) , sizeof(char) );	
	
	if( temp_risultato->l_row > 0)
	memcpy( string_result , pt_temp , sizeof(char) * temp_count );	

  for(i=0; i<argc; i++)
	{
		if(i > 0)
		strcat( string_result ,"|" );

    strcat( string_result , (argv[i] ? argv[i] : "NULL") );
  }
  strcat( string_result ,"^");
	
	if( temp_risultato->l_row > 0)
	free(pt_temp);

	++temp_risultato->l_row;
  temp_risultato->object = (void *) string_result ;
	return 0;
}



// (Serverboot) select sul DB ritorna: il risultato della query in una stringa (puntatore a char) 
// Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" l'eventuale errore, varibile "booleana" 
//						 per segnalare l'errore
//NOTA rispetto alla select_db_am questa funzione è usata per avere una stringa formattata da inviare ai client che richiedono la lista server.
char* select_listserver(DB* data_base , char* string_query , char* string_error , int* no_error)
{
	int rc;
	ResultQuery *risultato;
	risultato = (ResultQuery * ) malloc ( sizeof(ResultQuery)); 
	risultato->l_row = 0 ;
	risultato->count = 0; 
	risultato->object = NULL ;
	char *temp_error;

	rc = sqlite3_exec(data_base->db, string_query , callback_listserver , (void*)risultato , &temp_error);

  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );
		*no_error = 0 ;
		return NULL;
  }
	
	char *temp_string;
	*no_error = 1;
	temp_string = (char *)risultato->object ;
	free(risultato);
	return temp_string;
}



// funzione usata per eleborare ogni risultato della funzione sqlite3_exec(), viene lanciata per ogni record avuto come risultato
// Parametri-> Struttura usata per tenere memoria del risultato, numeri di campi del record avuto come risultato, array di stringhe con i valori 
//						 dei campi del record, array di stringhe con i nomi dei campi del record
int callback_dom_serv(void* risultato, int argc, char **argv, char **azColName)
{
	
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery *) risultato;
	char *string_result;
	string_result = (char *) temp_risultato->object;

	int temp_count = temp_risultato->count ;				

	int i ;
	int length = 0;
	char *pt_temp;

	if( temp_risultato->l_row > 0)
	pt_temp = string_result;

	for(i=0; i<argc ; i++){
	length = length + strlen (argv[i] ? argv[i] : "NULL") ;
	}
	length = length + 3 ;
	
	
	temp_risultato->count = temp_risultato->count  + length ;

	string_result = (char *) calloc ( (temp_risultato->count +1 ) , sizeof(char) );	
	
	if( temp_risultato->l_row > 0)
	memcpy( string_result , pt_temp , sizeof(char) * temp_count );	

  for(i=0; i<argc; i++)
	{
		if(i > 0)
		strcat( string_result ,"|" );

    strcat( string_result , (argv[i] ? argv[i] : "NULL") );
  }
  strcat( string_result ,"^");
	
	if( temp_risultato->l_row > 0)
	free(pt_temp);

	++temp_risultato->l_row;
  temp_risultato->object = (void *) string_result ;
	return 0;
}



// select sul DB per creare la stringa dom-server ritorna: il risultato della query in una stringa (puntatore a char) 
// Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" l'eventuale errore, varibile "booleana" 
//						 per segnalare l'errore
char* select_dom_serv(DB* data_base , char* string_query , char* string_error , int* no_error)
{
	int rc;
	ResultQuery *risultato;
	risultato = (ResultQuery * ) malloc ( sizeof(ResultQuery)); 
	risultato->l_row = 0 ;
	risultato->count = 0; 
	risultato->object = NULL ;
	char *temp_error;

	rc = sqlite3_exec(data_base->db, string_query , callback_listserver , (void*)risultato , &temp_error);

  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );
		*no_error = 0 ;
		return NULL;
  }
	
	char *temp_string;
	*no_error = 1;
	temp_string = (char *)risultato->object ;
	free(risultato);
	return temp_string;
}




// (Client) funzione usata per eleborare ogni risultato della funzione sqlite3_exec(), viene lanciata per ogni record avuto come risultato
// Parametri-> Struttura usata per tenere memoria del risultato, numeri di campi del record avuto come risultato, array di stringhe con i valori 
//						 dei campi del record, array di stringhe con i nomi dei campi del record
int callback_sel(void* risultato, int argc, char **argv, char **azColName)
{
	
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery *) risultato;
	char *string_result;
	string_result = (char *) temp_risultato->object;
				
	int i ;
	int length ;
	char *color = "\e[1;34m" ;

	if(temp_risultato->l_row == 0 )
	{
		length = 20; // partiamo da 20 per l'aggiunta dei colori
		for(i=0; i<argc ; i++)
		{
			length = length + 1 + strlen (azColName[i]) ;
		}
		++length;
		temp_risultato->count = temp_risultato->count  + length ;
		string_result = (char *) calloc (temp_risultato->count , sizeof(char) );

		strcat( string_result , color ); // colori campi tabella
		for(i=0; i<argc ; i++)
		{
			strcat( string_result ,"|" );
  		strcat( string_result , azColName[i]);
		}
		strcat( string_result ,"\033[m"); // colori campi tabella
		strcat( string_result ,"\n");

		++temp_risultato->l_row;

	}

	char *pt_temp = string_result;
	int temp_count = temp_risultato->count ;


	length = 0;
	for(i=0; i<argc ; i++){
	length = length + 1 + strlen (argv[i] ? argv[i] : "NULL") ;
	}
	length += 1;
	
	temp_risultato->count = temp_risultato->count  + length ;
	string_result = (char *) calloc ( temp_risultato->count , sizeof(char) );	

	memcpy( string_result , pt_temp , sizeof(char) * temp_count );	

  for(i=0; i<argc; i++)
	{
		strcat( string_result ,"|" );
    strcat( string_result , (argv[i] ? argv[i] : "NULL") );
  }
  strcat( string_result ,"\n");

	free(pt_temp);
	++temp_risultato->l_row;
	temp_risultato->object = (void *) string_result ;  

	return 0;
}



// (Client) select su una tabella ritorna: il risultato della query in una stringa (puntatore a char) 
// Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" l'eventuale errore, varibile "booleana" 
//						 per segnalare l'errore
char* select_tb(DB* data_base , char* string_query , char* string_error , int* no_error)
{
	int rc ;
	ResultQuery *risultato;
	risultato = (ResultQuery * ) malloc ( sizeof(ResultQuery)); 
	char *temp_error;
	risultato->l_row = 0 ;
	risultato->count = 0; 
	risultato->object = NULL ;

	rc = sqlite3_exec(data_base->db, string_query , callback_sel , (void*)risultato , &temp_error);

  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );

		*no_error = 0 ;
		return NULL;
  }

	char *temp_string;
	*no_error = 1;
	temp_string = (char *)risultato->object ;
	free(risultato);
	return temp_string;

}



// select sul DB usato per la lettura dei parametri di configurazione, ritorna: il valore del parametro di configurazione
// Parametri-> descrittore db sqlite3, un puntatore char con il nome del parametro, un puntatore char per "restituire" l'eventuale errore,
//    		     varibile "booleana" per segnalare l'errore
								 
int select_conf_param(DB* data_base , char* name , char* string_error , int* no_error)
{
	int l;
	int rc;
	sqlite3_stmt* result;
	char *temp_str = "select valore from Parametri where nome = '";
	int count = strlen(temp_str) + strlen(name) + 2 ; // per l'apice e il carattere di terminazione stringa 


	char *string_query;
	string_query = (char *) calloc ( count , sizeof(char) );
	strcpy( string_query , temp_str) ;
	strcat( string_query , name);
	strcat( string_query , "'");

	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			l = sqlite3_column_int(result, 0);
		}
		sqlite3_finalize(result);
		*no_error = 1 ;
	}
	else
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Error database: ") ;
		strcat( string_temp , sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);
		*no_error = 0 ;
	}
	return l;
}



// select sul DB usato per la lettura dei parametri di configurazione tipo char, ritorna: il valore del parametro di configurazione
// Parametri-> descrittore db sqlite3, un puntatore char con il nome del parametro, un puntatore char per "restituire" l'eventuale errore,
//    		     varibile "booleana" per segnalare l'errore
								 
char* select_conf_charparam(DB* data_base , char* name , char* string_error , int* no_error)
{
	char *parametro;
	int rc;
	sqlite3_stmt* result;
	unsigned const char *pt_result ;
	char *temp_str = "select valore from CharParam where nome = '";
	int count = strlen(temp_str) + strlen(name) + 2 ; // per l'apice e il carattere di terminazione stringa 


	char *string_query;
	string_query = (char *) calloc ( count , sizeof(char) );
	strcpy( string_query , temp_str) ;
	strcat( string_query , name);
	strcat( string_query , "'");

	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			pt_result = sqlite3_column_text(result, 0);
		}
		
		parametro = calloc( 1, sizeof(char) * (strlen(pt_result) + 1 ) );
		strcpy(parametro , pt_result  );
		
		sqlite3_finalize(result);
		*no_error = 1 ;
	}
	else
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Error database: ") ;
		strcat( string_temp , sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);
		*no_error = 0 ;
	}
	return parametro;
}



// select sul DB per la lettura della versione
// Parametri-> descrittore db sqlite3, un puntatore double per "restituire" il valore del parametro di versione v_dom, 
// 						 un puntatore double per "restituire" il valore del parametro di versione v_primary,
//    		     un puntatore char per "restituire" l'eventuale errore.
								 
void select_conf_vers(DB* data_base , double* v_dom , double* v_primary , char* string_error)
{
	sqlite3_stmt* result;
	char *string_query= "select v_dom , v_primary from Version";
	int rc;

	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);	

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			*v_dom = sqlite3_column_double(result, 0);
			*v_primary = sqlite3_column_double(result, 1);
		}
		sqlite3_finalize(result);
	}
	else
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Error database: ") ;
		strcat( string_temp , sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);
	}
}



// select sul DB per la lettura del nome del server e dell'id, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
// Parametri-> descrittore db sqlite3, un puntatore char per "restituire" il nome del server, un puntatore int per "restituire" l'id 
// l'eventuale errore.					 
int select_server_NameId(DB* data_base , char* name , int *id, char* string_error)
{
	sqlite3_stmt* result;
	unsigned const char *pt_result ;
	char *string_query= "select nome , id_server from NameIdServer where id = 0";
	int rc;

	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);	

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			pt_result = sqlite3_column_text(result, 0);
			*id = sqlite3_column_int(result, 1);
		}
		strcpy(name , pt_result  );

		sqlite3_finalize(result);
	}
	else
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _NW_DIM_ERROR );
		strcpy( string_temp , "Error database: ") ;
		strcat( string_temp , sqlite3_errmsg(data_base->db));
		strncpy(string_error , string_temp , _NW_DIM_ERROR );
		free(string_temp);
		return 0;
	}
	return 1;
}



// (Serverboot) select sul DB per la lettura della password amministratore ritorna: 0 se il nick non è corretto, 1 se il nick è corretto
// Parametri-> descrittore db sqlite3, un puntatore char al nickname
// 						  , un puntatore char per "restituire" la password, la dimensione della password
int select_password_am(DB *data_base , char* nick , char* psw ,int dim)
{
	sqlite3_stmt* result;
	int b = 0;
	unsigned const char *pt_result ;
	char *temp_str = "select psw from Accesso where nick ='";
	int count = strlen(temp_str) + strlen(nick) + 2 ; // per l'apice e il carattere di terminazione stringa

	char *string_query;
	string_query = (char *) calloc ( count , sizeof(char) );
	strcpy( string_query , temp_str) ;
	strcat( string_query , nick);
	strcat( string_query , "'");

	sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);

	if(sqlite3_step(result) == SQLITE_ROW)	
	{
		pt_result = sqlite3_column_text(result, 0);
		b = 1;
	}
	memcpy(psw , pt_result , sizeof(char)*dim );

	sqlite3_finalize(result);
	return b;
}



//  Select sul DB per la lettura delle informazioni sul BootServer, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore db sqlite3, un puntatore char per "restituire" il valore dell'ip, 
// 						 un puntatore uint16_t per "restituire" il valore della porta,
//    		     il nome del server di boot.
int select_boot_server(DB* data_base , char *ip ,  uint16_t *port , int id)
{
	sqlite3_stmt* result;
	char *temp_string_query= "select ip, port from BootServer where id_server = '";
	
	char string_query[501];	
	int rc;
	
	memset( string_query , '\0' , 501 );
	
	snprintf(string_query, 501*sizeof(char) , "select ip, port from BootServer where id_server = %d " , id);
	
	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);	

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			strcpy(ip , sqlite3_column_text(result, 0) );
			*port = (uint16_t) sqlite3_column_int(result, 1);
		}
		sqlite3_finalize(result);
		rc = 1;
	}
	else
	{
		rc = 0 ;
	}


	return rc;
}



//  Select sul DB per la lettura delle informazioni sul BootServer, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore db sqlite3, il nome della tabella, l'ip restituito , la porta restituita , le condizioni per la query
int select_IpPort_server(DB* data_base , char *tb , char *ip ,  int *port , char *cond)
{
	sqlite3_stmt* result;
	char string_query[500];
	int rc;
	memset (string_query , '\0' , 500);
	
	snprintf(string_query, 500*sizeof(char) , "select ip , port from %s %s" , tb , cond);
	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);	

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			strcpy(ip , sqlite3_column_text(result, 0) );
			*port =  sqlite3_column_int(result, 1);
		}
		sqlite3_finalize(result);
		rc = 1;
	}
	else
	{
		rc = 0 ;
	}

	return rc;	
}



//  Select sul DB per la lettura del primary relativo al dominio, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore db sqlite3, il nome del dominio , un puntatore char per "restituire" il nome del server, 	 
int select_nameServer_dominio(DB* data_base , char *dominio ,  char *name )
{
	sqlite3_stmt* result;
	char string_query[500];
	int rc;
	memset (string_query , '\0' , 500);
	
	snprintf(string_query, 500*sizeof(char) , "select server from Dom where dominio = '%s'" , dominio );

	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);	

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			strcpy(name , sqlite3_column_text(result, 0) );
		}
		sqlite3_finalize(result);
		rc = 1;
	}
	else
	{
		rc = 0 ;
	}

	return rc;
}



//  Select sul DB per la lettura del primary relativo al dominio, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore db sqlite3, il nome del dominio , un puntatore char per "restituire" il nome del server, un puntatore char 
//	per "restituire" l'ip	e un int* per restituire la porta 
int info_primary_dominio(DB* data_base, char* dominio , char*server , char* ip , int* port)
{
	sqlite3_stmt* result;
	char string_query[500];
	int rc;
	memset (string_query , '\0' , 500);
		
	snprintf(string_query, 500*sizeof(char) , "select server , ip , port from Dom where dominio = '%s'" , dominio );

	rc = sqlite3_prepare_v2(data_base->db, string_query, strlen(string_query), &result, NULL);	

	if( rc ==	SQLITE_OK)
	{
		if(sqlite3_step(result) == SQLITE_ROW)
		{
			strcpy(server , sqlite3_column_text(result, 0) );
			strcpy(ip , sqlite3_column_text(result, 1) );
			*port =  sqlite3_column_int(result, 2);
			
		}
		sqlite3_finalize(result);
		rc = 1;
	}
	else
	{
		rc = 0 ;
	}

	return rc;
}



// (Server)funzione usata per eleborare ogni risultato della funzione sqlite3_exec(), viene lanciata per ogni record avuto come risultato
// Parametri-> Struttura  risultato, numeri di campi del record avuto come risultato, array di stringhe con i valori 
//						 dei campi del record, array di stringhe con i nomi dei campi del record
int callback_bufferServer(void* list, int argc, char **argv, char **azColName)
{
	
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery *) list;
	struct L_Server *listS;
	listS = (struct L_Server *) temp_risultato->object;

	int index = temp_risultato->l_row ;
	
	if(index < temp_risultato->count)
	{
		strcat( listS[index].name , (argv[0] ? argv[0] : "NULL") );
		strcat( listS[index].ip , (argv[1] ? argv[1] : "NULL") );
		listS[index].port = atoi( argv[2] ? argv[2] : "0") ;
		++temp_risultato->l_row;
	}
	
  temp_risultato->object = (void *) listS ;
	return 0;
}



//  (Server) select di nua tabella memorizzata in una struttura Server, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//  Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" il risultato, un puntatore char
//  per "restituire" l'eventuale errore 

int select_on_bufferServer(DB* data_base , char* string_query , ResultQuery *list, char* string_error )
{
	int rc;
	char *temp_error;

	rc = sqlite3_exec(data_base->db, string_query , callback_bufferServer , (void*)list , &temp_error);

  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );
		return 0;
  }
	
	return 1;
}



// (Server)funzione usata per eleborare ogni risultato della funzione sqlite3_exec(), viene lanciata per ogni record avuto come risultato
// Parametri-> Struttura  risultato, numeri di campi del record avuto come risultato, array di stringhe con i valori 
//						 dei campi del record, array di stringhe con i nomi dei campi del record
int callback_bufferDomini(void* list, int argc, char **argv, char **azColName)
{
	
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery *) list;
	struct L_Dominio *listD;
	listD = (struct L_Dominio *) temp_risultato->object;

	int index = temp_risultato->l_row ;
	
	if(index < temp_risultato->count)
	{
		strcat( listD[index].name , (argv[0] ? argv[0] : "NULL") );

		++temp_risultato->l_row;
	}
	
  temp_risultato->object = (void *) listD ;
	return 0;
}



//  (Server) select di una tabella memorizzata in una struttura Domini, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//  Parametri-> descrittore db sqlite3, un puntatore char con la query, un puntatore char per "restituire" il risultato, un puntatore char
//  per "restituire" l'eventuale errore 

int select_on_bufferDomini(DB* data_base , char* string_query , ResultQuery *list, char* string_error )
{
	int rc;
	char *temp_error;

	rc = sqlite3_exec(data_base->db, string_query , callback_bufferDomini , (void*)list , &temp_error);

  if( rc!=SQLITE_OK )
	{
		memset(string_error , '\0' , _NW_DIM_ERROR );	
		
		strcpy( string_error , "SQL error: ") ;
		strncat(string_error , temp_error , _NW_DIM_ERROR - strlen(string_error) );
		return 0;
  }
	
	return 1;
}
