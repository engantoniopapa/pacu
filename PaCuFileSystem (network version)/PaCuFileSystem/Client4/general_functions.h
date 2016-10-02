

void conf_client(); // inizializzazione delle variabili di configurazione se si accede come client generico

void conf_am(); // inizializzazione delle variabili di configurazione se si accede come amministratore

void print_conf(); //stampa la configurazione del sistema

int update_version(double , double); //aggiorna la version del client

Server * create_list_server(DB * ,  int* , char* , char* , char * );// crea una lista dei server in base ai vincoli inseriti

void free_list_server(); //  dealloca la struttura

Dominio * create_list_domini(DB * ,  int* , char* , char* , char * );// crea una lista dei domini in base ai vincoli inseriti

void free_list_domini(); //  dealloca la struttura

int anal_sint_list(char *list); //  analizza sintatticamente la lista formattata

int update_dom_primary( char * , char *  , char*  , int  , char * , char *); // aggiorna il db primary e domini con i nuovi dati


