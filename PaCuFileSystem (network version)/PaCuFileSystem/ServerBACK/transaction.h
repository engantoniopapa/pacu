

struct Id_Trans
{
	int id_p; //id primary
	int id_c; //id count
	int msg_count; // count message
};
typedef struct Id_Trans Id_Trans;


struct List_ModCap
{
	int id_cap;
	char path_mod_cap[_DIM_RES];
	char path_sav_cap[_DIM_RES];
};
typedef struct List_ModCap List_ModCap;


int start_transaction(int  , InitMessage *); // avvia la transazione lato server

int transaction_upload(int , Id_Trans * , InitMessage * , unsigned int , char *); // operazione di upload file

int transaction_remove(int , Id_Trans * , InitMessage * , char * ); // operazione di remove file

int transaction_mkdir(int  , Id_Trans * , InitMessage * , char *); // operazione di creazione di una directory

int transaction_new_fileGa(int  , Id_Trans *, InitMessage * , char *); // operazione di creazione di un file GA

int transaction_newCap(int , Id_Trans * , InitMessage *  , int  , int , char *); // operazione di inserimento di un capitolo in un file GA

int transaction_moveCap(int  , Id_Trans * , InitMessage *  , int  , int , char * ); // operazione di move sul capitolo del file Ga

int transaction_removeCap(int  , Id_Trans * , InitMessage *  , int , char * ); // operazione di remove directory

int transaction_removeDir(int , Id_Trans * , InitMessage * , char * ); // operazione di remove directory

int transaction_modCap(int  , Id_Trans *, InitMessage *  , int  , int  , char *);// operazione di modifica capitoli in un file GA

List_ModCap *init_List_ModCap( int  , char [][_DIM_RES],  int *); // Inizializza la struttura List_ModCap, e la ordina per avere id_cap crescenti 

int redo_mod_cap( List_ModCap * , int  , char * ); // effettua la redo dell'operazione di mod_cap
