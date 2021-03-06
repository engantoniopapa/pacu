

// ----- Payload Descriptor------
#define _BOOT_PING 1 // 0x01 Bootstrap Ping
#define _BOOT_PONG 2 // 0x02 Bootstrap Pong
#define _BOOT_LIST 3 // 0x03 Bootstrap List Primary
#define _BOOT_ERROR 4 // 0x04 Bootstrap Error
#define _BOOT_LOGIN_AM 5 // 0x05 Bootstrap Login Amministrator
#define _BOOT_EXIT_AM 6 // 0x06 Bootstrap Exit Amministrator
#define _BOOT_USE_DB_AM 7 // 0x07 Bootstrap Use DB Amministrator
#define _BOOT_SELECT_AM 8 // 0x08 Bootstrap Select Amministrator
#define _BOOT_GE_OP_AM 9 // 0x09 Bootstrap Generic Operation Amministrator
#define _BOOT_ERROR_AM 10 // 0x10 Bootstrap Error Amministrator
#define _BOOT_CHANGE_PSW_AM 11 // 0x11 Bootstrap Change Password Amministrator
#define _BOOT_LOGOUT_AM 12 // 0x12 Bootstrap Logout Amministrator
#define _BOOT_COMMIT_AM 13 // 0x13 Bootstrap Commit Amministrator

#define _PING_MESS_UDP 25 // 0x25 Ping Message UDP
#define _PONG_MESS_UDP 26 // 0x26 Pong Message UDP

#define _INIT_UPDATE_DOM 30 // 0x25 InitMessage Update Lista Domini
#define _INIT_START_TR 31 // 0x31 InitMessage Start Transaction
#define _INIT_LS 32 // 0x32 InitMessage List 
#define _INIT_CD 33 // 0x33 InitMessage Change Directory
#define _INIT_SEARCH 34 // 0x34 InitMessage Search Resource
#define _INIT_READ_RESOURCE 35 // 0x34 InitMessage Read Resource
#define _INIT_STATE_TR 36 // 0x36 InitMessage State Transaction
#define _INIT_WHOIS_PR 37 // 0x36 InitMessage Who Is Primary

#define _TR_DO_ID 41 // 0x41 TransactionMessage Do Id
#define _TR_COMMIT 42 // 0x42 TransactionMessage Commit
#define _TR_ABORT 43 // 0x43 TransactionMessage Abort
#define _TR_MKDIR 44 // 0x44 TransactionMessage Create Directory
#define _TR_UPLOAD 45 // 0x45 TransactionMessage Upload File
#define _TR_NEW_GA 46 // 0x46 TransactionMessage Create New GA File
#define _TR_RM 47 // 0x47 TransactionMessage Remove File
#define _TR_ERROR 48 // 0x48 TransactionMessage Error
#define _TR_NEW_CAP 49 // 0x49 TransactionMessage Create New Cap
#define _TR_RM_CAP 50 // 0x50 TransactionMessage Remove Cap
#define _TR_MV_CAP 51 // 0x51 TransactionMessage Move Cap
#define _TR_MOD_CAP 52 // 0x52 TransactionMessage Modify Cap
#define _TR_RM_DIR 53 // 0x53 TransactionMessage Remove Directory

#define _DIAG_ACK 61 // 0x61 DiagnosticMessage Ack
#define _DIAG_REP_CON 62 // 0x62 DiagnosticMessage Repeat Connect
#define _DIAG_ERROR 63 // 0x63 DiagnosticMessage Error
#define _DIAG_STATE_TR 64 // 0x64 DiagnosticMessage State Transaction

#define _IDT_PRIMARY 71 // 0x71 Identity Primary



#define _PR_BULLY_ELECT 91 // 0x91 Primary-Replica Bully Election
#define _PR_ACK 92 // 0x92 Primary-Replica Ack
#define _PR_BULLY_PRIMARY 93 // 093 Primary-Replica Bully New Primary
#define _PR_LIST_SERVDOM 94 // 0x94 Primary-Replica LIST Server-Dominio
#define _PR_LIST_SERVER 95 // 0x95 Primary-Replica LIST Server
#define _PR_ERROR 96 // 0x96 Primary-Replica ERROR
#define _PR_LOG_STATE 97 //0x97 Primary-Replica Log State
#define _PR_DEL_SERVDOM 98 // 0x98 Primary-Replica Delete Server-Dom
#define _PR_SET_LOG_STATE 99 // 0x99 Primary-Replica Log State

//------- Parametri DiagnosticMessage Error -------
#define _GENERIC_ERROR 0 // Errore generico
#define _BIZANTINE_ERROR 1 // Errore bizantino
#define _NO_DOM_ERROR 2 // Errore il primary non gestisce il dominio
#define _IGNORE_DOM_ERROR 3 // Errore il dominio è inesistente
#define _NO_RESOURCE 4 // Errore la risorsa cercata non esiste

