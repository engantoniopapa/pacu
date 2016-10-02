#include "framework.h"


// controlla se un file è di tipo GA, ritorna 1 se il file è di tipo Ga, 0 se non è di tipo Ga, -1 se c'e' un errore
// Parametri -> path del file , stringa contente il tipo di errore
int is_GaFile(char *path, char *errore) 
{
	int fd;
	fd = open(path, O_RDONLY);
	
	if(fd <= 0)
	{
		strcpy(errore , "Errore nell'apertura della risorsa controllare i permessi e riprovare!!!!");
		return -1;
	}
	
	char formato[strlen(GA_FORMAT)+1];
	int ga_form = 0;

	memset(formato , '\0' , strlen(GA_FORMAT)+1 );


	if(readn(fd , formato , strlen(GA_FORMAT) ) == 0)
	{

	 	if(strncmp(GA_FORMAT , formato , strlen(GA_FORMAT)) == 0)
	 		ga_form = 1;

	}

	close(fd);
	
	return ga_form;
}



// controlla se un file è di tipo GA, ritorna 1 se il file è di tipo Ga, 0 se non è di tipo Ga, -1 se c'e' un errore (non ritorna 
// la descrizione dell' errore)
// Parametri -> path del file 
int is_gaFile(char *path) 
{
	int fd;
	fd = open(path, O_RDONLY);
	
	if(fd <= 0)
	{
		return -1;
	}
	
	char formato[strlen(GA_FORMAT)+1];
	int ga_form = 0;

	memset(formato , '\0' , strlen(GA_FORMAT)+1 );


	if(readn(fd , formato , strlen(GA_FORMAT) ) == 0)
	{

	 	if(strncmp(GA_FORMAT , formato , strlen(GA_FORMAT)) == 0)
	 		ga_form = 1;

	}

	close(fd);
	
	return ga_form;
}



// converte un file ga in un file stampabile, ritorna: 1 se l'operazione è riuscita, 0 se c'e' stato un errore
// Parametri -> path del file ga , path del file da stampare , dimensione del blocco lettura scrittura
int print_ga_txt(char *path_ga , char *path_txt , int size)
{
	FILE *f_source;
	FILE *f_dest;
	
	f_source = fopen(path_ga , "r");
	if( f_source == NULL)
	{
		perror("Error: ");
		return 0;
	}
	
	f_dest = fopen(path_txt , "w+");
	if( f_dest == NULL)
	{
		perror("Error: ");
		return 0;
	}

	fseek(f_source, 0, SEEK_SET);
	fseek(f_dest, 0, SEEK_SET);
	
	char buff[size+1];
	int capitolo = 1;
	
	fgets(buff, size, f_source);
	fgets(buff, size, f_source);
	
	memset(buff , '\0' , size +1);
	if( fgets(buff, size, f_source) != NULL )
	{
		if(strncmp(buff , CHAPTERS , 3*sizeof(char)) == 0)
		{
			memset(buff , '\0' , size+1);
			snprintf(buff , size , "\e[1;32mCapitolo %d \033[m\n\n" ,  capitolo);
			++capitolo;
			fputs(buff , f_dest);
			memset(buff , '\0' , size+1);
		}
		
		while(fgets(buff, size, f_source) != NULL)
		{
			if(strncmp(buff , CHAPTERS , 3*sizeof(char)) == 0 )
			{
				
				memset(buff , '\0' , size+1);
				snprintf(buff , size , "\n\n\n\n\e[1;32mCapitolo %d \033[m\n\n" ,  capitolo);
				++capitolo;
				fputs(buff , f_dest);
			}
			else
			{
				 fputs(buff , f_dest);
			}
			memset(buff , '\0' , size+1);
		}
	}
	
	fclose(f_source);
	fclose(f_dest);
	
	return 1;
}



