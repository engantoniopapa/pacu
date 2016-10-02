
#include "header.h"

void free_all(); // libera tutta la memoria heap

void aggiorna_crash(int val); // aggiorna la variabile di stato crash

int main(int argc, char *argv[])
{
	int rc ;
	pthread_t dispatcher_c; // descrittore dispatcher client
	pthread_t dispatcher_s; // descrittore dispatcher server
	pthread_t dispatcher_pr; // descrittore dispatcher server PR
	pthread_t dispatcher_ppC; // descrittore dispatcher client ping-pong
	pthread_t dispatcher_ppS; // descrittore dispatcher server ping-pong
	pthread_t ctrl_bully; // descrittore dispatcher bully

	sem_vista = make_Writers_Reader();
	sem_list_serv = make_Writers_Reader();
	sem_list_dom_serv = make_Writers_Reader();

	sem_bully = make_Two_rooms(); 
	
	get_conf(); //carico la configurazione di sistema

	sem_accept_c =  make_semaphore(1) ;	// mutex per il lock sull'accept	per i client
	sem_accept_s =  make_semaphore(1) ;	// mutex per il lock sull'accept	per i server
	
	sem_accept_pr =  make_semaphore(1) ;	// mutex per il lock sull'accept	per i server PR
	
	sem_accept_ppCl = make_semaphore(1);	// mutex per il lock sulla recv dei pong client
	sem_accept_ppSrv = make_semaphore(1);	// mutex per il lock sulla recv dei pong server
	
	sem_exit_disp_c = make_semaphore (0); // inizializzazione signal per la terminazione del dispatcher per i client
	sem_exit_disp_s = make_semaphore (0); // inizializzazione signal per la terminazione del dispatcher per i server
	sem_exit_disp_pr = make_semaphore (0); // inizializzazione signal per la terminazione del dispatcher per i server PR
	sem_exit_disp_ppC = make_semaphore (0); // inizializzazione signal per la terminazione del dispatcher ping-pong  per i client
	sem_exit_disp_ppS = make_semaphore (0); // inizializzazione signal per la terminazione del dispatcher ping-pong  per i server
	
	sem_count = make_Two_rooms();
	
	id_count = max_numTR(PATH_DB_LOG  ," Log" , id_server );

	if(id_count < 0 )
	{
			exit(0);
	}
	
	printf("----id_count = %d \n", id_count);
	++id_count;
	
	sem_count_pp = make_Two_rooms();
	id_pp = 0;
	
	sem_listCl_pp = make_Two_rooms(); 
	sem_listServ_pp= make_Two_rooms(); 
	
	exit_helper_c = 0;// inizializzazione varibile per segnalare l'uscita agli helper del dispatcher per i client
	exit_helper_s = 0;// inizializzazione varibile per segnalare l'uscita agli helper del dispatcher per i server
	exit_helper_pr = 0;// inizializzazione varibile per segnalare l'uscita agli helper del dispatcher per i server PR
	exit_th_pp = 0; // inizializzazione varibile per segnalare l'uscita ai thread udp
	exit_bully = 0 ;// inizializzazione varibile per segnalare l'uscita al disp bully
	
	listCl_pp = init_listPong( _L_PP_CL);
	listServ_pp = init_listPong( _L_PP_SERV);

	if(!cp_rewrite_file(PATH_DB_DOMINI , PATH_DB_TMPDOM , max_line ) )
	{
		free_all();
		exit(-1);
	}

	if(start_recovery(crash) == 0)
	{
		printf("Errore: Il server non può essere riportato in uno stato coerente, senza l'ausilio dell'amministratore :( !! \n");
		free_all();
		exit(-1);
	}
	
	aggiorna_crash(1);
	
	//cancellazione file temporanei
	int pid;
	pid = fork();
	
	if( pid == 0)
	{
		execlp("rm" , "rm"  , "-rf" ,  tmp_path , NULL );
		fprintf (stderr, "Exec failed.\n");
	}
	
	wait(NULL);
	
	if( mkdir(tmp_path, 0777) < 0) 
	{
		if( errno == EEXIST)
		{
			printf("exist dir \n");
		}
		else
		{
	 	fprintf (stderr, "mkdir failed.\n");
	 	exit(0);}
	}
		
	printf("Fine bootstrap, avvio Threads \n");
	
	rc = pthread_create (&dispatcher_s, NULL, func_dispatcher_s, (void *)NULL); // creazione del dispatcher
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	free_all();
   	exit(-1);
	}
	

	rc = pthread_create (&dispatcher_c, NULL, func_dispatcher_c, (void *)NULL); // creazione del dispatcher
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	free_all();
   	exit(-1);
	}
	
	rc = pthread_create (&dispatcher_pr, NULL, func_dispatcher_pr, (void *)NULL); // creazione del dispatcher
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	free_all();
   	exit(-1);
	}
	
	rc = pthread_create (&dispatcher_ppS, NULL, dispSrv_rcvPP_UDP, (void *)NULL); // creazione del dispatcher
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	free_all();
   	exit(-1);
	}
	

	rc = pthread_create (&dispatcher_ppC, NULL, dispCl_rcvPP_UDP, (void *)NULL); // creazione del dispatcher
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	free_all();
   	exit(-1);
	}
	
	
	
	
	primariesElection = init_electionStatus( &len_primariesElection);
	
	printf("------len election = %d \n" , len_primariesElection );
	rc = pthread_create (&ctrl_bully, NULL, control_bully, (void *)NULL); // creazione del dispatcher
	if (rc)
	{
   	printf("ERROR; return code from pthread_create() is %d\n",rc);
   	free_all();
   	exit(-1);
	}
	
	
	short int boolean = 1;
	char risp[10];
	sleep(2);
	
	while(boolean)
	{
		setvbuf(stdin, (char*)NULL, _IONBF, 0); //svuota l'stdin
		memset( risp , '\0' ,10 );
		puts("\nInserisci exit per terminare il server: ");
		fgets( risp , 9 , stdin);
		puts("\n");
		
		if((strncmp(risp ,"exit" , 4)== 0) || (strncmp(risp ,"Exit" , 4)== 0) )
		{
			boolean = 0;
		}
		else
		{
			boolean = 1;
		}
	}	
	
	aggiorna_crash(0);
	
	exit_bully = 1;
	sem_post(sem_exit_disp_c);	
	sem_post(sem_exit_disp_s);	
	sem_post(sem_exit_disp_pr);	
	sem_post(sem_exit_disp_ppC);	
	sem_post(sem_exit_disp_ppS);	

	pthread_join(dispatcher_c, NULL ); // aspetto la terminazione del dispatcher client
	pthread_join(dispatcher_s, NULL ); // aspetto la terminazione del dispatcher server
	pthread_join(dispatcher_pr, NULL ); // aspetto la terminazione del dispatcher server
	pthread_join(dispatcher_ppC, NULL ); // aspetto la terminazione del dispatcher ping-pong client
	pthread_join(dispatcher_ppS, NULL ); // aspetto la terminazione del dispatcher ping-pong server
	pthread_join(ctrl_bully, NULL ); // aspetto la terminazione del dispatcher ping-pong server
	
	free_all();
	
	return 0;
}



