

#define _DEBUG 1 // 1: debug attivato 0: debug disattivato

#define _DIM_MD5 16 //lunghezza md5

#define PATH_DB_CONF "DB/Conf"
#define PATH_DB_PRIMARY "DB/Primary"


#include "../framework/framework.h"

#define _MAX_STRING_ERROR _NW_DIM_ERROR // la lunghezza massima della stringa errore
#define _DIM_IPV4 _DIM_IP // lunghezza char ip, comprende il terminatore di stringa

#include "func_networkAppBoot.h"
#include "dispatcher_servboot.h"
#include "general_functions.h"





Semaphore *sem_accept;	// mutex per il lock sull'accept
Semaphore *sem_exit_disp; // signal per la terminazione del dispatcher_serverboot
short int exit_helper;//varibile per segnalare l'uscita agli helper del disptacher boot

in_port_t *port_client; // numero di porta per la comunicazione client-server
in_port_t *port_am; // numero di porta per la comunicazione amministratore-server
int backlog_client; // backlog 
int thread_helper; // numero helper del dispatcher
int max_line; // la dimensione massima dei blocchi per write/read su file
double version_dom; // versione domini 
double version_serv; // versione server
char *list_primary; // lista dei server primary
//short int exit_server; // settata a 1 termina il server
unsigned int time_helper; // il tempo in secondi che deve aspettare il dispatcher dopo l'avviso di chiusura per forzare la terminazione degli helper
char* color; // colore dei campi nome





short int timeout_ping; // tempo in sec di timeout per l'invio/ricezione del ping
short int timeout_pong; // tempo in sec di timeout per l'invio/ricezione del pong
short int timeout_error; // tempo in sec di timeout per l'invio/ricezione del messaggio error
short int timeout_req_list; // tempo in sec di timeout per la ricezione della richiesta lista primary
short int timeout_send_servlist; // tempo in sec di timeout per l'invio della lista primary
short int timeout_am;   //tempo in sec di timeout per le operazioni amministratore 

#ifndef _THREAD_SAFE
#define _THREAD_SAFE
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif
