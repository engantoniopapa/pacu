

int pool_ComunicationServ(int ); // ricezione primo messaggio dal priary/replica

int pool_serv_StartTr(int , InitMessage *); // preparazione della transazione che deve ricevere il pool server

int pool_transaction_upload(int,InitMessage*,int,unsigned int,char *,int,int,int);//transazione sulla replica di _TR_UPLOAD, _TR_NEW_GA, _TR_NEW_CAP, _TR_MV_CAP, _TR_RM_CAP
int pool_transaction_remove(int , InitMessage *, char * , int ,int ,int ); // transazione sulla replica di _TR_RM

int pool_transaction_mkdir(int , InitMessage * ,char * , int  ,int ,int ); // transazione sulla replica di _TR_MKDIR

int pool_transaction_removeDir(int , InitMessage * ,char * , int ,int  ,int );// transazione sulla replica di _TR_RM_DIR

int pool_transaction_modCap(int , InitMessage * ,int , int , char * , int ,int ); // transazione sulla replica di _TR_MOD_CAP