//------- Parametri DiagnosticMessage State Transaction -------
#define _STATE_ABORT 0 // Stato Abort
#define _STATE_COMMIT 1 // Stato Commit
#define _STATE_PRONTA 2 // Stato Transazione Pronta

//------- Parametri TransactionMessage Error -------
#define _ERR_TR_GE 0 // ErrorMessage Transaction Generic Error
#define _ERR_TR_RECON 1 // ErrorMessage Transaction Reconnect
#define _ERR_TR_BIZ 2 // ErrorMessage Transaction Bizatin Error
#define _ERR_TR_MISSPACK 3 // ErrorMessage Transaction Miss Pack
#define _ERR_TR_ABORT 4 // ErrorMessage Transaction Abort
#define _ERR_TR_NODOM 5 // ErrorMessage Transaction No Domain
#define _ERR_TR_IGNDOM 6 // ErrorMessage Transaction Ignore Domain
#define _ERR_TR_NORES 7 // ErrorMessage Transaction No resorce
#define _ERR_TR_NOCAP 8 // ErrorMessage Transaction No Cap
#define _ERR_TR_OLDRES 9 // ErrorMessage Transaction Old Resource
#define _ERR_TR_FULLDIR 10 // ErrorMessage Transaction Full Directory
#define _ERR_TR_BUSYRES 11 // ErrorMessage Transaction Busy Resource
#define _ERR_TR_NOFORMAT_CAP 12 // ErrorMessage Transaction No Formatation Capter

//------- Opt Init Start Transaction Message -------
#define OPT1_ISTR_DIR 1
#define OPT1_ISTR_FILE 2
#define OPT2_ISTR_IS 1
#define OPT2_ISTR_NOIS 0

//------- Parametri pacchetti -------
#define _DIM_NICK_PSW 30 // max lunghezza nickname e password
#define _DIM_QUERY_AM 1000 // max lunghezza query amministratore
#define _DIM_NAME_SERVER 30 // max lunghezza nome server
#define _DIM_RES 1024 // max lunghezza campo path risorsa
#define _DIM_DOM 30 // lunghezza campo nome dominio
#define _DIM_IP 16 // lunghezza char ip, comprende il terminatore di stringa

char nick_am[_DIM_NICK_PSW]; //  nickname amministratore
int l_nick_am; //  lunghezza nickname amministratore



// struttura dei pacchetti di comunicazione client-BootServer
struct BootMessage
{
	unsigned char pay_desc;
	double versione_dominio;
	double versione_primay;
};
typedef struct BootMessage BootMessage;


//struttura dei pacchetti di comunicazione amministratore-BootServer
struct BootAM
{
	unsigned char pay_desc;
	char nick[_DIM_NICK_PSW];
	int dim_nick;
	char psw[_DIM_NICK_PSW];
	int dim_psw;
	char query[_DIM_QUERY_AM];
	int dim_query;	
};
typedef struct BootAM BootAM;


// struttura identificativa del server 
struct Server
{
	char name[_DIM_NAME_SERVER+1];
	char ip[_DIM_IP];
	int port;	
};
typedef struct Server Server;



// struttura identificativa del dominio
struct Dominio
{
	char name[_DIM_DOM+1];
};
typedef struct Dominio Dominio;


// struttura dei pacchetti di comunicazione UDP di ping e pong
struct PPMessage
{
	unsigned char pay_desc;
	int id;
	char ip[_DIM_IP];
	int port;	
};
typedef struct PPMessage PPMessage;


// struttura dei pacchetti di comunicazione client-Primary (prima iterazione)
struct InitMessage
{
	unsigned char pay_desc;
	char res[_DIM_RES];
	int l_res;	
	int opt1;
	int opt2;
};
typedef struct InitMessage InitMessage;


// struttura dei pacchetti di comunicazione Diagnostica
struct DiagMessage
{
	unsigned char pay_desc;
	int param_state;
};
typedef struct DiagMessage DiagMessage;

// struttura dei pacchetti di comunicazione Transaction
struct TrMessage
{
	unsigned char pay_desc;
	int id_primary;
	int id_count;
	int msg_count;
	int opt1;
	unsigned int opt2;
};
typedef struct TrMessage TrMessage;

// struttura pacchetto di comunicazione per l'identificazione di un Primary server
struct IDT_PrimaryMessage
{
	unsigned char pay_desc;
	char name[_DIM_NAME_SERVER];
	int l_name;
	char ip[_DIM_IP];
	int port;
};
typedef struct IDT_PrimaryMessage IDT_Message;

// struttura pacchetto di comunicazione per il Bully Algorithm
struct PR_Message
{
	unsigned char pay_desc;		
	char server[_DIM_NAME_SERVER];
	char dom[_DIM_DOM];
	int opt1;
	int opt2;
	char opt_char[_DIM_NAME_SERVER];
};
typedef struct PR_Message PR_Message;


#include "func_networkApp.h"
