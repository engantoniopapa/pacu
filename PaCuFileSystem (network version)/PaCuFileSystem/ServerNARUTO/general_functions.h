#define _DIM_COND 150
#define _DIM_QUERY_COND 300



struct Th_CtrlRep
{
	int timeout;
	int tentativi;
	char remote_ip[_DIM_IP];
	int port;
	int state;
};
typedef struct Th_CtrlRep Th_CtrlRep;




void get_conf(); // inizializzazione delle variabili di configurazione

Server * create_list_server(DB * ,  int* , char* , char* , char * ); // crea una lista dei server in base ai vincoli inseriti

char *create_string_dom_serv(DB * , int * , char*  , char *, char * ); // crea la stringa formattata domini - server primary

Server *vistaServer(char * , char * ,	int * ); // crea la vista dei server per un dato dominio

int is_primary( char * , int  , char *); // verifica se il server è primary della risorsa

int is_replica( char * , int   , char *); // verifica se il server è replica della risorsa

int is_Primary_OR_Replica( char *, int , char *); // verifica se il server è primary o replica della risorsa

int is_dominio (char * , int   ); // verifica se esiste la risorsa,

int whois_primary( char * , int   , char * , char *, int* ); //cerca il primary della risorsa

Dominio * create_list_domini(DB * , int * , char *, char * );//  crea la lista dei domini

int get_count( );  // restituisce l'id incrementato

void get_idTr( int * , int *); // restituisce un id transaction valido

int control_replic(Server * , int * , char * , char *);// il primary verifica se i server replica di un dominio sono attivi

void *func_helper_ctrlRep(void *l); //funzione helper di control_replic
