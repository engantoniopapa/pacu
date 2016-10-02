#include "header.h"



//	Inizializza un messaggio di tipo DiagMessage, ritorna: il puntatore al messaggio creato
//	Parametri-> descrittore di payload del messaggio , campi opzionali
DiagMessage* init_DiagMessage( unsigned char c , int param)
{
	DiagMessage *b;
	b = (DiagMessage *) calloc( 1 , sizeof(DiagMessage) );

	b->pay_desc = c ;
	b->param_state = param;
	
	return b;
}



//	Inizializza un messaggio di tipo IDT, ritorna: il puntatore al messaggio creato
//	Parametri-> descrittore di payload del messaggio ,ed altri campi del pacchetto
IDT_Message* init_IDT_Message( unsigned char c , char *name , int l_name , char *ip , int port)
{
	IDT_Message *b;
	b = (IDT_Message *) calloc( 1 , sizeof(IDT_Message) );

	b->pay_desc = c ;
	strncpy(b->name ,name , l_name) ;
	b->l_name = l_name;
	strncpy(b->ip , ip , _DIM_IPV4);
	b->port = port;
	
	return b;
}



//	Inizializza un messaggio di tipo PR, ritorna: il puntatore al messaggio creato
//	Parametri-> descrittore di payload del messaggio ,ed altri campi del pacchetto
PR_Message* init_PR_Message( unsigned char c , char *server , char *dom , int opt1 , int opt2 , char* opt3)
{
	PR_Message *b;
	b = (PR_Message *) calloc( 1 , sizeof(PR_Message) );

	b->pay_desc = c ;
	
	memset(b->server , '\0' , _DIM_NAME_SERVER);
	strncpy(b->server ,server , _DIM_NAME_SERVER-1) ;
	
	memset(b->dom , '\0' , _DIM_DOM);
	strncpy(b->dom ,dom , _DIM_DOM-1) ;
	
	b->opt1 = opt1;
	b->opt2 = opt2;
	
	memset(b->opt_char , '\0' , _DIM_NAME_SERVER);
	strncpy(b->opt_char ,opt3 , _DIM_NAME_SERVER-1) ;
	
	
	return b;
}



//	modifica un messaggio di tipo PR, ritorna: il puntatore al messaggio creato
//	Parametri-> descrittore di payload del messaggio ,ed altri campi del pacchetto
void set_PR_Message(PR_Message *b , unsigned char c , char *server , char *dom , int opt1 , int opt2 , char *opt3)
{
	b->pay_desc = c ;
	memset(b->server , '\0' , _DIM_NAME_SERVER);
	strncpy(b->server ,server , _DIM_NAME_SERVER-1) ;
	
	memset(b->dom , '\0' , _DIM_DOM);
	strncpy(b->dom ,dom , _DIM_DOM-1) ;
	
	b->opt1 = opt1;
	b->opt2 = opt2;
	
	memset(b->opt_char , '\0' , _DIM_NAME_SERVER);
	strncpy(b->opt_char ,opt3 , _DIM_NAME_SERVER-1) ;
}



//	Inizializza un messaggio di tipo Transaction, ritorna: il puntatore al messaggio creato
//	Parametri-> descrittore di payload del messaggio, i vari campi del tipo di messaggio
TrMessage* init_transaction_msg(unsigned char p , int id_pr , int id_c , int count ,  int opt1 , int opt2)
{
	TrMessage *msg;
	msg = (TrMessage *) calloc( 1 , sizeof(TrMessage) );

	msg->pay_desc = p;

	if(p == _TR_DO_ID || p ==  _TR_COMMIT || p == _TR_ABORT || p == _TR_MKDIR || p == _TR_UPLOAD || p == _TR_NEW_GA || p == _TR_RM || p == _TR_ERROR || 
		 p == _TR_NEW_CAP || p == _TR_RM_CAP || p == _TR_MV_CAP || p == _TR_MOD_CAP || p == _TR_RM_DIR )
	{
		msg->id_primary = id_pr ;
		msg->id_count = id_c ;
		msg->msg_count = count ;
		msg->opt1 = opt1 ;
		msg->opt2 = opt2 ;
	} 

	return msg;
}



//	modfica un messaggio di tipo Transaction, ritorna: il puntatore al messaggio creato
//	Parametri-> descrittore di payload del messaggio, i vari campi del tipo di messaggio
void set_transaction_msg(TrMessage* msg , unsigned char p , int id_pr , int id_c , int count ,  int opt1 , int opt2)
{
	
	memset(msg , '0' , sizeof(TrMessage) );

	msg->pay_desc = p;

	if(p == _TR_DO_ID || p ==  _TR_COMMIT || p == _TR_ABORT || p == _TR_MKDIR || p == _TR_UPLOAD || p == _TR_NEW_GA || p == _TR_RM || p == _TR_ERROR || 
		 p == _TR_NEW_CAP || p == _TR_RM_CAP || p == _TR_MV_CAP || p == _TR_MOD_CAP || p == _TR_RM_DIR )
	{
		msg->id_primary = id_pr ;
		msg->id_count = id_c ;
		msg->msg_count = count ;
		msg->opt1 = opt1 ;
		msg->opt2 = opt2 ;
	} 

}



// Bootstrap della connessione lato server, ritorna: il descrittore socket per mettersi in ascolto sulla porta
//	Parametri-> puntatore alla porta , e lunghezza coda di backlog
int bootServer( in_port_t *port , int backlog) 
{
	int listensd;
	struct sockaddr_in servaddr;
	
	if((listensd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("errore in socket");
		exit(-1);
	}

	memset((void*)&servaddr,0,sizeof(servaddr));

	addr_init(&servaddr, port ,INADDR_ANY);

	if(bind(listensd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		perror("errore in bind");
		exit(-1);
	}
	
	puts("bind effettuato con successo");
	
	if(listen(listensd,backlog)<0)
	{
		perror("errore in listen");
		exit(-1);
	}

	puts("listen effettuata con successo");
	return listensd;
}

