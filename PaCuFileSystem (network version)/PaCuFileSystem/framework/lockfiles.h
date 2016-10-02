

/*Struttura allocata per ogni thread passata come parametro per la rispettiva funzione. Contiene:
arg = definisce il tipo di lock su file (LOCK_EX, LOCK_SH);
init = id capitolo da modificare
pathFile = il path del file su cui si vuole effettuare il lock;
pathChapter = path del file contenente il capitolo modificato*/
typedef struct
{
   int arg, init;
   char pathFile[1024];
   char pathChapter[1024];
}paramLockFile;

/*Struttura che rappresenta le caratteristiche di un file.
lista = elenco dei capitoli contenuti in un file
inode = inode del file
access = numero di accessi presenti in un dato istante sul file*/
typedef struct
{
   struct list_el *lista;
   int inode;
   int access;
}regions;

/*Allocazione dinamica ed inizializzazione delle variabili necessarie al locking*/
regions * allocForLocking(int size , regions * , pthread_mutex_t);

/*Libera la memoria allocata precedentemente*/
void freeForLocking(int , regions * list_f , pthread_mutex_t);

/*Lock di tipo shared sul file da leggere. Restituisce LOCK_SUCCESS in caso di successo, LOCK_ERROR in caso di errore.*/
int lockRead(int file);

/*Lock di tipo exlusive sul file da leggere. Restituisce LOCK_SUCCESS in caso di successo, LOCK_ERROR in caso di errore.*/
int lockWrite(int );

/*Unlock del file bloccato precedentemente. Restituisce LOCK_SUCCESS in caso di successo, LOCK_ERROR in caso di errore.*/
int unlockFile(int );

/*Calcola e restituisce la patch relativa ad un capitolo*/
char *loadPatch(int , int , char *);

/*Realizza il lock logico di un capitolo all'interno del file. Chi trova un capitolo bloccato rimane in attesa del segnale di sblocco.*/
struct list_el *lockLogicalWrite(int file, int init , int , regions *  , pthread_mutex_t);

/*Ricerca un inode nella listaFile. Restituisce lo slot occupato dall'inode o uno disponibile.*/
int findSlot(int , int , regions *);

/*Unlock logico di un capitolo di un file precedentemente bloccato*/
void unlockChapter(struct list_el *chapter, int inode , int  , regions * , pthread_mutex_t);

/*Unlock logico di un capitolo di un file precedentemente bloccato con indice*/
void unlockChapter_index(int index, struct list_el *chapter, int inode , int  , regions * , pthread_mutex_t);

/*Sostituisce il capitolo originale con quello patchato. Restituisce 0  in caso di errore, 1 altrimenti*/
int commitWrite(char *filePath, char *chapP, int chapter);

/*Applica la patch al capitolo del file. Restituisce 0 in caso di errori, 1 altrimenti.*/
int applyPatchToChapter(int , char *, char *, char *);

/*Blocca tutti i capitoli di un file, lock logico e fisico*/
int lockAllChapters(int  , regions *  ,int, pthread_mutex_t);

/*Sblocca tutti i capitoli di un file, unlock logico e fisico*/
int unlockAllChapters(int index, int file , regions *  , int , pthread_mutex_t );

void remove_logicalFile(int  , regions *); // rimuove un file ga dalla lista
