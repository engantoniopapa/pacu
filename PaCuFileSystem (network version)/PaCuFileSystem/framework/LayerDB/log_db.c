#include "api_db.h"
#include "log_db.h"

// Setta i valori della struttura TrLog
void set_TrLog( TrLog *l , int id_p , int id_c , short int type , char *ip , int port, char *res , UnitLog* list , int length)
{
	l->id_p = id_p;
	l->id_c = id_c;
	strncpy( l->ip , ip , LOG_IP-1);
	l->port = port;
	l->type = type;
	l->list = list;
	strncpy( l->local_res , res , LOG_RES-1);
	l->length = length;
}



// Rimuove la struttura TrLog
void remove_det_TrLog( TrLog *l)
{
	free(l->list);
	free(l);
}


// Rimuove la lista di strutture TrLog
void remove_list_TrLog( TrLog *l , int len)
{
	int i ;
	for( i = 0 ; i < len ; ++i)
		free(l[i].list);
	
	free(l);
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
int insert_log(char *path, int id_p ,int id_c ,char *ip ,int port , short int type ,char *r1 ,char *r2, char *r3,int opt1 ,int opt2 ,short int state)
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

	snprintf(string_query , LOG_QUERY-1 , "insert into Log values(NULL , %d , %d , '%s' , %d , %d , '%s' , '%s' , '%s', %d ,%d , %d ) " , id_p, id_c, ip , 		port ,type, r1, r2, r3, opt1 , opt2 ,state);

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
int update_log(char *path , int id_p , int id_c  , short int state)
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

	snprintf(string_query , LOG_QUERY-1 , "update Log set state = %d  where id_p = %d AND id_c = %d" , state , id_p , id_c );
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



// aggiorna i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int update_log_state(char *path , short int old_state  , short int new_state)
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

	snprintf(string_query , LOG_QUERY-1 , "update Log set state = %d  where state = %d" , new_state , old_state );
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



// aggiorna i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int update_log_opt(char *path , int id_p , int id_c  , short int state , int opt1 , int opt2)
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

	snprintf(string_query ,LOG_QUERY-1 ,"update Log SET state = %d , opt1 = %d , opt2 = %d where id_p = %d AND id_c = %d", state ,opt1, opt2, id_p, id_c);

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
  if( !rc )
  {
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
int update_log_res1(char *path , int id_p , int id_c  , short int state , char* res1)
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

	snprintf(string_query , LOG_QUERY-1 , "update Log SET state = %d , ris1 = '%s' where id_p = %d AND id_c = %d" , state ,res1 , id_p , id_c );
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



// aggiorna i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int update_log_res2(char *path , int id_p , int id_c  , short int state , char* res1 , char* res2)
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

	snprintf(string_query , LOG_QUERY-1 , "update Log SET state = %d , ris2 = '%s' where id_p = %d AND id_c = %d AND ris1 = '%s' " , state ,res2 , id_p , id_c , res1);
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



// aggiorna i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int update_log_res3(char *path , int id_p , int id_c  , short int state , char* res1  , char* res2  , char* res3)
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

	snprintf(string_query , LOG_QUERY-1 , "update Log SET state = %d , ris3 = '%s' where id_p = %d AND id_c = %d AND ris1 = '%s' AND ris2 = '%s'" , state ,res3 , id_p , id_c , res1 , res2);
	
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



// aggiorna i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int update_log_res2res3(char *path , int id_p , int id_c  , short int state , char* res1  , char* res2  , char* res3)
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

	snprintf(string_query , LOG_QUERY-1 , "update Log SET state = %d , ris2 = '%s' , ris3 = '%s' where id_p = %d AND id_c = %d AND ris1 = '%s' " , state , res2 , res3 , id_p , id_c , res1 );
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



// elimina i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int delete_log(char *path , int id_p , int id_c )
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

	snprintf(string_query , LOG_QUERY-1 , "delete from Log where id_p = %d AND id_c = %d" , id_p , id_c );
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



// elimina tutti i record della tabella log file, ritorna: 1 se l'operazione è riuscita 0 se c'e qualche errore 
int delete_all_log(char *path)
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

	snprintf(string_query , LOG_QUERY-1 , "delete from Log " );
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
		set_TrLog( &(list[index]) , atoi(argv[0]) ,atoi(argv[1]) , (unsigned char) atoi(argv[2]) ,argv[3] ,atoi(argv[4]) , argv[5] ,NULL , 0);

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
		
	rc = open_db_ro( db_log , path , stringa_errore);
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
	snprintf(string_query , LOG_QUERY-1 , "select id_p , id_c , type , ip , port , ris1 from (select * from %s %s %s)", str_tab , str_cond , str_groupby);
	
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



int callback_select_detTR(void* object_RQ, int argc, char **argv, char **azColName)
{
	ResultQuery *temp_risultato ;
	temp_risultato = (ResultQuery*) object_RQ;
	TrLog *tr;
	tr = (TrLog *) temp_risultato->object;

	int index = temp_risultato->l_row ;
	
	if(index < temp_risultato->count)
	{
		strncpy(tr->list[index].res2, argv[0] ,LOG_RES-1);
		strncpy(tr->list[index].res3, argv[1] ,LOG_RES-1);
		tr->list[index].opt1 = (short int) atoi(argv[2] );
		tr->list[index].opt2 = (short int) atoi(argv[3] );
		tr->list[index].state = (short int) atoi(argv[4] );
		++temp_risultato->l_row;
	}
	
  temp_risultato->object = (void *) tr ;
	return 0;
}


// ottieni i dettagli sulla transazione , ritorna: la lista della transazioni selezionate o null
// lo stato della funzione è inserito per riferimento in len che ritorna il numero di transazioni selezionato oppure -1 se c'e' un errore
TrLog *select_detTR(char *path, char *str_tab, int id_p, int id_c, short int type, char *ip, int port, char *ris1, char *stringa_errore, int *rc)
{	 		
	TrLog *tr_log;
	DB *db_log; // descrittore del DB Log
	int len;
	db_log = ( DB *) malloc (sizeof(DB) ) ;

	*rc = open_db_ro( db_log , path , stringa_errore);
	if( !(*rc) )
	{
		fprintf(stderr, "%s\n", stringa_errore);
		free(db_log);
  	*rc = -1;
    return NULL ;
	}

	char string_query[LOG_QUERY] = {0x0};

	snprintf(string_query , LOG_QUERY-1 , " select count(*) from %s where id_p =  %d AND id_c = %d" , str_tab , id_p , id_c);			

 	len = count_log( db_log,  string_query , stringa_errore);

 	if(len < 0)
 	{
 		free(db_log);
 		*rc = -1;
		return NULL;
	}

	if(len == 0)
	{
		*rc = 0;
		return NULL;
	}

	tr_log = (TrLog *) calloc( 1 , sizeof(TrLog) );
	tr_log->list = (UnitLog * ) calloc ( len , sizeof(UnitLog) );

	set_TrLog( tr_log , id_p , id_c , type , ip , port , ris1  , tr_log->list , len );

	memset(string_query , '\0' , LOG_QUERY);
	snprintf(string_query , LOG_QUERY-1 , "select ris2, ris3, opt1 , opt2 ,state from %s where id_p = %d AND id_c = %d"  ,  str_tab , id_p , id_c);

	ResultQuery* objectRQ;
	objectRQ = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );
	objectRQ->object = (void*) tr_log;
	objectRQ->count = len;
	objectRQ->l_row = 0;

	char *temp_error;
	*rc = sqlite3_exec(db_log->db, string_query , callback_select_detTR , (void*)objectRQ , &temp_error);

	if( *rc!=SQLITE_OK )
	{
		memset(stringa_errore , '\0' , _NW_DIM_ERROR );	
		
		strcpy( stringa_errore , "SQL error: ") ;
		strncat(stringa_errore , temp_error , _NW_DIM_ERROR - strlen(stringa_errore) );
		*rc = -1;
		free(objectRQ);
		free(tr_log->list);
		free( tr_log );
		free(db_log);
		return NULL;
	}

	*rc = close_db(db_log, stringa_errore);
  if( !(*rc) )
  {
  	free(objectRQ);
    free(db_log);
    free(tr_log->list);
    free(tr_log );
  	*rc = -1 ;
    return NULL ;
  }

  free(db_log);
  free(objectRQ);

  return tr_log;
}

