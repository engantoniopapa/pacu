

struct ContainerTr1
{
	InitMessage *msg_i;
	Id_Trans *id_t ;
	unsigned char type ;
	char ip[_DIM_IP];
	int fd ; 
	int state; // 1 invio commit alle repliche , 0 invio abort alle repliche , -1 errore di connessione
	int consd;
};
typedef struct ContainerTr1 ContainerTr1;

struct ContainerTr2
{
	InitMessage *msg_i;
	Id_Trans *id_t;
	unsigned char type ;
	char ip[_DIM_IP];
	int state; // 1 invio commit alle repliche , 0 invio abort alle repliche , -1 errore di connessione
	int consd;
};
typedef struct ContainerTr2 ContainerTr2;

struct ContainerTr3
{
	InitMessage *msg_i;
	Id_Trans *id_t;
	unsigned char type ;
	char ip[_DIM_IP];
	List_ModCap* list_cap ; 
	int len_cap ;
	int state; // 1 invio commit alle repliche , 0 invio abort alle repliche , -1 errore di connessione
	int consd;
};
typedef struct ContainerTr3 ContainerTr3;



int op_rep_tr1(InitMessage * ,  unsigned char , Server * , int , int ,Id_Trans *);
void *th_rep_tr1(void *);

int op_rep_tr2(InitMessage * ,  unsigned char  , Server * , int , Id_Trans *);
void *th_rep_tr2(void *);

int op_rep_tr3(InitMessage * ,  unsigned char , Server * , int , List_ModCap* list_cap , int len_cap ,Id_Trans *);
void *th_rep_tr3(void *);

void *th_rep_send_response1(void *);// thread preposto all'invio di un commit o di un abort alla replica
void *th_rep_send_response2(void *);// thread preposto all'invio di un commit o di un abort alla replica
void *th_rep_send_response3(void *);// thread preposto all'invio di un commit o di un abort alla replica

int opRep_upload(int  , char * , Id_Trans * , int , int ); // operazione per invio di una risorsa su una replica
int opRep_generic(int , unsigned char , char * , Id_Trans * , int );// operazione per l'invio di _TR_RM, _TR_MKDIR, _TR_RM_DIR su una replica 
int opRep_modcap(int , char *, Id_Trans *, List_ModCap* , int , int );// operazione per l'invio di TR_MODCAP  su una replica

int ctrl_messagePR(int connsd); //gestione dei messaggi PR
