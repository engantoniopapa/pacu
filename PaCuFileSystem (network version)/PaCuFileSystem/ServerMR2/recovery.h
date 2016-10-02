
int start_recovery(short int ); // Avvia l'operazione di recovery

int recovery_pronto_replica_primary(int , TrLog *); // gestisce lo stato pronto sul log di una replica contattando il primary

int recovery_pronto_replica_replica( TrLog *, Server * , int ) ;// la replica che ha lo stato pronto con primary assente

void *control_bully(); // processo che fa partire l'algoritmo bully

void *func_helper_bully(void*l)	;

void internal_bully(char * , int );// richiama la procedura di bully (no dispatcher).

void resolv_pronto_newprimary(char *dominio);// richiama la procedura di risoluzione dei log in stato di pronto per un dominio che ha cambiato primary a causa di un crash

Server *search_primary(char *path_db ,	char *dominio ); // cerca il primary di un dominio,
