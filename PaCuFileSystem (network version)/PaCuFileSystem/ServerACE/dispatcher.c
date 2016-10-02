#include "header.h"



//	porzione di codice del dispatcher che rimane in ascolto su una porta 
void *func_dispatcher_s()
{
	int t ;	
	int listensd = bootServer(port_server , backlog_server);

	printf("Creating disp_DNS %lu\n", (unsigned long)pthread_self());		
	pthread_t threads[thread_helper_s];

	for(t=0;t < thread_helper_s;t++)
	{
		int rc ;
	  	rc = pthread_create (&threads[t], NULL, func_helper_s, (void *)&listensd);
	  	if (rc)
		{
		  	printf("ERROR; return code from pthread_create() is %d\n",rc);
		  	exit(-1);
	  	}
	}
	
	sem_wait(sem_exit_disp_s);

	exit_helper_s = 1;	

	sleep(time_helper_s);
	for(t=0;t < thread_helper_s;t++)
	{	
  		pthread_cancel(threads[t] );
	}			
	close(listensd);

	pthread_exit(NULL);

}



//	porzione di codice del dispatcher che rimane in ascolto su una porta PR
void *func_dispatcher_pr()
{
  int t ;	
  in_port_t port = htons(*port_server_pr); 

	int listensd = bootServer(&port , backlog_server);

	printf("Creating disp_DNS %lu\n", (unsigned long)pthread_self());		
	pthread_t threads[thread_prSrv];

	for(t=0;t < thread_prSrv;t++)
	{
		int rc ;
  	rc = pthread_create (&threads[t], NULL, func_helper_pr, (void *)&listensd);
  	if (rc)
		{
	  	printf("ERROR; return code from pthread_create() is %d\n",rc);
	  	exit(-1);
  	}
	}

	sem_wait(sem_exit_disp_pr);

	exit_helper_pr = 1;	

	sleep(time_helper_s);
	for(t=0;t < thread_prSrv;t++)
	{	
  		pthread_cancel(threads[t] );
	}			
	close(listensd);

	pthread_exit(NULL);

}



void *func_dispatcher_c()
{
	int listensd  = bootServer(port_client , backlog_client);
 	int t ;	

	printf("Creating disp_DNS %lu\n", (unsigned long)pthread_self());		
	pthread_t threads[thread_helper_c];

	for(t=0;t < thread_helper_c;t++)
	{
		int rc ;

  	rc = pthread_create (&threads[t], NULL, func_helper_c, (void *)&listensd);
  	
  	if (rc)
		{
     	printf("ERROR; return code from pthread_create() is %d\n",rc);
     	exit(-1);
  	}
	}

	sem_wait(sem_exit_disp_c);

	exit_helper_c = 1;

	sleep(time_helper_c);
	for(t=0;t < thread_helper_c;t++)
	{	
  		pthread_cancel(threads[t] );
	}		
	close(listensd);

	pthread_exit(NULL);

}


//	porzione di codice degli helper che si contendono le richieste di connessione server
void *func_helper_s(void *l)
{	
	int listensd = *(int *)l;
	int connsd;

	printf("Creating thread_s %lu\n", (unsigned long)pthread_self());	

	for ( ; ; ) 
	{
		short int var_exit;
		
		var_exit = exit_helper_s;	

		if(var_exit)
		{
				pthread_exit(NULL);	
		}


		sem_wait(sem_accept_s);

			connsd = connessioneServer(listensd);

		sem_post(sem_accept_s);		 
		
		pool_ComunicationServ(connsd);
	}

	pthread_exit(NULL);
}



//	porzione di codice degli helper che si contendono le richieste di connessione server
void *func_helper_pr(void *l)
{	
	int listensd = *(int *)l;
	int connsd;
	int rc;

	printf("Creating thread_pr %lu\n", (unsigned long)pthread_self());	

	for ( ; ; ) 
	{
		short int var_exit;
		
		var_exit = exit_helper_pr;	

		if(var_exit)
		{
				pthread_exit(NULL);	
		}


		sem_wait(sem_accept_pr);

			connsd = connessioneServer(listensd);

		sem_post(sem_accept_pr);	
		
		rc = ctrl_messagePR(connsd);
		close(connsd);
	}

	pthread_exit(NULL);
}



//	porzione di codice degli helper che si contendono le richieste di connessione client
void *func_helper_c(void *l)
{	
	int listensd = * (int* )l;
	int connsd;
	int rc;

	printf("Creating thread_c %lu\n", (unsigned long)pthread_self());	

	for ( ; ; ) 
	{
		short int var_exit;
		
		var_exit = exit_helper_c;	

		if(var_exit)
		{
				pthread_exit(NULL);	
		}

		sem_wait(sem_accept_c);
			connsd = connessioneServer(listensd);
		sem_post(sem_accept_c);	

		printf("init thread\n");
		rc = init_message(connsd);
		close(connsd);
	}

	pthread_exit(NULL);
}
