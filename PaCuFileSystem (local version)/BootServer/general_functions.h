

void get_conf(); // inizializzazione delle variabili di configurazione

void create_password( char * , char*  , int ); // crea la password in md5 (Amministratore)

int scelta_db(DB * , BootAM * , char* ); // seleziona il DB su cui operare (Amministratore)

void free_all(); // dealloca la memoria dinamica allocata in precedenza



