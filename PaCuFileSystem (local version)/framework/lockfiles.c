#include "framework.h"

/*Allocazione dinamica ed inizializzazione delle variabili necessarie al locking*/
regions * allocForLocking(int size , regions * list_f , pthread_mutex_t p_mux)
{
	int i;
	list_f = (regions *)calloc(size, sizeof(regions));
	for(i = 0; i<size; ++i)
	{
		list_f[i].lista = initHeader();
		list_f[i].inode = -1;
		list_f[i].access = 0;
	}
	pthread_mutex_init(&p_mux, NULL);
	
	return list_f;
}

/*Carica la lista dei capitoli dal file
fd = file descriptor del file
index = indice relativo all'inode del file nella struttura listaFile*/
void loadFromFile(int fd, int index , regions * list_f)
{

	FILE *file = fdopen(fd, "rw");
	fseek(file, 0, SEEK_SET);
	
	char *occurence;
	char tmp[1024]={0x0};
	int i = 0;
	int chapter;
	lockRead(fd);

	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{	
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			chapter = atoi(&tmp[3]); 
			list_f[index].lista = addItemAfterPos(i, chapter, list_f[index].lista);
		}
		++i;
	}	

	unlockFile(fd);

}

/*Libera la memoria allocata precedentemente*/
void freeForLocking(int n_thread , regions * list_f , pthread_mutex_t p_mux)
{
	int i;
	for(i = 0; i < n_thread; i++)
   {
      freeList(list_f[i].lista);
   }
   free(list_f);
   pthread_mutex_destroy(&p_mux);
}

/*Lock di tipo shared sul file da leggere. Restituisce LOCK_SUCCESS in caso di successo, LOCK_ERROR in caso di errore.
file = file descriptor del file*/
int lockRead(int file)
{
	if(flock(file, LOCK_SH) == -1)
	{
		if(errno != EWOULDBLOCK)
		   perror("Errore del lock");
		   return LOCK_ERROR;
	}

	return LOCK_SUCCESS;
}

/*Lock di tipo exlusive sul file da leggere. Restituisce LOCK_SUCCESS in caso di successo, LOCK_ERROR in caso di errore.
file = file descriptor del file*/
int lockWrite(int file)
{
	if(flock(file, LOCK_EX) == -1)
	{
		if(errno != EWOULDBLOCK)
		   perror("Errore del lock");
		   return LOCK_ERROR;
	}
	return LOCK_SUCCESS;
}

/*Unlock del file bloccato precedentemente. Restituisce LOCK_SUCCESS in caso di successo, LOCK_ERROR in caso di errore.
file = file descriptor del file*/
int unlockFile(int file)
{
	if(flock(file, LOCK_UN) == -1)
	{
		if(errno != EWOULDBLOCK)
		   perror("Errore con unlock");
		   return LOCK_ERROR;
	}
	return LOCK_SUCCESS;
}

/*Realizza il lock logico di un capitolo all'interno del file. Chi trova un capitolo bloccato rimane in attesa del segnale di sblocco.
file = file descriptor del fileClient
init = capitolo da bloccare*/
struct list_el *lockLogicalWrite(int file, int init , int n_thread , regions * list_f , pthread_mutex_t p_mux  )
{
	int index = -1;
	int turn = 0;
	struct stat buf;  
   struct list_el *chapter;
   fstat(file, &buf);

   pthread_mutex_lock(&p_mux);
   //printf("Ottenuto mutex = %lu\n", pthread_self());
   if((index = findSlot(buf.st_ino , n_thread ,list_f)) == -1)
   {
   	return NULL;
   }
   
   if(list_f[index].inode != buf.st_ino)
   {
   	//printf("Carico la lista dal file %d\n", file);
   	list_f[index].inode = buf.st_ino;
   	loadFromFile(file, index , list_f);
   }
   //printf("init = %d, init lista = %d\n", init, list_f[index].lista->init);
	chapter = searchItemFromID(init, list_f[index].lista);
	
