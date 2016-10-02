#define LOG_ABORT 0
#define LOG_PREPARA 1
#define LOG_PRONTO 2
#define LOG_COMMIT 3

#define LOG_QUERY 2000
#define LOG_RES 1024
#define LOG_IP 16




struct UnitLog
{
	char res2[LOG_RES];
	char res3[LOG_RES];
	int opt1;
	int opt2;
	short int state;	
};
typedef struct UnitLog UnitLog;

struct TrLog
{
	int id_p;
	int id_c;
	char ip[LOG_IP];
	int port;
	short int type;
	char local_res[LOG_RES];
	UnitLog* list;
	int length;
	
};
typedef struct TrLog TrLog;



void set_TrLog( TrLog * , int  , int  , short int  , char * , int , char * , UnitLog*  , int ); // Setta i valori della struttura TrLog

void remove_det_TrLog( TrLog *); // Rimuove la struttura TrLog

void remove_list_TrLog( TrLog * , int); // Rimuove la lista di strutture TrLog

int count_log(DB*  , char*  , char* );// conta i record log dal Db

int insert_log(char * , int  , int  ,char * , int ,short int , char * , char * , char * , int , int , short int); // inserisce un record nella tabella dei log

int update_log(char * , int  , int  , short int ); // aggiorna i record della tabella log file

int update_log_state(char * , short int   , short int ); // aggiorna i record della tabella log file

int update_log_opt(char *, int , int , short int, int , int ); // aggiorna i record della tabella log file

int update_log_res1(char * , int  , int , short int , char* ); // aggiorna i record della tabella log file

int update_log_res2(char * , int  , int , short int, char* , char* ); // aggiorna i record della tabella log file

int update_log_res3(char * , int  , int , short int , char* , char* , char*); // aggiorna i record della tabella log file

int update_log_res2res3(char * , int, int , short int, char* , char* , char* ); // aggiorna i record della tabella log file

int delete_log(char * , int  , int ); // elimina i record della tabella log file

int delete_all_log(char *); // elimina tutti i record della tabella log file

int callback_select_recTR(void* , int , char **, char **);

TrLog *select_recTR(char * , char *, char * , char * ,char * , int *); // seleziona le transazioni durante la procedura di recovery

int max_numTR(char *  ,char * , int  ); // trova la transazione relativa al server con id + grande

int callback_select_detTR(void* , int , char **, char **);

TrLog *select_detTR(char * , char *, int  , int , short int  , char * , int , char * ,  char * , int *); // ottieni i dettagli sulla transazione



