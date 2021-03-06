#define GA_FORMAT "##PACUFS__GA__FORMAT##214123412342134123##" 
#define CHAPTERS "^^^"



int is_GaFile(char * , char *); // controlla se un file è di tipo GA 

int is_gaFile(char *); // controlla se un file è di tipo GA (non ritorna la descrizione dell' errore)

int print_ga_txt(char * , char * , int ); // converte un file ga in un file stampabile

int extract_from_IdCapGA(int , int , int ); // estrae il capitolo con descrittore chapter del file fd, nel file tempFile

int extract_excluse_IdCapGA(int , int , int ); // estrae tutto il file escluso il capitolo con descrittore chapter del file fd, nel file tempFile

int extract_from_PosCapGA(int , int , int ); // estrae il capitolo numero pos del file fd , nel file tempFile

int move_PosCapGA(int , int  , int, int , char* ); // sposta il capitolo con descrittore chapter alla posizione pos nel file

int count_CapGA(int ); // conta il numero di capitoli del file GA

int get_id_pos(int , int); // restituisce l'id del capitolo di un file GA ad una data posizione

int insert_CapGA(int , int  , int, int , char*); // inseriscil il capitolo con descrittore chapter alla posizione pos nel file

int anal_ga(int ); // verifica che non ci siano caratteri speciali nel testo 
