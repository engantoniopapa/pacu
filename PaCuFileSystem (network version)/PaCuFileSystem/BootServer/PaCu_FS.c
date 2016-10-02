#include "header.h"


void bootstrap(); // funzione di bootstrap del sistema

int main(int argc, char *argv[])
{


	bootstrap();
  return 0;
}


void bootstrap()
{
	int rc ;
	pthread_t dispatcher; // descrittore dispatcher
	pthread_t thread_am; // descrittore gestore porta & connessione amministratore

	port_client =  ( in_port_t *) malloc (sizeof(in_port_t )); // porta per la comunicazione client - server
	port_am =  ( in_port_t *) malloc (sizeof(in_port_t )); // porta per la comunicazione client - server

	get_conf(); //carico la configurazione di sistema

	sem_accept =  make_semaphore(1) ;	// mutex per il lock sull'accept	
	sem_exit_disp = make_semaphore (0); // inizializzazione signal per la terminazione del dispatcher_serverboot
	exit_helper = 0;// inizializzazione varibile per segnalare l'uscita agli helper del disptacher boot

	printf("Fine bootstrap, avvio Threads \n");
	
	rc = pthread_create (&thread_am, NULL, func_thread_am, (void *)NULL); // creazione del thread amministratore
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	exit(-1);
	}
	

	rc = pthread_create (&dispatcher, NULL, func_dispatcher, (void *)NULL); // creazione del dispatcher
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	exit(-1);
	}

	pthread_join(thread_am, NULL ); // aspetto la terminazione del thread amministratore

	sem_post(sem_exit_disp);// signal di terminazione ai dispatcher
	pthread_join(dispatcher, NULL ); // aspetto la terminazione del dispatcher

	free_all(); // libero la memoria allocata per le strutture
		
}

	
