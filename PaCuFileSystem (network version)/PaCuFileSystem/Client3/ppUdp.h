
#define CHOICE_LIST 3
#define DIF_TIME 20
#define WAIT_PP 4

struct UnitPong
{
	int id;
	time_t time_stamp;	
};
typedef struct UnitPong UnitPong;

// struttura della lista dei pong UDP ricevuti 
struct ListPong
{
	int used;
	int length;
	UnitPong *list[CHOICE_LIST];
};
typedef struct ListPong ListPong;


struct Unit_TH_Pong
{
	ListPong *list;	
	Two_rooms *sem;
	char *ip;
	int port;	
};
typedef struct Unit_TH_Pong Unit_TH_Pong;



PPMessage *create_PPmessage(); //crea un messaggio di tipo PPMessage

PPMessage *init_PPmessage(unsigned char , int , char * , int	); // inizializza un messaggio di tipo PPMessage

void set_PPmessage(PPMessage * , unsigned char , int , char * , int ); // setta un messaggio di tipo PPMessage

void set_Unit_TH_Pong(Unit_TH_Pong * , ListPong *, Two_rooms * , char * , int ); // setta una struttura di tipo Unit_TH_Pong

ListPong* init_listPong( int ); // inizializza la struttura della lista dei pong UDP ricevuti

void remove_listPong( ListPong *); // dealloca la struttura della lista dei pong UDP ricevuti

int reset_age_bufferPong( ListPong * ); // resetta un buffer + vecchio della struttura della lista dei pong UDP ricevuti

int isID_listPong( ListPong * , int ); // verifica se un id è presente nella struttura della lista dei pong UDP ricevuti

int searchID_listPong( ListPong *  , int , int * , int *); // cerca un id nella struttura della lista dei pong UDP ricevuti, 

int insert_listPong(ListPong * , Two_rooms * , int ); // inserisci un id nella struttura della lista dei pong UDP ricevuti

int get_listPong(ListPong *  , Two_rooms * , int ); // ritira un id dalla lista dei pong UDP ricevuti

int send_PPmessage(PPMessage * , char* , int  ); // invia un messaggio UDP di tipo PPmessage

int recv_PPmessage(PPMessage * , int ); // riceve un messaggio UDP di tipo PPmessage

int isAlive_pp(ListPong * , Two_rooms * , int , int , char* , char*, int );// invio ping e controllo pong

void *thread_rcvPP_UDP(void * ); // funzione thread per la ricezione dei ping-pong