// estrae il capitolo con descrittore chapter del file fd, nel file tempFile (nota estrae solo il testo non il descrittore de capitolo)
// ritorna: 1 se è stato trovato il capitolo, 0 se l'id del capitolo non esiste
// Parametri fd file GA da leggere, descrittore del file temporaneo dove estrapolare il capitolo, id capitolo
int extract_from_IdCapGA(int fd, int tempFile, int id_cap)
{
	char cap[1024]={0x0};
	char tmp[1024]={0x0};
	char *occurence;
	short int b = 0;

	char *result;

	FILE *file = fdopen(fd, "rw");
	snprintf(cap, 1024, "%s%d", CHAPTERS, id_cap);

	fseek(file, 0, SEEK_SET);
	lseek(tempFile, 0, SEEK_SET);
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, cap)))
		{	
			b = 1;
			break;
		}
	}
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			fseek(file, 0, SEEK_SET);
			lseek(tempFile, 0, SEEK_SET);
			return b;
		}
		else
		{
			writen(tempFile, tmp, strlen(tmp));
		}
	}
	
	fseek(file, 0, SEEK_SET);
	lseek(tempFile, 0, SEEK_SET);
	return b;
}


// estrae tutto il file escluso il capitolo con descrittore chapter del file fd, nel file tempFile, ritorna: 1 se è stato trovato il capitolo, 
// 0 se l'id del capitolo non esiste
// Parametri fd file GA da leggere, descrittore del file temporaneo dove estrapolare il capitolo, id capitolo
int extract_excluse_IdCapGA(int fd, int tempFile, int id_cap)
{
	char cap[1024]={0x0};
	char tmp[1024]={0x0};
	char *occurence;

	char *result;
	short int b = 0;

				
	FILE *file = fdopen(fd, "rw");
	snprintf(cap, 1024, "%s%d", CHAPTERS, id_cap);

	fseek(file, 0, SEEK_SET);
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, cap)))
		{
			b = 1;
			printf("SSSS \n");
			break;
		}
		else
		{
			writen(tempFile, tmp, strlen(tmp));
		}
	}

	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			writen(tempFile, tmp, strlen(tmp));
			break;
		}
	}

	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		writen(tempFile, tmp, strlen(tmp));
	}

	fseek(file, 0, SEEK_SET);
	lseek(tempFile, 0, SEEK_SET);
	return b;
}



// estrae il capitolo numero pos del file fd , nel file tempFile, (nota estrae solo il testo),
// ritorna: 1 se è stato trovato il capitolo, 0 se l'id del capitolo non esiste
// Parametri fd file GA da leggere, descrittore del file temporaneo dove estrapolare il capitolo, posizione capitolo
int extract_from_PosCapGA(int fd, int tempFile, int pos)
{
	char cap[1024]={0x0};
	char tmp[1024]={0x0};
	char *occurence;
	char *result;
	int n_cap = 0;
	
	FILE *file = fdopen(fd, "rw");

	fseek(file, 0, SEEK_SET);

	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{	
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			++n_cap;
		}
		
		if( n_cap == pos)
			break;
	}
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			fseek(file, 0, SEEK_SET);
			lseek(tempFile , 0 , SEEK_SET);
			return 1;
		}
		else
		{
			writen(tempFile, tmp, strlen(tmp));
		}
	}

	fseek(file, 0, SEEK_SET);
	lseek(tempFile , 0 , SEEK_SET);
	return 0;
}


