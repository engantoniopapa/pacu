

#define _DEBUG 1 // 1: debug attivato 0: debug disattivato
#define _DIM_MD5 16 //lunghezza md5
#define _DIM_PORT 6 // lunghezza char porta, comprende il terminatore di stringa
#define _DIM_COND 150
#define _DIM_QUERY_COND 300
#define _MAX_COUNT 1000000000
#define _LIST_PP 50 // lunghezza buffer per la lista pong UDP nella comunicazione client - server


#define PATH_DB_CONF "DB/Conf"
#define PATH_DB_DEFCONF "DB/ConfDefault"
#define PATH_DB_PRIMARY "DB/Primary"
#define PATH_DB_TMP_PRI "DB/temp_Primary"
#define PATH_DB_DOMINI "DB/Domini"
#define PATH_DB_TMPDOM "DB/temp_Domini"
#define PATH_GA_TXT "Download/tmp/~Ga_TXT"
#define PATH_DB_LOG "DB/Logfile"

#include "../framework/framework.h"

#define _MAX_STRING_ERROR _NW_DIM_ERROR // la lunghezza massima della stringa errore
#define _DIM_IPV4 _DIM_IP  // lunghezza char ip, comprende il terminatore di stringa
#define _TENTATIVI_PP 3 // numero di prove del ping-pong

#include "func_networkApp_client.h"
#include "general_functions.h"
#include "menu.h"
#include "boot_fase.h"
#include "am_fase.h" 
#include "init_fase.h"
#include "console.h"
#include "transaction.h"
#include "ppUdp.h"



char *in_path; // path dove vengono scaricati i file
char *tmp_path; // path dove vengono salvati i file temporanei per la lettura

short int timeout_am; //timeout connessione amministratore;
short int timeout_boot; // timeout per il client in fase di bootstrap
short int timeout_init_msg; // tempo in sec di timeout per l'invio/ricezione degli InitMessage
short int timeout_domlist; // tempo in sec di timeout per l'invio/ricezione della lista domini- primary
short int timeout_tr; // tempo in sec di timeout per l'invio/ricezione delle transazioni
int max_line; // la dimensione massima dei blocchi per write/read su file
int tr_max_cap; // numero massimo di capitoli che si possono modificare

double version_dom; // versione domini 
double version_serv; // versione server

ListPong *listCl_pp; // lista dei pong UDP ricevuti, nella comunicazione client-server
Two_rooms *sem_listCl_pp; //struttura di sincronizzazione sulla lista dei pong UDP ricevuti, nella comunicazione client-server
Two_rooms *sem_count_pp; //struttura di sincronizzazione sul counter id del pong 
short int exit_th_pp; //varibile per segnalare l'uscita ai thread UDP
int id_pp; // id per i ping UDP
short int timeout_pp; //tempo stimato di RTT per il ping-pong
int *port_client_pp; // numero di porta per la comunicazione client-server relativa allo scambio ping-pong
short int time_helper_pp; // il tempo in secondi che deve aspettare il dispatcher dopo l'avviso di chiusura per forzare la terminazione degli helper

char my_ip[_DIM_IP]; //proprio ip
char *iterface_network; // interfaccia di rete a cui si è connessi

char* color; // colore dei campi nome
short int tentativi_boot; // numeri di tentativi nella fase di boot
double server_v_dom; //versione domini del server di boot
double server_v_primary; // versione primary del server di boot
Server *list_server ; // lista dei server Primary-Replica del sistema
int len_list_server ; // lunghezza list_server
Dominio *list_domini ; // lista dei domini
int len_list_domini ; // lunghezza list_domini


#ifndef _THREAD_SAFE
#define _THREAD_SAFE
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif
