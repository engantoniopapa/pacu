int init_message(int );  // ricezione primo messaggio dal client

int send_dom_list(int  , char*); //	Invio della lista Domini

int send_list_file(int , char **, size_t ); //invia la lista file

int send_list_filtered(int  , char **, size_t); //invia la lista in risposta alla search

int op_init_update(int ); // operazione sul messaggio di init_update

int op_init_ls(int  , InitMessage *); // operazione sul messaggio di init_ls

int op_init_StartTr(int , InitMessage *); // operazione sul messaggio di init_start_tr

int op_init_cd(int  , InitMessage *); // operazione sul messaggio di init_cd

int op_init_search(int , InitMessage *); // operazione sul messaggio di init_search

int op_init_readR(int , InitMessage *); // operazione sul messaggio di init_read_resource

int op_init_stateTR(int  , InitMessage *); // operazione sul messaggio di init_state_transaction

int op_init_whoIsPr(int connsd , InitMessage *msg_i); // operazione sul messaggio di  InitMessage Who Is Primary
