#include "header.h"


void bootstrap(int scelta); // funzione di bootstrap del sistema

int main(int argc, char *argv[])
{

	int scelta ;
	srand(time(NULL));

	color = (char*) malloc ( sizeof(char) * 9 );
	strcpy( color , "\e[1;34m" );
	
	sem_count_pp = make_Two_rooms();
	id_pp = 0;
	
	sem_listCl_pp = make_Two_rooms(); 
	listCl_pp = init_listPong( _LIST_PP);
	
	exit_th_pp = 0;
	
	scelta = menu() ;

	if( scelta == 1 || scelta == 66)
	{
		bootstrap(scelta);
	}	


	figlet2();
  return 0;
}


void bootstrap(int scelta)
{

		
		if( scelta == 66)
		{
			int rc ;
			
			conf_am();
			rc = init_am_fase();
		}
		else
		{	
			conf_client();
			
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
			
			int socksd;
			int  i ;
			int state_boot = 0;

			if( (socksd = connection_serverBoot()) > 0 )
			{ 
				state_boot = init_boot_fase(socksd);
			} 

			if(!cp_rewrite_file(PATH_DB_PRIMARY , PATH_DB_TMP_PRI , max_line ) )
				exit(0);

			get_list_server(PATH_DB_TMP_PRI);

		
			for(i = 0 ; i < len_list_server ; ++i)
			{
						printf( "%d - name = %s  , ip = %s , port = %d \n" , i , list_server[i].name , list_server[i].ip , list_server[i].port );
			}
			/*
			state_boot = 2 ; // prova!!!!!!!!!!!!!!!!!!!!!!!!!

			if(state_boot == 2 || state_boot ==4)
			{
				update_domini();
			}*/
			
			if(!cp_rewrite_file(PATH_DB_DOMINI , PATH_DB_TMPDOM , max_line ) )
				exit(0);
				

			get_list_domini(PATH_DB_TMPDOM);

		/*	for(i = 0 ; i < len_list_domini ; ++i)
			{
						printf( "%d - name = %s   \n" , i , list_domini[i].name );
			}*/
			
			start_recovery(PATH_DB_LOG);
			
			int rc;
			pthread_t th_pp; // descrittore thread ping-pong udp per comunicazione client-server
			Unit_TH_Pong uPong_c;
			
			set_Unit_TH_Pong( &uPong_c , listCl_pp , sem_listCl_pp , my_ip , *port_client_pp);
			rc = pthread_create (&th_pp, NULL, thread_rcvPP_UDP, (void *) &uPong_c); 
			if (rc)
			{
		 		printf("ERROR; return code from pthread_create() is %d\n",rc);
		 		exit(-1);
			}
		
			start_console();
			
			exit_th_pp = 1;
			
			sleep(time_helper_pp);
			
			pthread_cancel(th_pp);
		}
		
		if( scelta == 66)
		{
			free(port_client_pp);
			free(in_path);
			free(tmp_path);
			free(iterface_network);
			free_Two_rooms(sem_listCl_pp);
			free_Two_rooms(sem_count_pp);
			free(color);
		}
		else
		{
			free(port_client_pp);
			free(in_path);
			free(tmp_path);
			free(iterface_network);
			remove_listPong(listCl_pp);
			free_Two_rooms(sem_listCl_pp);
			free_Two_rooms(sem_count_pp);
			free(color);
			free_list_server();
			free_list_domini();
		}
}

	
