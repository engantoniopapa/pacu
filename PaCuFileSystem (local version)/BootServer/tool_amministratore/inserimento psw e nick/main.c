
#include "../../../framework/framework.h"


#define PATH_DB_CONF  "../../DB/Conf"
#define _MAX_STRING_ERROR _NW_DIM_ERROR // la lunghezza massima della stringa errore



int main(int argc, char *argv[])
{
  
  int rc;
  int scelta;
  char *str_query;
  sqlite3 *db;
  char nick[31];
  char password[31];
  char hash[16];
	char *stringa_errore ;
	int count;

	stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char));

	rc = sqlite3_open_v2(PATH_DB_CONF, &db, SQLITE_OPEN_READWRITE, NULL);
	
	 if( rc!=SQLITE_OK )
	{
		char *string_temp; 
		string_temp = (char *) malloc((sizeof(char)) * _MAX_STRING_ERROR );
		strcpy( string_temp , "Can't open database: ") ;
		strcat( string_temp , sqlite3_errmsg(db));
		strncpy(stringa_errore , string_temp , _MAX_STRING_ERROR );
		free(string_temp);
		printf("%s \n" ,stringa_errore);

    sqlite3_close(db);
    return 0;
  }
  
  printf("Inserisci nickname:\n");
  scanf("%s" , nick); 

  printf("Inserisci password:\n");
  scanf("%s" , password);
  
  printf("Seleziona Operazione :\n");
  printf("1) Inserimento\n");
  printf("2) Update Password\n"  );
  scanf("%d" , &scelta);
  

  md5_state_t hash_state;
	md5_init(&hash_state);
  md5_append(&hash_state, (const md5_byte_t *)password, strlen(password));
	md5_finish(&hash_state, (md5_byte_t *) hash);

	
	

  if(scelta == 1 )
  {
    char *temp_query_1 = "Insert into Accesso values('";
    char *temp_query_2 = " , '";
		count = strlen(temp_query_1)  + strlen(temp_query_2) + strlen(nick) + 16 + 4;
		str_query = (char*) calloc (count , sizeof(char) );
    strcpy(str_query , temp_query_1);
		strcat(str_query , nick);
		strcat(str_query , "'");
		strcat(str_query , temp_query_2);
		strncat(str_query, hash , 16);
		strcat(str_query , "')");
    
  }
  
  if(scelta == 2 )
  {
    char *temp_query_1 = "update  Accesso set psw = '";
		char *temp_query_2 = " where nick = '";
    count =  strlen(temp_query_1) + strlen(temp_query_2) + 16 + 3 + strlen(nick);
		str_query = (char*) calloc (count , sizeof(char) );
		strcpy(str_query , temp_query_1);
		strncat(str_query , hash,16);
		strcat(str_query , "'");
		strcat(str_query , temp_query_2);
		strcat(str_query, nick );
		strcat(str_query , "'");
  }

 	printf("query -> %s \n" , str_query);
  
  if( generic_op_db(db , str_query , stringa_errore) )
	{
		printf("OPERAZIONE RIUSCITA :) \n");
	}
	else
	{
		printf("ERRORE '*_* \n\n");
		printf("%s \n" ,stringa_errore);
	}

  sqlite3_close(db);

  free(str_query);
	free(stringa_errore);

  return 0;
}

