
#define _DIM_COM_CONSOLE_AM 12


int init_am_fase();//  inizio fase amministratore

int login( int ); // fase login

void stampa_comandi_am(); // stampa i comandi amministratore

int operation_am(int ); // operazioni amministratore

int parsing_am(char * , int ); //  parsing dei comnadi dell'amministratore

int op_exit_am(int ); //  implementazione comando .exit

int op_logout_am(int ); //  implementazione comando .logout

int op_use_am(int , char * , int );  //  implementazione comando .use

int op_chpsw_am(int  , char * , char *); //  implementazione comando .chpsw

int op_select_am(int  , char*  , int ); // implementazione comando .select

int op_geop_am(int  , char*  , int ); // implementazione comando .select