void free_all()
{
	free(color);
	free(name_server);
	free(string_dom_serv);
	free(list_server);
	free(port_client);
	free(port_server);
	free(port_client_pp);
	free(port_server_pp);
	free(port_server_pr);
	free(fs_path);
	free(tmp_path);
	free(iterface_network);
	remove_listPong(listCl_pp);
	remove_listPong(listServ_pp);
	free_Two_rooms(sem_count);
	free_Two_rooms(sem_count_pp); 
	free_Two_rooms(sem_listCl_pp);
	free_Two_rooms(sem_listServ_pp);
	free_Two_rooms(sem_bully);
	free_semaphore(sem_accept_c);	
	free_semaphore(sem_accept_s);
	free_semaphore(sem_accept_pr);
	free_semaphore(sem_accept_ppCl);
	free_semaphore(sem_accept_ppSrv);
	free_semaphore(sem_exit_disp_s);
	free_semaphore(sem_exit_disp_c);
	free_semaphore(sem_exit_disp_pr);
	free_semaphore(sem_exit_disp_ppS);
	free_semaphore(sem_exit_disp_ppC);
	free_Writers_Reader(sem_vista);
	free_Writers_Reader(sem_list_serv);
	free_Writers_Reader(sem_list_dom_serv);
	freeForLocking(thread_helper_c  , lista_File , list_mutex );
	free(primariesElection);
}



// aggiorna la variabile di stato crash
// Parametri -> valore con cui aggiornare la variabile di stato
void aggiorna_crash(int val)
{
	int rc;
	char *stringa_errore;
	char query_string[501];
	DB *db_conf; // descrittore del DB Conf
	
	db_conf = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR  , sizeof(char) );
	memset(query_string, '\0' , 500); 
	
	rc = open_db_rw( db_conf , PATH_DB_CONF , stringa_errore);
  if( !rc )
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }
	
	snprintf(query_string , 500 ," update parametri set valore = %d where nome ='crash' ", val);
	
	if( generic_op_db(db_conf , query_string , stringa_errore) == -1)
	{
		printf("%s \n" , stringa_errore );
	}
	
	if( !close_db(db_conf, stringa_errore))
	{
		fprintf(stderr, "%s\n", stringa_errore);
		exit(1);
	}	
	
	free(db_conf);
	free(stringa_errore);
}
