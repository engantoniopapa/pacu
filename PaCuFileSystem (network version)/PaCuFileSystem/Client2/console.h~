

#define _DIM_SHELL_COM _DIM_RES+11
#define _DIM_PATH_USER _DIM_RES+30
#define _DIM_SHELL_RES _DIM_RES+1
#define _DIM_SHELL_DOM _DIM_DOM +3
#define _MAX_ARG 5

//-------- operazioni transaction ----
#define _SHELL_UPLOAD _TR_UPLOAD 
#define _SHELL_RM _TR_RM
#define _SHELL_MKDIR _TR_MKDIR
#define _SHELL_NEWGA _TR_NEW_GA
#define _SHELL_NEWCAP _TR_NEW_CAP
#define _SHELL_MVCAP _TR_MV_CAP
#define _SHELL_MODCAP _TR_MOD_CAP
#define _SHELL_RMCAP _TR_RM_CAP
#define _SHELL_RMDIR _TR_RM_DIR


struct Shell_El
{
	char ip[_DIM_IPV4]; //ip server a cui connettersi
	int port;	// port server a cui connettersi

	char path_user[_DIM_PATH_USER];  // path console
	char path_res[_DIM_SHELL_RES]; // percorso risorsa
	char path_dom[_DIM_SHELL_DOM]; // percorso dominio
};
typedef struct Shell_El Shell_El;


struct TokenArgs
{
	int l_arg;
	char args[_MAX_ARG][_DIM_SHELL_RES];
};
typedef struct TokenArgs TokenArgs;

Shell_El* create_Shell_El(); // alloca dinamicamente una struttura Shell_El

void reset_Shell_El(Shell_El*); // resetta una struttura Shell_El

TokenArgs* create_TokenArgs(); // alloca dinamicamente una struttura TokenArgs

void reset_TokenArgs(TokenArgs*); // resetta una struttura Shell_El

int start_console();// fa partire la console

void stampa_comandi_user(); // stampa i comandi user

int operation_user(Shell_El * , TokenArgs*); // operazioni user

void set_path_userConsole(char *,  char *); // setta il path console

int parsing_user(char *, TokenArgs * ); //  parsing dei comnadi dell'utente

void stampa_elencoDom(); //stampa la lista domini

int op_mount(char * , Shell_El * ); // operazione di mount del dominio

int op_cd(char * , Shell_El *); // operazione di cd

int op_ls(char * , Shell_El *); // operazione ls

int op_search(char * , char * , Shell_El *); // operazione ls

int op_read_save(char * , Shell_El * , int) ; // operazione di lettura della risorsa

int op_readga(char * , char *); //operazione di lettura su un file di tipo ga

int op_transaction(int , TokenArgs * , Shell_El * , char *); //operazione generica di scrittura sul server che avvia una transazione