// sposta il capitolo con descrittore chapter del file fd , nella posizione del file pos file tempFile, 
// ritorna: 1 se è stato trovato il capitolo, 0 se l'id del capitolo non esiste
// Parametri fd file GA da leggere, descrittore del file sorgente, id del capitolo, posizione capitolo, cartella file temp
int move_PosCapGA(int fd, int temp_file , int id , int pos , char* tmp_path)
{
	lseek(temp_file , 0 , SEEK_SET);
	lseek(fd , 0 , SEEK_SET);
	
	int count = count_CapGA(fd);
	int dim_temp = strlen(tmp_path) + L_TEMP;

	if(pos > count-1)
		pos = -1;
		
	char path_filetemp1[dim_temp];

	int ft1 = create_fileTemp(tmp_path , path_filetemp1 , dim_temp);

	lseek(temp_file , 0 , SEEK_SET);
	lseek(fd , 0 , SEEK_SET);

	if (!extract_excluse_IdCapGA(fd, ft1, id) )
	{
		close(ft1);
		remove(path_filetemp1);
		lseek(temp_file , 0 , SEEK_SET);
		lseek(fd , 0 , SEEK_SET);
		return 0;
	}

	char path_filetemp2[dim_temp];

	int ft2 = create_fileTemp(tmp_path , path_filetemp2 , dim_temp);

	if (!extract_from_IdCapGA(fd, ft2, id) )
	{printf("fasfadsfasdfas \n");
		close(ft1);
		close(ft2);
		remove(path_filetemp1);
		remove(path_filetemp2);
		lseek(temp_file , 0 , SEEK_SET);
		lseek(fd , 0 , SEEK_SET);
		return 0;
	}

	char cap[1024]={0x0};
	char tmp1[1024]={0x0};
	char tmp2[1024]={0x0};
	char *occurence;
	char *result;
	int n_cap = 0;

	snprintf(cap, 1024, "%s%d", CHAPTERS, id);
	
	FILE *file1 = fdopen(ft1, "rw");

	fseek(file1, 0, SEEK_SET);
	
	FILE *file2 = fdopen(ft2, "rw");

	fseek(file2, 0, SEEK_SET);
	
	lseek(temp_file , 0 , SEEK_SET);
	lseek(fd , 0 , SEEK_SET);

	if(pos != -1)
	{
		while(file1!=NULL && fgets(tmp1, sizeof(tmp1), file1)!=NULL)
		{	
			if ((occurence = strstr(tmp1, CHAPTERS)))
			{
				++n_cap;
			}
		
			if( n_cap == pos)
			{
				writen(temp_file, cap, strlen(cap));	
				writen(temp_file, "\n", strlen("\n"));
			
				while(file2!=NULL && fgets(tmp2, sizeof(tmp2), file2)!=NULL)
				{	
					writen(temp_file, tmp2, strlen(tmp2));	
				}
				++n_cap;
			}
		
			writen(temp_file, tmp1, strlen(tmp1));	
		}
	}
	else
	{
		while(file1!=NULL && fgets(tmp1, sizeof(tmp1), file1)!=NULL)
		{	
			writen(temp_file, tmp1, strlen(tmp1));	
		}
		
		writen(temp_file, cap, strlen(cap));	
		writen(temp_file, "\n", strlen("\n"));
		
		while(file2!=NULL && fgets(tmp2, sizeof(tmp2), file2)!=NULL)
		{	
			writen(temp_file, tmp2, strlen(tmp2));	
		}
	}
	close(ft1);
	close(ft2);
	remove(path_filetemp1);
	remove(path_filetemp2);
	lseek(temp_file , 0 , SEEK_SET);
	lseek(fd , 0 , SEEK_SET);

	return 1;
}



// conta il numero di capitoli del file GA, ritorna: il numero di capitoli del file GA
// Parametri -> descrittore file
int count_CapGA(int fd)
{		

	FILE *file = fdopen(fd, "rw");
	fseek(file, 0, SEEK_SET);
	
	char *occurence;
	char tmp[1024]={0x0};
	int n_cap = 0;
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			++n_cap;
		}
	}

	fseek(file, 0, SEEK_SET);
	return n_cap;
}


// restituisce l'id di uno specifico capitolo del file GA
// Parametri -> descrittore file, posizione del capitolo
int get_id_pos(int fd, int pos)
{
	FILE *file = fdopen(fd, "rw");
	fseek(file, 0, SEEK_SET);
	char *occurence;
	char tmp[1024]={0x0};
	int n_cap = 0;
	int id;
	
	while(file!=NULL && fgets(tmp, sizeof(tmp), file)!=NULL)
	{
		if ((occurence = strstr(tmp, CHAPTERS)))
		{
			++n_cap;
		}
		if( n_cap == pos)
		{

			id = atoi(&tmp[strlen(CHAPTERS)] );
			fseek(file, 0, SEEK_SET);
			return id;
		}
	}
	fseek(file, 0, SEEK_SET);
	return -1;
}



