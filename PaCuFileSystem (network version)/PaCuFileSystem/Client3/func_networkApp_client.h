
BootMessage* init_bootmessage(unsigned char) ;// inizializza un messaggio di tipo BootMessage

void set_bootmessage(BootMessage * , unsigned char ); // modifica un messaggio di tipo BootMessage

InitMessage* init_InitMessage( unsigned char  , char * , int ,int ,int ); // Inizializza un messaggio di tipo InitMessage

DiagMessage* init_DiagMessage( unsigned char  , int ); //	Inizializza un messaggio di tipo DiagMessage

BootAM* init_boot_am(unsigned char , char * , int , char * , int , char * , int ); //	Inizializza un messaggio di tipo BootAM

TrMessage* init_transaction_msg(unsigned char , int  , int  , int ,  int , int ); // Inizializza un messagio di tipo Transaction Message

void set_transaction_msg(TrMessage* , unsigned char , int  , int  , int ,  int , int ); //	Modfica un messaggio di tipo Transaction,

int connection_serverBoot() ; //comunicazione client-BootServer


