#define LOG_ABORT 0
#define LOG_PREPARA 1
#define LOG_PRONTO 2
#define LOG_COMMIT 3

#define LOG_QUERY 1024
#define LOG_RES 1024





struct UnitLog
{
	char res2[LOG_RES];
	short int state;	
};
typedef struct UnitLog UnitLog;

struct TrLog
{
	int id_p;
	int id_c;
	unsigned char type;
	char local_res[LOG_RES];
	UnitLog* list;
	int length;
	
};
typedef struct TrLog TrLog;



void set_TrLog( TrLog * , int  , int  , unsigned char  , char * , UnitLog*  , int ); // Setta i valori della struttura TrLog

int count_log(DB*  , char*  , char* );// conta i record log dal Db

int insert_log(char * , int  , int  , unsigned char  , char * , char * , short int); // inserisce un record nella tabella dei log,

int update_log(char * , int  , int  , unsigned char  , short int ); // aggiorna i record della tabella log file

int callback_select_recTR(void* , int , char **, char **);

TrLog *select_recTR(char * , char *, char * , char * ,char * , int *); // seleziona le transazioni durante la procedura di recovery

int max_numTR(char *  ,char * , int  ); // trova la transazione relativa al server con id + grande

