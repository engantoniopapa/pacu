

int update_domini(); //aggiorna la tabella domini

void get_list_server(char *); // richiedi la creazione della lista server
void get_list_domini(char *); // richiedi la creazione della lista domini

int recv_dom_list(int ); //	ricevo la lista Domini

int parsing_dom_list(char *list); //  analizza la lista domini

int insert_domlist_DB( char *list   , char* string_error); // 	Inserisci nel Db la dom list ricevuta