	if(!chapter)
	{
		puts("Capitolo inesistente. Si prega di ricaricare il file per ulteriori aggiornamenti");
		pthread_mutex_unlock(&p_mux);
		return NULL;
	}
	list_f[index].access++;
	if(!chapter->locked)
	{
		chapter->locked = 1;
		//printf("Ottenuto lock regione %d\n", init);
		chapter->ticketGenerator = 0;
		chapter->turn = turn;
		//printf("Rilascio mutex = %lu\n", pthread_self());
		pthread_mutex_unlock(&p_mux);
		return chapter;
	}
	
	printf("Lock già presente - %lu\n", pthread_self());
	turn = ++chapter->ticketGenerator;
	do
		pthread_cond_wait(&chapter->cond_write, &p_mux);
	while(chapter->turn != turn);	
	//printf("sono libero = %lu\n", pthread_self());
	chapter->locked = 1;
	pthread_mutex_unlock(&p_mux);
	return chapter; 
}




/*Realizza il lock logico senza mutex di un capitolo all'interno del file. Chi trova un capitolo bloccato rimane in attesa del segnale di sblocco.
file = file descriptor del fileClient
init = capitolo da bloccare*/
struct list_el *lockLogicalWrite_nomutex(int file, int init , int n_thread , regions * list_f , pthread_mutex_t p_mux )
{
	int index = -1;
	int turn = 0;
	struct stat buf;  
  struct list_el *chapter;
  fstat(file, &buf);


   if((index = findSlot(buf.st_ino , n_thread ,list_f)) == -1)
   {
   	return NULL;
   }
   
   if(list_f[index].inode != buf.st_ino)
   {
   	//printf("Carico la lista dal file %d\n", file);
   	list_f[index].inode = buf.st_ino;
   	loadFromFile(file, index , list_f);
   }
   //printf("init = %d, init lista = %d\n", init, list_f[index].lista->init);
	chapter = searchItemFromID(init, list_f[index].lista);
	
	if(!chapter)
	{
		//puts("Capitolo inesistente. Si prega di ricaricare il file per ulteriori aggiornamenti");
		return NULL;
	}
	list_f[index].access++;
	if(!chapter->locked)
	{
		chapter->locked = 1;
		chapter->ticketGenerator = 0;
		chapter->turn = turn;
		return chapter;
	}
	
	//printf("Lock già presente - %lu\n", pthread_self());
	turn = ++chapter->ticketGenerator;
	do
		pthread_cond_wait(&chapter->cond_write, &p_mux);
	while(chapter->turn != turn);	

	chapter->locked = 1;

	return chapter; 
}



/*Ricerca un inode nella listaFile. Restituisce lo slot occupato dall'inode o uno disponibile.
inode = inode del file*/
int findSlot(int inode , int n_thread , regions * list_f)
{   		
	int freeSlot, oldSlot, i;
	freeSlot = -1;
	oldSlot = -1;
	for(i = 0; i < n_thread; ++i)
   {
   		
      if(list_f[i].inode == inode)
      {   
         return i;
      }
      if(list_f[i].inode == -1 && freeSlot == -1)
         freeSlot = i;
      if(list_f[i].access == 0 && oldSlot == -1)
      	oldSlot = i;
   }
  
   if(freeSlot == -1)
   	return freeSlot;
   else
   	return oldSlot;
}

/*Unlock logico di un capitolo di un file precedentemente bloccato
chapter = lista dei capitoli relativa al file considerato
file = file descriptor del file*/
void unlockChapter(struct list_el *chapter, int file , int n_thread , regions * list_f  , pthread_mutex_t p_mux)
{
	int index;
	struct stat buf;
	fstat(file, &buf);
	pthread_mutex_lock(&p_mux);
	index = findSlot(buf.st_ino , n_thread , list_f);
	//printf("Libero capitolo = %d - %lu\n", chapter->init, pthread_self());
	chapter->locked = 0;
	++chapter->turn;	
	pthread_cond_broadcast(&chapter->cond_write);
	--list_f[index].access;
	pthread_mutex_unlock(&p_mux);
}