// inserisci il capitolo con descrittore chapter del file fc , nella posizione del file pos file temp_file, 
// ritorna: l'id del capitolo inserito , 0 se c'e' qualche errore
// Parametri fd file GA da leggere, descrittore del file con il capitolo, file destinazione ,id del capitolo,  cartella file temp
int insert_CapGA(int fd, int fc , int temp_file , int pos , char* tmp_path)
{
	lseek(temp_file , 0 , SEEK_SET);
	lseek(fd , 0 , SEEK_SET);

	int count = count_CapGA(fd);
	int dim_temp = strlen(tmp_path) + L_TEMP;
	printf("pos = %d , count = %d\n" , pos , count);	
	if(pos > count-1)
		pos = -1;
	
	printf("pos = %d , count = %d\n" , pos , count);	
	char path_filetemp1[dim_temp];
	

	char cap[1024]={0x0};
	char tmp1[1024]={0x0};
	char tmp2[1024]={0x0};
	char *occurence;
	char *result;
	int n_cap = 0;
	int id;
	
	FILE *file1 = fdopen(fd, "rw");

	fseek(file1, 0, SEEK_SET);
	
	FILE *file2 = fdopen(fc, "rw");

	fseek(file2, 0, SEEK_SET);
	
	fgets(tmp1, sizeof(tmp1), file1);
	writen(temp_file, tmp1, strlen(tmp1));	
	
	fgets(tmp1, sizeof(tmp1), file1);
	id = atoi(&tmp1[3]);
	++id;
	memset(tmp1, '\0' , 1024 );
	snprintf(tmp1, 1024, "%s%d\n", "&&&", id);
	writen(temp_file, tmp1, strlen(tmp1));	
	
	snprintf(cap, 1024, "%s%d", CHAPTERS, id);
	
	if(pos != -1)
	{
		while(file1!=NULL && fgets(tmp1, sizeof(tmp1), file1)!=NULL)
		{	
			if ((occurence = strstr(tmp1, CHAPTERS)))
			{
				++n_cap;
			}
		
			if( n_cap == pos)
			{
				writen(temp_file, cap, strlen(cap));	
				writen(temp_file, "\n", strlen("\n"));
			
				while(file2!=NULL && fgets(tmp2, sizeof(tmp2), file2)!=NULL)
				{	
					writen(temp_file, tmp2, strlen(tmp2));	
				}
				++n_cap;
			}
		
			writen(temp_file, tmp1, strlen(tmp1));	
		}
	}
	else
	{
		while(file1!=NULL && fgets(tmp1, sizeof(tmp1), file1)!=NULL)
		{	
			writen(temp_file, tmp1, strlen(tmp1));	
		}
		
		writen(temp_file, cap, strlen(cap));	
		writen(temp_file, "\n", strlen("\n"));
		
		while(file2!=NULL && fgets(tmp2, sizeof(tmp2), file2)!=NULL)
		{	
			writen(temp_file, tmp2, strlen(tmp2));	
		}
	}

	fseek(file1, 0, SEEK_SET);
	fseek(file2, 0, SEEK_SET);
	lseek(temp_file , 0 , SEEK_SET);
	return id;
}

// verifica che non ci siano caratteri speciali nel testo, restituisce 1 se il testo ne è privo, 0 altrimenti
// Parametri -> descrittore del file
int anal_ga(int fd) 
{
	char tmp1[1024]={0x0};
	char *occurence;
	FILE *file1 = fdopen(fd, "rw");

	fseek(file1, 0, SEEK_SET);
	
	while(file1!=NULL && fgets(tmp1, sizeof(tmp1), file1)!=NULL)
	{	
		if ((occurence = strstr(tmp1, CHAPTERS) ) || (occurence = strstr(tmp1, "&&&") ))
		{
			fseek(file1, 0, SEEK_SET);
			return 0;
		}	
	}
	
	fseek(file1, 0, SEEK_SET);
	return 1;
}
