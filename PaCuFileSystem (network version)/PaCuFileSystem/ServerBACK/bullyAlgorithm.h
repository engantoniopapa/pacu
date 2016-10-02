typedef struct
{
   char nomeDominio[_DIM_DOM+1];
   short int status;
}electionStatus;


typedef struct
{
	Server* server;
	char* dominio;
	int *countOK;
	char *primary;
}ContainerBully_th;



electionStatus *init_electionStatus( int *); //inizializza la struttura electionStatus

int start_bully(char * , char* , char* , Server * , int);

Server *vistaBully(char *, char *,	int *); // crea la vista Bully per un dato dominio
