

#define _DIM_COND 150
#define _DIM_QUERY_COND 300


void get_conf(); // inizializzazione delle variabili di configurazione

Server * create_list_server(DB * ,  int* , char* , char* , char * ); // crea una lista dei server in base ai vincoli inseriti

char *create_string_dom_serv(DB * , int * , char*  , char *, char * ); // crea la stringa formattata domini - server primary

Server *vistaServer(char * , char * ,	int * , char * ); // crea la vista dei server per un dato dominio

int is_primary( char * , int  , char *); // verifica se il server è primary della risorsa

int is_dominio (char * , int   ); // verifica se esiste la risorsa,

int whois_primary( char * , int   , char * , char *, int* ); //cerca il primary della risorsa

int get_count( );  // restituisce l'id incrementato

void get_idTr( int * , int *); // restituisce un id transaction valido
