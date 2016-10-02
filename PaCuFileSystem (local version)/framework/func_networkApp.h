

BootAM* create_boot_am(); // crea un messaggio di tipo BootAM

int connessioneClient(char *, uint16_t , int );// Crea la connessione

void addr_init(struct sockaddr_in *, in_port_t * , long int ) ;// Inizializzazione della variabile sockaddr_in

int connessioneServer(int listensd); // effettua la connect

int set_timeout(int  , time_t  , suseconds_t  , time_t  , suseconds_t ); // imposta il timeout sulla connessione

void get_ip_string( char * , struct in_addr * ); // converto un indirizzo ip in formato stringa in uno in formato u_long (unsigned long)

int send_InitMessage( InitMessage * , int ); //	Invia un messaggio di tipo Init_Message

int  recv_InitMessage( int , InitMessage *); // riceve un messaggio di tipo Init_Message

int send_IDT_Message(int  , IDT_Message* ); //	Invio messaggio IDT

int  recv_IDT_Message( int , IDT_Message* ); // riceve un messaggio di tipo IDT

int send_DiagMessage(int  ,  DiagMessage* ); //	Invio messaggio diagnostico

int recv_DiagMessage( int , DiagMessage *); //	Ricezione messaggio di Diagnostic

int send_TrMessage(int , TrMessage* ); // Invio messaggio transaction

int  recv_TrMessage( int , TrMessage * ); // Ricezione messaggio transaction

char* writeFile_on_buffer(int , size_t); // Scrive su buffer il contenuto di un file o socket

char* writeFile_on_buffer_noblock(int , size_t );// Scrive su buffer il contenuto di un file o socket (bloccante solo prima della ricezione)

int writeBuffer_on_file(int , size_t , char* ); //  Scrive su file o socket il contenuto di un buffer

ssize_t writen(int , const void *, size_t ); // gestisce la scrittura su socket

ssize_t readn(int , void *, size_t ); // gestisce la lettura su socket

ssize_t readn_noblock(int , void *, size_t ); // gestisce la lettura su socket bloccante solo per la prima ricezione

void print_diaError(int ); // stampa il tipo di DiagError riscontrato

void print_TrError(int , char*); //  Stampa il tipo di TrError riscontrato in una stringa
