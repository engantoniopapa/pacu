
DiagMessage* init_DiagMessage( unsigned char  , int ); //	Inizializza un messaggio di tipo DiagMessage

IDT_Message* init_IDT_Message( unsigned char  , char * , int , char * , int ); // Inizializza un messaggio di tipo IDT

PR_Message* init_PR_Message( unsigned char  , char * , char * , int , int , char *); //	Inizializza un messaggio di tipo PR

void set_PR_Message(PR_Message * , unsigned char  , char * , char * , int , int , char *);//	modifica un messaggio di tipo PR

int bootServer(in_port_t * , int ); //bootstrap della connesione lato server

TrMessage* init_transaction_msg(unsigned char , int  , int  , int ,  int , int ); // Inizializza un messagio di tipo Transaction Message

void set_transaction_msg(TrMessage* , unsigned char , int  , int  , int ,  int , int ); //	Modfica un messaggio di tipo Transaction,
