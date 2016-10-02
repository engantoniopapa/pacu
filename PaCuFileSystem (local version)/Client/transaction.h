
struct Id_Trans
{
	int id_p; //id primary
	int id_c; //id count
	int msg_count; // count message
};
typedef struct Id_Trans Id_Trans;


int init_transaction(int , char * , char * , int , int , int , Shell_El* ,char * ); // inizia la transazione

int ctrl_errorTR(int , TrMessage * , Shell_El * , char * ); // gestisce il tipo di errore

int opTr_upload(int , char * , Id_Trans * , char * , Shell_El * , char *); // transazione per l'upload di un file

int opTr_remove(int , char * , Id_Trans * , Shell_El * , char *); // transazione per il remove di un file

int opTr_mkdir(int , char * , Id_Trans * , Shell_El * , char *); // transazione per la crezione di una directory

int opTr_newGa(int , char * , Id_Trans * , Shell_El * , char *); // transazione per la crezione di un file GA

int opTr_newCap(int , char * , Id_Trans * , Shell_El * , int , char *); // transazione per la crezione di un capitolo in un file GA

int opTr_mvCap(int , char * , char * , Id_Trans * , Shell_El * , int, int , char *); // transazione per lo spostamento  di un capitolo in un file GA

int opTr_removeCap(int , char * , char * , Id_Trans * , Shell_El * , int , char *); // transazione per il remove di un capitolo di un file GA

int opTr_removeDir(int , char * , Id_Trans * , Shell_El * , char *); // transazione per il remove di una directory