/*Calcola e restituisce la patch relativa ad un capitolo
fd = file descriptor del file da cui estrarre il capitolo
chapter = identificativo del capitolo da modificare
newChapter = file descriptor del file contentente il testo con cui calcolare la patch*/
char *loadPatch(int fd, int chapter, int newChapter)
{
	char cap[1024];
	char tmp[1024]={0x0};
	char tempPath[1024]; 
	char *occurence;
	struct stat buf;
	char *result;
	int tempFile = create_fileTemp("./", tempPath, sizeof(tempPath));
	FILE *file = fdopen(fd, "rw");
	snprintf(cap, 1024, "%s%d", CHAPTERS, chapter);
	printf("Valore stringa = %s\n", cap);
	fseek(file, 0, SEEK_SET);
	int size = 0;
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, cap)))
		{
			break;
		}
	}
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			break;
		}
		else
		{
			size += strlen(tmp);
			writen(tempFile, tmp, strlen(tmp));
		}
	}
	lseek(tempFile, 0L, SEEK_SET);
	char buffer[size];
	memset(buffer, '\0', sizeof(buffer));
	
	if(readn(tempFile, buffer, sizeof(buffer)) < 0)
	{
		perror("errore lettura capitolo originale");
		return 0;
	}

	fstat(newChapter, &buf);
	char buffer2[buf.st_size + 1];
	memset(buffer2, '\0', sizeof(buffer2));
	if(readn(newChapter, buffer2, sizeof(buffer2)) < 0)
	{
		perror("errore lettura nuovo Capitolo");
		return 0;
	}
	
	if(!(result = patchCreate(buffer, buffer2)))
	{
		printf("Errore nella creazione della patch %lu\n", pthread_self());
	}
	
	close(tempFile);
	if(remove(tempPath) == -1)
	{
		perror("Errore nella rimozione del file temporaneo");
	}
	return result;
}

/*Applica la patch al capitolo del file. Restituisce 0 in caso di errori, 1 altrimenti.
fd = file descriptor del file originale
chapter = id del capitolo da modificare
patch = testo della patch da utilizzare
tempPath = path del file temporaneo contentente il capitolo patchato*/
int applyPatchToChapter(int fd, int chapter, char *patch, char *tempPath)
{
	char cap[1024];
	char tmp[1024]={0x0};
	char *occurence;
	char *result;
	int tempFile = create_fileTemp("./", tempPath, sizeof(tempPath));
	FILE *file = fdopen(fd, "rw");
	snprintf(cap, 1024, "%s%d", CHAPTERS, chapter);
	printf("Valore stringa = %s\n", cap);
	//lockRead(fd); Lascia il commento se non vuoi morire
	fseek(file, 0, SEEK_SET);
	int size = 0;
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, cap)))
		{
			break;
		}
	}
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			break;
		}
		else
		{
			size += strlen(tmp);
			writen(tempFile, tmp, strlen(tmp));
		}
	}

	lseek(tempFile, 0L, SEEK_SET);
	char buffer[size];
	memset(buffer, '\0', sizeof(buffer));
	
	if(readn(tempFile, buffer, sizeof(buffer)) < 0)
	{
		perror("errore lettura capitolo originale");
		return 0;
	}
	

	result = patchApply(buffer, patch);
	printf("RISULTATO: \n%s --------------%lu\n", result, pthread_self());

	close(tempFile);
	tempFile = open(tempPath, O_RDWR|O_TRUNC);
	writen(tempFile, result, strlen(result)+1);
	printf("<<<<<<<<<<<<<<<<<<< SIX %lu\n", pthread_self());
	free(result);
	close(tempFile);
	return 1;
}

/*Sostituisce il capitolo originale con quello patchato. Restituisce 0  in caso di errore, 1 altrimenti
filePath = path del file originale
chapP = path del file contenente il capitolo patchato
chapter = id del capitolo da modificare*/
int commitWrite(char *filePath, char *chapP, int chapter)
{
	char cap[1024] = {0x0};
	char tmp[1024]={0x0};
	char tempPath[1024];
	char *occurence;
	int size = 0;
	snprintf(cap, 1024, "%s%d", CHAPTERS, chapter);
	int filePatched = create_fileTemp("./", tempPath, sizeof(tempPath));
	int fd = open(filePath, O_RDWR);

	FILE *file = fdopen(fd, "rw");
	FILE *chapPatched = fopen(chapP, "r");
	fseek(file, 0, SEEK_SET);
	fseek(chapPatched, 0, SEEK_SET);
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, cap)))
		{
			writen(filePatched, tmp, strlen(tmp));
			size += strlen(tmp);
			break;
		}
		else
		{
			writen(filePatched, tmp, strlen(tmp));
			size += strlen(tmp);
		}
	}
	
	while(chapPatched!=NULL && fgets(tmp, sizeof(tmp), chapPatched)!=NULL)
	{
			writen(filePatched, tmp, strlen(tmp));
			size += strlen(tmp);
	}
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			writen(filePatched, "\n", 1);
			writen(filePatched, tmp, strlen(tmp));
			size += strlen(tmp) + 1;
			break;
		}
	}
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		writen(filePatched, tmp, strlen(tmp));
		size += strlen(tmp);
	}
	lseek(filePatched, 0L, SEEK_SET);
	char buffer[size];
	close(fd);
	fd = open(filePath, O_RDWR|O_TRUNC);
	if(readn(filePatched, buffer, sizeof(buffer)) < 0)
	{
		perror("errore lettura nuovo file");
		return 0;
	}
	if(writen(fd, buffer, sizeof(buffer)) < 0)
	{
		perror("errore scrittura nuovo file");
		return 0;
	}
	//unlockFile(fd); Lascia il commento se non vuoi morire
	fclose(chapPatched);
	if(remove(tempPath) == -1)
	{
		perror("Errore nella rimozione del file temporaneo");
	}
	if(remove(chapP) == -1)
	{
		perror("Errore nella rimozione del file temporaneo");
	}
	printf("Ho concluso il commit - %lu\n", pthread_self());
	close(fd);
	return 1;
}


// Blocca tutti i capitoli di un file, lock logico e fisico, ritorna l'indice del file nella lista, se c'e' un errore -1
// descrittore del file ga
int lockAllChapters(int file , regions * list_f  , int n_thread, pthread_mutex_t p_mux)
{
	int index = -1;
	struct stat buf;
	struct list_el *chapter;
	fstat(file, &buf);
	pthread_mutex_lock(&p_mux);
	
	if((index = findSlot(buf.st_ino , n_thread , list_f)) == -1)
	{
		return -1;
	}

	if(list_f[index].inode != buf.st_ino)
	{
		list_f[index].inode = buf.st_ino;
		loadFromFile(file, index , list_f);
	}

	chapter = list_f[index].lista;

	while(chapter)
	{
		lockLogicalWrite_nomutex(file, chapter->init , n_thread , list_f , p_mux);
		chapter = chapter->next;
	}

	if(!lockWrite(file))
	{
		unlockAllChapters(index, file , list_f , n_thread , p_mux);
	}
	


	return index;
}



// Sblocca tutti i capitoli di un file, unlock logico e fisico, ritorna 1 se l'operazione è andata a buon fine, se c'e' un errore 0
// descrittore del file ga
int unlockAllChapters(int index, int file , regions * list_f , int n_thread, pthread_mutex_t p_mux)
{	
	lseek(file, 0, SEEK_SET);
	struct list_el *chapter = list_f[index].lista;
	while(chapter)
	{
		unlockChapter(chapter, file , n_thread , list_f , p_mux);
		chapter = chapter->next;
	}
	
	return unlockFile(file);
}



// rimuove un file ga dalla lista
void remove_logicalFile(int index , regions * list_f )
{
	list_f[index].lista = freeList(list_f[index].lista);
	list_f[index].inode = -1;
	list_f[index].access = 0 ;
}
