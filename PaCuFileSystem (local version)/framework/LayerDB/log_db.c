#include "api_db.h"
#include "log_db.h"

// Setta i valori della struttura TrLog
void set_TrLog( TrLog *l , int id_p , int id_c , unsigned char type , char *res , UnitLog* list , int length)
{
	l->id_p = id_p;
	l->id_c = id_c;
	l->type = type;
	l->list = list;
	strncpy( l->local_res , res , LOG_RES-1);
	l->length = length;
}


// conta i record log dal Db, ritorna: il numero di record selezionati oppure -1 se c'e un errore
int count_log(DB* data_base , char* string_query , char* string_error)
{	
	int l;
	int rc;
	sqlite3_stmt* result;


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



// inserisce un record nella tabella dei log, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore
int insert_log(char *path , int id_p , int id_c , unsigned char type , char *ris1 , char *ris2 , short int state)
{
	DB *db_log; // descrittore del DB Log
	int rc;
	char *stringa_errore;
	short int b = 0;
	char string_query[LOG_QUERY] = {0x0} ;
	
	
	db_log = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_NW_DIM_ERROR  , sizeof(char) );
		
	rc = open_db_rw( db_log , path , stringa_errore);
	if( !rc )
	{
		fprintf(stderr, "%s\n", stringa_errore);
		free(db_log);
  	free(stringa_errore);
    return 0 ;
	}

	snprintf(string_query , LOG_QUERY-1 , "insert into Log values(NULL , %d , %d , %c , '%s' , '%s' , %d ) " , id_p, id_c, type, ris1, ris2, state);

	if(insert_db(db_log , string_query , stringa_errore) )
	{
		b = 1 ;
	}
	else
	{
    fprintf(stderr, "%s\n", stringa_errore);
    b = 0;
	}
	
	rc = close_db(db_log, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    free(db_log);
  	free(stringa_errore);
    return 0 ;
  }
  
  free(db_log);
  free(stringa_errore);
  return b;
}



// aggiorna i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int update_log(char *path , int id_p , int id_c , unsigned char type  , short int state)
{
	DB *db_log; // descrittore del DB Log
	int rc;
	char *stringa_errore;
	short int b = 0;
	char string_query[LOG_QUERY] = {0x0} ;
	
	
	db_log = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_NW_DIM_ERROR  , sizeof(char) );
		
	rc = open_db_rw( db_log , path , stringa_errore);
	if( !rc )
	{
		fprintf(stderr, "%s\n", stringa_errore);
		free(db_log);
  	free(stringa_errore);
    return 0 ;
	}

	snprintf(string_query , LOG_QUERY-1 , "update Log set state = %d , where id_p = %d AND id_c = %d" , state , id_p , id_c );
	if(generic_op_db(db_log , string_query , stringa_errore) )
	{
		b = 1 ;
	}
	else
	{
    fprintf(stderr, "%s\n", stringa_errore);
    b = 0;
	}
	
	rc = close_db(db_log, stringa_errore);
  if( !rc ){
    fprintf(stderr, "%s\n", stringa_errore);
    free(db_log);
  	free(stringa_errore);
    return 0 ;
  }
  
  free(db_log);
  free(stringa_errore);
  return b;
}




int callback_select_recTR(void* object_RQ, int argc, char **argv, char **azColName)
{
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery*) object_RQ;
	TrLog *list;
	list = (TrLog *) temp_risultato->object;

	int index = temp_risultato->l_row ;
	
	if(index < temp_risultato->count)
	{
		set_TrLog( &(list[index]) , atoi(argv[0]) ,atoi(argv[1]) , (unsigned char) atoi(argv[2]) , argv[3]  , NULL , 0);

		++temp_risultato->l_row;
	}
	
  temp_risultato->object = (void *) list ;
	return 0;
}


// seleziona le transazioni durante la procedura di recovery, ritorna: la lista della transazioni selezionate o null
// lo stato della funzione è inserito per riferimento in len che ritorna il numero di transazioni selezionato oppure -1 se c'e' un errore
TrLog *select_recTR(char *path , char *str_tab, char *str_cond , char *str_groupby ,char *stringa_errore , int *len)
{
	TrLog *list_log;
	DB *db_log; // descrittore del DB Log
	int rc;
	db_log = ( DB *) malloc (sizeof(DB) ) ;
		
	rc = open_db_rw( db_log , path , stringa_errore);
	if( !rc )
	{
		fprintf(stderr, "%s\n", stringa_errore);
		free(db_log);
  	*len = -1;
    return NULL ;
	}
	
	char string_query[LOG_QUERY] = {0x0};
	
	snprintf(string_query , LOG_QUERY-1 , " select count(*) from ( select * from %s %s %s)" , str_tab , str_cond , str_groupby);
 	*len = count_log( db_log,  string_query , stringa_errore);
 	
 	if(*len < 0)
 	{
 		free(db_log);
		return NULL;
	}
	
	if(*len == 0)
	{
		return NULL;
	}
	
	list_log = (TrLog *) calloc( *len , sizeof(TrLog) );

	
	memset(string_query , '\0' , LOG_QUERY);
	snprintf(string_query , LOG_QUERY-1 , "select id_p , id_c , type , ris1 from (select * from %s %s %s)"  ,  str_tab , str_cond , str_groupby);
	
	ResultQuery* objectRQ;
	objectRQ = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );
	objectRQ->object = (void*) list_log;
	objectRQ->count = *len;
	objectRQ->l_row = 0;
	
	char *temp_error;
	rc = sqlite3_exec(db_log->db, string_query , callback_select_recTR , (void*)objectRQ , &temp_error);

	if( rc!=SQLITE_OK )
	{
		memset(stringa_errore , '\0' , _NW_DIM_ERROR );	
		
		strcpy( stringa_errore , "SQL error: ") ;
		strncat(stringa_errore , temp_error , _NW_DIM_ERROR - strlen(stringa_errore) );
		*len = -1;
		free(objectRQ);
		free( list_log );
		free(db_log);
		return NULL;
	}
	
	rc = close_db(db_log, stringa_errore);
  if( !rc )
  {
  	free(objectRQ);
    free(db_log);
    free( list_log );
  	*len = -1 ;
    return NULL ;
  }
  
  free(db_log);
  free(objectRQ);
  return list_log;
}


// trova la transazione relativa al server con id + grande, ritorna l'id della transazione 0 -1 se c'e' un errore
int max_numTR(char *path  ,char *str_tab , int id )
{
	DB *db_log; // descrittore del DB Log
	int rc;
	db_log = ( DB *) malloc (sizeof(DB) ) ;
	int l;
	char stringa_errore[_NW_DIM_ERROR] = {0x0};	
	
	rc = open_db_ro( db_log , path , stringa_errore);
	if( !rc )
	{
		fprintf(stderr, "%s\n", stringa_errore);
		free(db_log);
  	l = -1;
    return l ;
	}
	
	char string_query[LOG_QUERY] = {0x0};
	snprintf(string_query , LOG_QUERY-1 , " select max(id_c) from %s where id_p = %d " , str_tab , id);
	
	sqlite3_stmt* result;


	rc = sqlite3_prepare_v2(db_log->db, string_query, strlen(string_query), &result, NULL);

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
		l = -1;
	}
	
	rc = close_db(db_log, stringa_errore);
  if( !rc )
  {
    free(db_log);
  	l = -1 ;
  }
  
  free(db_log);
	return l;
	
}
