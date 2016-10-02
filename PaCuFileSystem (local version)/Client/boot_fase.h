
int init_boot_fase(int ); // avvio fase di boot del client

int send_ping(int , BootMessage * ); //	Invio messaggio di Bootping

int recv_pong( int  , BootMessage *  ); //	Ricezione messaggio di Bootpong

int check_state_boot( double  , double );//  Verifica la versione del client

int send_req_lilst(int  , BootMessage* ); //	Invio messaggio di BootList

int recv_server_list(int ); //	ricevo la lista PrimaryServer

int parsing_server_list(char *list); //  analizza la lista server

int insert_servlist_DB( char*  , char*); // inserisci nel Db la serv list ricevuta
