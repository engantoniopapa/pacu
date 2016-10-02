

#define _DEBUG 1 // 1: debug attivato 0: debug disattivato
#define _DIM_MD5 16 //lunghezza md5
#define _MAX_COUNT 1000000000
#define _L_PP_CL 400 // lunghezza buffer per la lista pong UDP nella comunicazione client- server
#define _L_PP_SERV 50 // lunghezza buffer per la lista pong UDP nella comunicazione server - server

#define PATH_DB_CONF "DB/Conf"
#define PATH_DB_PRIMARY "DB/Server"
#define PATH_DB_DOMINI "DB/Domini"
#define PATH_DB_TMPDOM "DB/temp_Domini"
#define PATH_DB_LOG "DB/Logfile"



#include "../framework/framework.h"

#define _MAX_STRING_ERROR _NW_DIM_ERROR // la lunghezza massima della stringa errore
#define _DIM_PATH_FILESYSTEM _DIM_RES+1 // lunghezza massima del path del filesystem
#define _DIM_IPV4 _DIM_IP // lunghezza char ip, comprende il terminatore di stringa
#define _TENTATIVI_PP 3 // numero di prove del ping-pong

#include "func_networkApp_server.h"
#include "dispatcher.h"
#include "general_functions.h"
#include "init_fase.h"
#include "transaction.h"
#include "ppUdp.h"


Semaphore *sem_accept_c;	// mutex per il lock sull'accept client
Semaphore *sem_accept_s;	// mutex per il lock sull'accept server

Semaphore *sem_accept_ppCl;	// mutex per il lock sulla recv dei pong client
Semaphore *sem_accept_ppSrv;	// mutex per il lock sulla recv dei pong server

Semaphore *sem_exit_disp_s; // signal per la terminazione del dispatcher per i server
Semaphore *sem_exit_disp_c; // signal per la terminazione del dispatcher per il client
Semaphore *sem_exit_disp_ppS; // signal per la terminazione del dispatcher ping-pong per i server
Semaphore *sem_exit_disp_ppC; // signal per la terminazione del dispatcher ping-pong per il client

Two_rooms *sem_count; //struttura di sincronizzazione sul counter id
Two_rooms *sem_count_pp; //struttura di sincronizzazione sul counter id del pong 

Two_rooms *sem_listCl_pp; //struttura di sincronizzazione sulla lista dei pong UDP ricevuti, nella comunicazione client-server
Two_rooms *sem_listServ_pp; //struttura di sincronizzazione sulla lista dei pong UDP ricevuti, nella comunicazione server-server
ListPong* listCl_pp; // lista dei pong UDP ricevuti, nella comunicazione client-server
ListPong* listServ_pp; // lista dei pong UDP ricevuti, nella comunicazione server-server

char *iterface_network; // interfaccia di rete a cui si è connessi

short int exit_helper_c; //varibile per segnalare l'uscita agli helper del disptacher client
short int exit_helper_s; //varibile per segnalare l'uscita agli helper del disptacher server
short int exit_th_pp; //varibile per segnalare l'uscita ai thread UDP

short int time_helper_c; // il tempo in secondi che deve aspettare il dispatcher dopo l'avviso di chiusura per forzare la terminazione degli helper
short int time_helper_s; // il tempo in secondi che deve aspettare il dispatcher dopo l'avviso di chiusura per forzare la terminazione degli helper
short int time_helper_pp; // il tempo in secondi che deve aspettare il dispatcher dopo l'avviso di chiusura per forzare la terminazione degli helper

char *tmp_path; // path dove vengono salvati i file temporanei per la lettura
char *fs_path; // path del filesystem

in_port_t *port_client; // numero di porta per la comunicazione client-server
in_port_t *port_server; // numero di porta per la comunicazione primary-repliche
int *port_client_pp; // numero di porta per la comunicazione client-server relativa allo scambio ping-pong
int *port_server_pp; // numero di porta per la comunicazione server-server relativa allo scambio ping-pong


int backlog_client; // backlog 
int backlog_server; // backlog 
int thread_helper_c; // numero helper del dispatcher per i client
int thread_helper_s; // numero helper del dispatcher per i server
int thread_ppCl; // numero dei thread per la gestione dei ping-pong provenienti dai client
int thread_ppSrv; // numero dei thread per la gestione dei ping-pong provenienti dai server
int max_line; // la dimensione massima dei blocchi per write/read su file
unsigned int time_helper; // il tempo in secondi che deve aspettare il dispatcher dopo l'avviso di chiusura per forzare la terminazione degli helper
char* color; // colore dei campi nome
char* name_server; // nome del server;
int id_server;
char my_ip[_DIM_IP]; //proprio ip

Server *list_server ; // lista dei server Primary-Replica del sistema (escluso se stesso)
int len_list_server ; // lunghezza list_server
char * string_dom_serv; // stringa formattata domini - server primary 

int id_count; // id per le transazioni
int id_pp; // id per i ping UDP
short int timeout_init_msg; // tempo in sec di timeout per l'invio/ricezione degli InitMessage
short int timeout_domlist; // tempo in sec di timeout per l'invio/ricezione della lista domini- primary
short int timeout_tr; // tempo in sec di timeout per la transazione
short int timeout_pp; //tempo stimato di RTT per il ping-pong


pthread_mutex_t list_mutex;
regions *lista_File;

Writers_Reader *sem_vista; // semaforo per la lettura scrittura della lista domini;

#ifndef _THREAD_SAFE
#define _THREAD_SAFE
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif
