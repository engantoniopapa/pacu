#include "framework.h"

int generatoreRandom(int num)
{
	int a;
	a = (int)(num * (rand()/(RAND_MAX+1.0)));
	return a;
}


// copia il file source nel file dest, se il file dest esiste lo sovrascrive, ritorna: 0 se c'e' un errore , 1 se l'operazione va a buon fine
// Parametri -> path file source , path file dest , dimensione blocco per lettura/scrittura   
int cp_rewrite_file(char *source , char *dest ,int size )
{
	FILE *f_source;
	FILE *f_dest;
	int count;
	
	f_source = fopen(source , "r");
	if( f_source == NULL)
	{
		perror("Error: ");
		return 0;
	}
	
	f_dest = fopen(dest , "w+");
	if( f_dest == NULL)
	{
		perror("Error: ");
		return 0;
	}
	
	unsigned char buff[size];
	
	count = 1 ;
	
	while(count)
	{
		memset(buff , '0' , size);
		count = fread( (void *)buff, sizeof(unsigned char) , size , f_source);

		if( fwrite( (void *)buff, sizeof(unsigned char) , count , f_dest) < count)
		{
				printf("errore nel file: %s \n" , dest); 
				count = 0 ;
		}

	}
	
	fclose(f_source);
	fclose(f_dest);
	
	return 1;
}


// verifica l'esistenza di una directory, ritorna: 0 se l'operazione fallisce, 1 se ha avuto successo
// Parametri -> path della directory
int isDir(const char *dname) 
{
  struct stat sbuf;
 
  if (lstat(dname, &sbuf) == -1)
  {
    return 0;
  }
 
  if(S_ISDIR(sbuf.st_mode)) 
  {
    return 1;
  }
 
return 0;
}



// verifica l'esistenza di un file, ritorna: 0 se l'operazione fallisce, 1 se ha avuto successo
// Parametri -> path del file
int isFile(const char *fname) 
{
  struct stat sbuf;
 
  if (lstat(fname, &sbuf) == -1) 
  {
    return 0;
  }
 
  if(S_ISREG(sbuf.st_mode)) 
  {
    return 1;
  }
 
return 0;
}



// Duplica la stringa passata come parametro, ritorna: la stringa duplicata
// Parametri -> stringa da duplicare
char *dup_str(const char *s) {
    size_t n = strlen(s) + 1;
    char *t = malloc(n);
    if (t) {
        memcpy(t, s, n);
    }
    return t;
}

 
 
//Ottiene la lista di tutti i file presenti in una directory
//PARAMETRI: - path: percorso della directory interessata
//				 - counter: numero di file e directory presenti nel path selezionato
char **get_all_files(const char *path, size_t *used) {
    DIR *dir;
    struct dirent *dp;
    char **files;
    char *temp;
    size_t alloc;
 
    if (!(dir = opendir(path))) {
        goto error;
    }
 
    *used = 0;
    alloc = 10;
    if (!(files = malloc(alloc * sizeof *files))) {
        goto error_close;
    }
 
    while ((dp = readdir(dir))) {
        if (*used + 1 >= alloc) {
            size_t new = alloc / 2 * 3;
            char **tmp = realloc(files, new * sizeof *files);
            if (!tmp) {
                goto error_free;
            }
            files = tmp;
            alloc = new;
        }
        if (!(files[*used] = dup_str(dp->d_name))) {
            goto error_free;
        }
        
        	temp = calloc(strlen(path) + 1 + strlen(files[*used]) + 1, sizeof(char*));
	  		strcat(temp, path);
	  		strcat(temp, "/");
	  		strcat(temp, files[*used]);
	  		if(isDir(temp))
	  		{
	  			files[*used] = strcat(files[*used], "$");
	  		}
        ++*used;
    }
 
    files[*used] = NULL;
 
    closedir(dir);
    return files;
 
error_free:
    while (*used--) {
        free(files[*used]);
    }
    free(files);
 
error_close:
    closedir(dir);
 
error:
    return NULL;
}
 
 
 
//Stampa il nome del contenuto della directory 
void printContent(char **files, size_t counter)
{
	int i;
	for (i = 0; i < counter; ++i) 
	{
    		if(strstr(files[i],"$"))
    		{
    			printf ("\e[1;31m");
    			char *temp = files[i] + strlen(files[i]) - 1;
    			*temp = ' ';
    		}
    		if((strcmp(files[i], ". ") != 0) && (strcmp(files[i], ".. ") != 0))
        		printf("%s\n", files[i]);
        printf ("\033[m");
    }
}



//Libera la memoria dalla matrice di stringhe utilizzata per l'elencazione del contenuto
void freeContent(char **files, size_t counter)
{
	int i;
	for (i = 0; i < counter; ++i) {
        free(files[i]);
    }
    free(files);
}



//Ottiene la lista di tutti i file e directory che corrispondono al filtro inserito
//PARAMETRI: - path: percorso della directory di partenza
//				 - counter: numero di file e directory corrispondenti al filtro
// 			 - list: elemento necessario per la ricorsione
//				 - alloc: indica il numero di stringhe da allocare in caso di esubero
//				 - used: indica il numero di stringhe presenti
//				 - ch: variabile di controllo;
char **get_filtered_files(char *path, char *search, char **list, int *alloc, int *used, short int *ch) {
    DIR *dir;
    struct dirent *dp;
    char *files, *temp;
    short int check;
    
    check = 0;
 
    if ((dir = opendir(path))) { 
		 while ((dp = readdir(dir))) {
		 		check = 0;
		 		if (*used + 1 >= *alloc) {
		         size_t new = *alloc + 5;
		         char **tmp = realloc(list, new * sizeof *list);
		         if (!tmp) {
		             puts("Errore nell'allocazione dinamica - Stampa risultati ottenuti fino ad ora");
						 while ((*used)--) {
						 	  printf("%s\n", list[*used]);
							  free(list[*used]);
						 }
						 free(list);
						 exit(0);
		         }
		         list = tmp;
		         *alloc = new;
        		}
				files = dup_str(dp->d_name);
	     		if((strcmp(files, ".") != 0) && (strcmp(files, "..") != 0))
	     		{
			  		temp = calloc(strlen(path) + 1 + strlen(files) + 1, sizeof(char*));
			  		strcat(temp, path);
			  		strcat(temp, "/");
			  		strcat(temp, files);
			  		if(strstr(files,search) != NULL){
					 	list[*used] = temp;
					 	*used = *used + 1;
					 	free(files);
			  			list = get_filtered_files(temp, search, list, alloc, used, &check);
			  			if(check)
			  			{
			  				*temp = '$';
			  				check = 0;
			  			}
				 	}
				 	else
				 	{
					 	free(files);
				  		list = get_filtered_files(temp, search, list, alloc, used, &check);
				  		free(temp);
				  	}
				}
				else
				{
					*ch = 0;
					free(files);
				}
		 }
		 
    	closedir(dir);
    	}
    	else
    	{
    		*ch = 1;
    	}
    	return list;
}
 
 
 
// Inizializza la lista da passare alla funzione get_filtered_files e da inizio alla procedura ricorsiva
char **search(char *path, char *search, int *counter)
{
	char **list;
	int alloc;
	short int check;
	
	alloc = 10;
	*counter = 0;
	check = 0;
	
	list = malloc(alloc * sizeof *list);
	list = get_filtered_files(path, search, list, &alloc, counter, &check);
	return list;
}



//Stampa i risultati ottenuti 
void printList(char **files, int counter)
{
	int i;
	for (i = 0; i<counter; ++i) {
    	  if(files[i])
    	  {
    	  	  if(*files[i] == '$')
    	  	  {
		 	  		printf ("\033[m");
		 	  		*files[i] = '/';
		 	  }
		     printf("%s\n", files[i]);
		     printf ("\e[1;31m");
        }
    }
}



//Libera la memoria dalla matrice di stringhe ottenuta come risultato della ricerca
void freeList_search(char **files, int counter)
{
	int i;
	for (i = 0; i<counter; ++i) {
        free(files[i]);
    }
    printf ("\033[m");
    puts("arrivo fino a qui");
    free(files);
}



//	Funzione di scrittura in stream su socket ritorna: -1 se c'è un errore altrimenti il numero di byte scritti
//	Parametri-> descrittore socket, buffer per la lettura, dimensione blocco scrittura
ssize_t writen(int fd, const void *buf, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = buf;
  nleft = n;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
       if ((nwritten < 0) && (errno == EINTR)) nwritten = 0; 	    
       else return(-1);	    /* errore */
    }
    nleft -= nwritten;
    ptr += nwritten;  
  }
  return(n-nleft);
}



//	Funzione di lettura in stream su socket ritorna: -1 se c'è un errore altrimenti il numero di byte rimanenti da
//	Parametri-> descrittore socket, buffer per la scrittura, dimensione blocco lettura
ssize_t readn(int fd, void *buf, size_t count) 
{
	size_t nleft;
	ssize_t nread;
 
	nleft = count;
	while (nleft > 0) {             /* repeat until no left */
		if ( (nread = read(fd, buf, nleft)) < 0) {
			if (errno == EINTR) {   /* if interrupted by system call */
				continue;           /* repeat the loop */
			} else {
				return(nread);      /* otherwise exit */
			}
		} else if (nread == 0) {    /* EOF */
			break;                  /* break loop here */ 
		}
		nleft -= nread;             /* set left to read */
		buf +=nread;                /* set pointer */
	}
	return (nleft);
}



//	Funzione di lettura che si blocca in attessa solo dei primi dati, ritorna: -1 se c'è un errore altrimenti il numero di byte letti
//	Parametri-> descrittore socket, buffer per la scrittura, dimensione blocco lettura
ssize_t readn_noblock(int fd, void *buf, size_t count) 
{
	size_t nleft;
	ssize_t nread;
 
	nleft = count;
	while (nleft > 0) 
	{             /* repeat until no left */
		if ( (nread = read(fd, buf, nleft)) < 0) 
		{
			if (errno == EINTR) 
			{   /* if interrupted by system call */
				continue;           /* repeat the loop */
			} 	
			else
		 	{
				return(nread);      /* otherwise exit */
		 	}
					
		} 
		else if (nread < nleft) 
		{    /* EOF */				
			break;                  /* break loop here */ 
		}

		nleft -= nread;             /* set left to read */
		buf +=nread;                /* set pointer */

	}

	return (nleft);
}



// crea un file temporaneo a partire da un file, ritorna il descrittore del file creato o -1 se c'e' un errore
// Parametri -> il path del file che viene copiato
int cp_fileTemp(char *path , int maxline)
{
	int fd, ftemp, n;
	n = 0;
	char indbuff[maxline];
	
	char tempPath[strlen(path) + 7];
	memset(tempPath, '\0', strlen(path) + 7);
	strcpy(tempPath, path);
	strcat(tempPath, "XXXXXX");
	printf("%s\n", tempPath);
	if((fd = open(path, O_RDONLY)) == -1)
	{
		perror("errore apertura file");
		return -1;
	}
	
	if((ftemp = mkstemp(tempPath)) == -1)
	{
		perror("errore creazione file temporaneo");
		return -1;
	}
	
	while(n == 0)
	{
		if ((n = readn(fd, indbuff, maxline)) == -1) 
		{
			perror("errore nella lettura del file");
			return -1;
		}

		if (writen(ftemp, indbuff, maxline-n) < 0) 
		{
			fprintf(stderr, "errore in write\n");
			exit(1);
		}
	}
	
	close(fd);
	return ftemp;
}



// crea un file temporaneo vuoto, ritorna il descrittore del file creato o -1 se c'e' un errore
// Parametri -> path della directory in cui creare il file temporaneo, path del filetemporaneo, dimensione del path
int create_fileTemp(char *dir_path , char *tempPath , int dim )
{
	int  ftemp;

	memset(tempPath, '\0', dim);
	strcpy(tempPath, dir_path);
	strncat(tempPath , "/" , 1);
	strcat(tempPath, "XXXXXX");

	if((ftemp = mkstemp(tempPath)) == -1)
	{
		perror("errore creazione file temporaneo");
		return -1;
		tempPath = NULL ;
	}

	return ftemp;
}



// copia la risorsa sorg nella risorsa dest, ritorna 1 se la copia è avvenuta con successo, 0 se c'e' stato qualche errore (versione bloccante)
// Parametri-> risorsa sorgente , risorsa destinazione, blocco in byte lettura/scrittura 
int write_res_on_res(int sorg, int dest, int maxline)
{
    int    n;
    int check = 0;
    char recvline[maxline];
    
   lseek(sorg, 0, SEEK_SET);
   lseek(dest, 0, SEEK_SET);
   
    for(; ;)
    {       
        if ((n = readn(sorg, recvline, maxline)) > 0)
        {
            check = 1;
        }
       
        if ((n = writen(dest, recvline, maxline-n)) < 0)
        {
            fprintf(stderr, "errore in write\n");
            return 0;
        }
       
        if(check == 1)
        {
            return 1;
        }
    }
}



// copia la risorsa sorg nella risorsa dest, ritorna 1 se la copia è avvenuta con successo, 0 se c'e' stato qualche errore 
// Parametri-> risorsa sorgente , risorsa destinazione, blocco in byte lettura/scrittura 
int write_control_res(int sorg, int dest, int maxline , unsigned int size_f)
{   
    int    n;
    int check = 0;
    
   lseek(sorg, 0, SEEK_SET);
   lseek(dest, 0, SEEK_SET);

    if( maxline > size_f)
    	maxline = (int) size_f;
    
    char recvline[maxline+1];
    memset(recvline , '\0' , maxline+1);
  
    for(; ;)
    {       
    	if ((n = readn(sorg, recvline, maxline)) > 0)
      {   
          check = 1;
      }

      if ((n = writen(dest, recvline, maxline-n)) < 0)
      {   
          fprintf(stderr, "errore in write\n");
          return 0;
      }

     	size_f = size_f - n;
     	
     	if( maxline > size_f)
    		maxline = (int) size_f;

      if(check == 1 || size_f == 0)
      {
          return 1;
      }
    }
}



// estrae il nome del file dal path 
// Parametri -> nome del file , max_lunghezza del nome , path
void get_namefile(char* name, int l_name, char *path)
{
	int i ;
	
	for(i = strlen(path) ; i > 0; --i)
	{
		if(path[i] == '/')
			break;
	}
	
	++i;
	
	strncpy( name , &path[i] , l_name);
}



// restituisce il percorso della cartella superiore 
// Parametri -> path superiore , max_lunghezza del nome , path
void get_top_path(char* sup_path, char *path)
{
	int i ;
	
	for(i = strlen(path) ; i > 0; --i)
	{
		if(path[i] == '/')
			break;
	}
	
	strncpy( sup_path , path , i);
}

// crea una directory, ritorna: 0 se l'operazione fallisce, 1 se ha avuto successo
// percorso della directory da creare
int createDirectory(char *path)
{
	if(mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1)
		return 0;
		
	else
		return 1;
}



// elimina una directory se vuota, ritorna: 0 se l'operazione fallisce, 1 se ha avuto successo
// percorso della directory da creare
int removeDirectory(char *path)
{
	if(rmdir(path) == -1)
		return 0;
	
	else
		return 1;
}



// restituisce l'indirizzo ip pubblico della macchina
// Parametri-> interfaccia di rete a cui si è connessi , ip che viene restituito , dimensione dell'ip in char
void get_myip(char *interface , char *ip , int dim)
{
	int fd;
	struct sockaddr_in in ;
	struct ifreq ifr; 

	memset(ip , '\0' , dim ) ;

	fd = socket(PF_INET , SOCK_STREAM , 0);
	strcpy(ifr.ifr_name, interface);
	ioctl(fd,SIOCGIFADDR,&ifr);
	in = *(  (struct sockaddr_in *)(&(ifr.ifr_addr))  ); 

	strncpy(ip, inet_ntoa(in.sin_addr) , dim-1);

	close(fd);
}
