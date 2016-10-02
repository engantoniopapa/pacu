#include "Sqlite/sqlite3.h"
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define _NW_DIM_ERROR 1600

//Struttura contente il descrittore sqlite3 del DB
struct object_db
{
	sqlite3 *db ;
};
typedef struct object_db DB;



//Struttura usata per tenere memoria del risultato sulla callback_am()
struct object_result_query
{
	int l_row;
	void *object;
	int  count;
};
typedef struct object_result_query ResultQuery;


// struttura di collegamento con la networkApp 
struct L_Server
{
	char name[31];
	char ip[16];
	int port;	
};
typedef struct L_Server L_Server;



// struttura di collegamento con la networkApp 
struct L_Dominio
{
	char name[31];
};
typedef struct L_Dominio L_Dominio;


int open_db_ro( DB* , char* , char* );// funzione di connessione al DB in sola lettura
int open_db_rw(  DB* , char*  , char* );// funzione di connessione al DB in lettura&Scrittura

int close_db(DB*  , char* );// funzione di chiusura del DB

int insert_db(DB* , char*  , char* );// insert sul DB
int count_tb(DB*  , char*  , char*  , char* ); // count su una tabella
int delete_db(DB*  , char*  , char* );// delete sul DB
int generic_op_db(DB* , char* , char*); //generica operazione sul DB (Amministratore)

int callback_am(void* , int , char ** , char **);// funzione usata per eleborare ogni risultato della funzione sqlite3_exec() (bootserver)
char* select_db_am(DB* , char*  , char* , int* );// select sul DB ritorna una stringa da stampare (bootserver)

int callback_listserver(void* , int , char **, char **); // funzione usata per eleborare ogni risultato della funzione sqlite3_exec() (bootserver)
char* select_listserver(DB* , char*  , char* , int* );// select sul DB ritorna una stringa formattata da inviare al client (bootserver)

int callback_dom_serv(void* , int , char **, char **);// funzione usata per eleborare ogni risultato della funzione sqlite3_exec()
char* select_dom_serv(DB* , char* , char* , int* ); // select sul DB per creare la stringa dom-server

int callback_sel(void* , int , char ** , char **);// funzione usata per eleborare ogni risultato della funzione sqlite3_exec() (client)
char* select_tb(DB* , char*  , char* , int* );// select su una tabella ritorna una stringa da stampare (client)

int select_conf_param(DB* , char*  , char* , int* );// select sul DB usato per la lettura dei parametri di configurazione

char* select_conf_charparam(DB* , char* , char* , int* ); // select sul DB par la lettura dei parametri di configurazione tipo char

void select_conf_vers(DB*  , double*  , double*  , char* );// select sul DB usato per la lettura della versione

int select_server_NameId(DB*  , char* , int* , char* ); // select sul DB per la lettura del nome del server e dell'id

int select_password_am(DB * , char*  , char * , int ); // select sul DB per la lettura della password amministratore (bootserver)

int select_boot_server(DB*  , char * ,  uint16_t * , int); //  Select sul DB per la lettura delle informazioni sul BootServer (client)

int select_IpPort_server(DB*  , char * , char* ,  int* , char *); //  Select sul DB per la lettura dell'ip e della porta del server

int select_nameServer_dominio(DB*  , char * , char * );//  Select sul DB per la lettura del primary relativo al dominio

int info_primary_dominio(DB* , char* , char* , char* , int*); //select sul DB per trovare il primary di un dominio

int callback_bufferServer(void*, int, char **, char **);// funzione usata per eleborare ogni risultato della funzione sqlite3_exec() (server)
int select_on_bufferServer(DB* , char*  , ResultQuery*, char*  ); //select di una tabella memorizzata in una struttura Server (server)

int callback_bufferDomini(void*, int, char **, char **);// funzione usata per eleborare ogni risultato della funzione sqlite3_exec() (server)
int select_on_bufferDomini(DB* , char*  , ResultQuery*, char*  ); //select di una tabella memorizzata in una struttura Server (server)

