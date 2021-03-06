#include "header.h"


// Avvia l'operazione di recovery, ritorna 1 se l'operazione va a buon fine, altrimenti 0
// Parametri -> stato di chiusura del precedente avvio
int start_recovery(short int state)
{
	TrLog *rec_tr;
	char str_cond[_DIM_COND] = {0x0};
	char str_groupby[_DIM_COND] = {0x0};
	char stringa_errore[_MAX_STRING_ERROR] = {0x0};
	int len;
	int i ;

	if( !update_log_state(PATH_DB_LOG , LOG_PREPARA  , LOG_ABORT) )
	{
		printf("\e[1;32mErrore nell'aggiornamento dei Log sul DB\033[m\n");
		exit(0);
	}
	
	snprintf(str_cond , _DIM_COND -1 , " where state = %d "  , LOG_PRONTO );
	strncpy(str_groupby , " group by id_p , id_c " , _DIM_COND -1 );
	
	rec_tr = select_recTR(PATH_DB_LOG, "Log" , str_cond , str_groupby , stringa_errore , &len);

	if(len <= 0)
	{ ; }
	else
	{
		for( i = 0 ; i < len ; ++i)
		{
			if( id_server == rec_tr[i].id_p) // recovery pronto primary
			{
				update_log(PATH_DB_LOG , rec_tr[i].id_p , rec_tr[i].id_c , LOG_ABORT);
			
				if(rec_tr[i].type == _TR_UPLOAD ) // redo primary sull'operazione di upload
				{		
					TrLog * det_tr;
					int rc;
					char stringa_errore[_MAX_STRING_ERROR] = {0x0};
					
					det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr[i].id_p,rec_tr[i].id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);
					
					if( det_tr->list[0].opt1 == 1)
					{
						cp_rewrite_file(det_tr->list[0].res2 ,  rec_tr[i].local_res ,max_line );	
					}
					else
					{
						if( isFile(rec_tr[i].local_res) )
							remove(rec_tr[i].local_res);
					}
					
					free(det_tr->list);
					free(det_tr);
				}
			
				if(rec_tr[i].type == _TR_RM)// redo primary sull'operazione di remove
				{
					if( !isFile(rec_tr[i].local_res) )
					{
						TrLog * det_tr;
						int rc;
						char stringa_errore[_MAX_STRING_ERROR] = {0x0};
					
						det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr[i].id_p,rec_tr[i].id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);
					
						cp_rewrite_file(det_tr->list[0].res2 , rec_tr[i].local_res ,max_line );	
						
						free(det_tr->list);
						free(det_tr);
					}
				}
			
				if(rec_tr[i].type == _TR_MKDIR)// redo primary sull'operazione di create directory
				{
					if( isDir(rec_tr[i].local_res) )
						removeDirectory(rec_tr[i].local_res);	
				}
			
				if(rec_tr[i].type == _TR_RM_DIR)// redo primary sull'operazione di remove directory
				{
					if( !isDir(rec_tr[i].local_res) )
						createDirectory(rec_tr[i].local_res);	
				}
			
				if(rec_tr[i].type ==_TR_NEW_GA)// redo primary sull'operazione create new file GA
				{
					if( isFile(rec_tr[i].local_res) )
						remove(rec_tr[i].local_res);
				}
			
				// redo primary sulle operazioni di create new cap, move cap e remove cap relative ai file GA
				if(rec_tr[i].type ==_TR_NEW_CAP || rec_tr[i].type == _TR_MV_CAP || rec_tr[i].type ==_TR_RM_CAP)
				{
					if( isFile(rec_tr[i].local_res) )
					{					
						TrLog * det_tr;
						int rc;
						char stringa_errore[_MAX_STRING_ERROR] = {0x0};
					
						det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr[i].id_p,rec_tr[i].id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);
						
						cp_rewrite_file(det_tr->list[0].res2 ,  rec_tr[i].local_res ,max_line );	
						
						free(det_tr->list);
						free(det_tr);
					}
				}
			
				if(rec_tr[i].type == _TR_MOD_CAP)	// redo primary sull'operazione di mod cap, relativa ai file GA
				{
					if( isFile(rec_tr[i].local_res) )
					{	
						TrLog * det_tr;
						int rc;
						char stringa_errore[_MAX_STRING_ERROR] = {0x0};
					
						det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr[i].id_p,rec_tr[i].id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);
					
						List_ModCap list[det_tr->length];
						int k ;
					
						for( k= 0 ; k < det_tr->length ; k++)
						{
							list[k].id_cap = det_tr->list[k].opt1;
							memset(list[k].path_mod_cap , '\0' , _DIM_RES);
							memset(list[k].path_sav_cap , '\0' , _DIM_RES);
							strcpy(list[k].path_sav_cap, det_tr->list[k].res3);
						}
		
						redo_mod_cap(list , rec_tr[i].length , rec_tr[i].local_res );
					
						free(det_tr->list);
						free(det_tr);
					}	
				}
			
			}
			else // recovery pronto replica
			{
				int connsd;
				connsd = connect_ServerServer(rec_tr[i].ip , *port_server_pr , 3) ;
				int contatta_rep = 0;
				
				if( connsd > 0)
				{	
					if(recovery_pronto_replica_primary(connsd , &rec_tr[i]) != 1)
					{
						contatta_rep = 1;
					}	
					close(connsd);
				}
				else
				{
					contatta_rep = 1;
				}
				
				if(contatta_rep)
				{
					Server *list_vista;
					int len_vista;
					char name_dom[_DIM_DOM+1] ={0x0};
					
					int len_fs;
					len_fs = strlen(fs_path);
					char *buffer, *buffer2;
					buffer = rec_tr[i].local_res + len_fs + 1;
					buffer2 = strstr(buffer, "/");
					strncpy(name_dom, buffer, buffer2 - buffer);

					list_vista = vistaServer(name_dom , PATH_DB_TMPDOM,	&len_vista ) ;
	
					if(len_vista > 0) 
					{ 
						recovery_pronto_replica_replica(&rec_tr[i] , list_vista , len_vista );
					} 
				}
			}
		}
	}
	
	free(rec_tr);
	return 1;
}



// gestisce lo stato pronto sul log di una replica contattando il primary, ritorna 1 se l'operazione riesce altrimenti 0
// Parametri -> connessione al server da contattare, struttura contenente informazioni generiche sul log
int recovery_pronto_replica_primary(int connsd , TrLog *rec_tr)
{
	PR_Message* msg;
	msg = init_PR_Message( _PR_LOG_STATE , "\0",  "\0" , rec_tr->id_p, rec_tr->id_c , "\0");
	
	set_timeout(connsd  , timeout_pr  , 0  , timeout_pr  , 0 );	
	
	if(send_PR_Message(connsd , msg ) != 1 )
	{
		close(connsd);
		free(msg);
		return 0;
	}
	else
	{
		free(msg);
		msg = (PR_Message*) calloc( 1 , sizeof(PR_Message) );
		
		int rc = recv_PR_Message(connsd , msg ) ;
		if( rc != 1 && msg->pay_desc != _PR_LOG_STATE )
		{
			close(connsd);
			free(msg);
			return 0;
		}
		else
		{
			if(msg->opt1 == LOG_ABORT)
			{
				update_log(PATH_DB_LOG , rec_tr->id_p, rec_tr->id_c , LOG_ABORT);
				
				TrLog * det_tr;
				int rc;
				char stringa_errore[_MAX_STRING_ERROR] = {0x0};

				det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr->id_p, rec_tr->id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);
				
				if(rec_tr->type == _TR_UPLOAD ) // redo replica sull'operazione di upload
				{						
					if( det_tr->list[0].opt1 == 1)
					{
						cp_rewrite_file(det_tr->list[0].res2 ,  rec_tr->local_res ,max_line );	
					}
					else
					{
						if( isFile(rec_tr->local_res) )
							remove(rec_tr->local_res);
					}
				}
				
				if(rec_tr->type == _TR_RM)// redo replica sull'operazione di remove
				{
					if( !isFile(rec_tr->local_res) )
					{				
						cp_rewrite_file(det_tr->list[0].res2 , rec_tr->local_res ,max_line );	
					}
				}
				
				if(rec_tr->type == _TR_MKDIR)// redo replica sull'operazione di create directory
				{
					if( isDir(rec_tr->local_res) )
						removeDirectory(rec_tr->local_res);	
				}

				if(rec_tr->type == _TR_RM_DIR)// redo replica sull'operazione di remove directory
				{
					if( !isDir(rec_tr->local_res) )
						createDirectory(rec_tr->local_res);	
				}
					
				if(rec_tr->type == _TR_MOD_CAP)	// redo primary sull'operazione di mod cap, relativa ai file GA
				{
					if( isFile(rec_tr->local_res) )
					{	

						List_ModCap list[det_tr->length];
						int k ;
						
						for( k = 0 ; k < det_tr->length ; k++)
						{
							list[k].id_cap = det_tr->list[k].opt1;
							memset(list[k].path_mod_cap , '\0' , _DIM_RES);
							memset(list[k].path_sav_cap , '\0' , _DIM_RES);
							strcpy(list[k].path_sav_cap, det_tr->list[k].res3);
						}										

						int fl = open(rec_tr->local_res, O_RDWR);
						for( k = 0  ;   k < det_tr->length ; ++k)// scrittura sul file originale
						{
							int fc;
							int fr;
							fc = open(list[k].path_mod_cap, O_RDWR);
							char result_file[_DIM_RES] = {0x0};

							mod_CapGA(fl, fc , result_file ,list[k].id_cap , tmp_path);

							close(fc);
							fr = open(result_file, O_RDWR);
							write_res_on_res(fr ,fl , max_line );
							close(fr);
							remove(result_file);
						}
						close(fl);
					}	
				}	
								
				free(det_tr->list);
				free(det_tr);	
			}
			
			if(msg->opt1 == LOG_COMMIT)
			{
				update_log(PATH_DB_LOG , rec_tr->id_p, rec_tr->id_c , LOG_COMMIT);
				
				TrLog * det_tr;
				int rc;
				char stringa_errore[_MAX_STRING_ERROR] = {0x0};

				det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr->id_p,rec_tr->id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);

				if(rec_tr->type == _TR_UPLOAD ) // conclusione della transazione sulla replica sull'operazione di upload
				{						
						if(isFile(det_tr->list[0].res3))
							cp_rewrite_file(det_tr->list[0].res3 ,  rec_tr->local_res ,max_line );	
				}
				
				if(rec_tr->type == _TR_RM)// conclusione della transazione sulla replica sull'operazione di remove
				{
					if( !isFile(rec_tr->local_res) )
					{				
						remove(rec_tr->local_res);	
					}
				}
				
				if(rec_tr->type == _TR_MKDIR)// conclusione della transazione sulla replica sull'operazione di create directory
				{
					if( !isDir(rec_tr->local_res) )
						createDirectory(rec_tr->local_res);
				}

				if(rec_tr->type == _TR_RM_DIR)// conclusione della transazione sulla replica sull'operazione di remove directory
				{
					if( isDir(rec_tr->local_res) )
						removeDirectory(rec_tr->local_res);	
				}
					
				if(rec_tr->type == _TR_MOD_CAP)	// conclusione della transazione sulla primary sull'operazione di mod cap, relativa ai file GA
				{
					if( isFile(rec_tr->local_res) )
					{	

						List_ModCap list[det_tr->length];
						int k ;

						for( k = 0 ; k < det_tr->length ; k++)
						{
							list[k].id_cap = det_tr->list[k].opt1;
							memset(list[k].path_mod_cap , '\0' , _DIM_RES);
							memset(list[k].path_sav_cap , '\0' , _DIM_RES);
							strcpy(list[k].path_sav_cap, det_tr->list[k].res3);
						}

						redo_mod_cap(list , rec_tr->length , rec_tr->local_res );
					}	
				}	
				
				free(det_tr->list);
				free(det_tr);	
			}
			return 1;
		}
	}
	return 0;				
}



// funzione invocata da una replica o da un nuovo primary che ha lo stato log  uguale a pronto, quando il primary non risulta prensente, 
// vengono contattate le altre repliche se una di queste ha uno stato diverso da pronto l'entità che ha invocato la funzione aggiorna lo stato 
// e restituisce 1 se aggiorna con COMMIT -1 se aggiorna con ABORT altrimenti restituisce 0;
int recovery_pronto_replica_replica( TrLog *rec_tr , Server *vista , int len_vista )
{
	int q = 0;
	short int riuscita = 0;
	int connsd;
	
	while( riuscita == 0 && q < len_vista)
	{
		connsd = connect_ServerServer(vista->ip , *port_server_pr , 3) ;
		
		if( connsd > 0)
		{	
			PR_Message* msg;
			msg = init_PR_Message( _PR_LOG_STATE , "\0",  "\0" , rec_tr->id_p, rec_tr->id_c , "\0");
	
			set_timeout(connsd  , timeout_pr  , 0  , timeout_pr  , 0 );	
	
			if(send_PR_Message(connsd , msg ) != 1 )
			{
				free(msg);
			}
			else
			{
				free(msg);
				msg = (PR_Message*) calloc( 1 , sizeof(PR_Message) );
		
				int rc = recv_PR_Message(connsd , msg ) ;
				if( rc != 1 && msg->pay_desc != _PR_LOG_STATE )
				{
					free(msg);
				}
				else
				{
					if(msg->opt1 == LOG_COMMIT)
					{	
						riuscita = 1;
					}
					
					if(msg->opt1 == LOG_ABORT)
					{	
						riuscita = -1;
					}
					free(msg);
				}
			}
			close(connsd);
		}
		q++;
	}
	
	// se tutti hanno pronto se ne occupa il nuovo primary primary
	if(riuscita == 1)
	{
		update_log(PATH_DB_LOG , rec_tr->id_p, rec_tr->id_c , LOG_COMMIT);
				
		TrLog * det_tr;
		int rc;
		char stringa_errore[_MAX_STRING_ERROR] = {0x0};

		det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr->id_p,rec_tr->id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);

		if(rec_tr->type == _TR_UPLOAD ) // conclusione della transazione sulla replica sull'operazione di upload
		{						
				if(isFile(det_tr->list[0].res3))
					cp_rewrite_file(det_tr->list[0].res3 ,  rec_tr->local_res ,max_line );	
		}
		
		if(rec_tr->type == _TR_RM)// conclusione della transazione sulla replica sull'operazione di remove
		{
			if( !isFile(rec_tr->local_res) )
			{				
				remove(rec_tr->local_res);	
			}
		}
		
		if(rec_tr->type == _TR_MKDIR)// conclusione della transazione sulla replica sull'operazione di create directory
		{
			if( !isDir(rec_tr->local_res) )
				createDirectory(rec_tr->local_res);
		}

		if(rec_tr->type == _TR_RM_DIR)// conclusione della transazione sulla replica sull'operazione di remove directory
		{
			if( isDir(rec_tr->local_res) )
				removeDirectory(rec_tr->local_res);	
		}
			
		if(rec_tr->type == _TR_MOD_CAP)	// conclusione della transazione sulla primary sull'operazione di mod cap, relativa ai file GA
		{
			if( isFile(rec_tr->local_res) )
			{	

				List_ModCap list[det_tr->length];
				int k ;

				for( k = 0 ; k < det_tr->length ; k++)
				{
					list[k].id_cap = det_tr->list[k].opt1;
					memset(list[k].path_mod_cap , '\0' , _DIM_RES);
					memset(list[k].path_sav_cap , '\0' , _DIM_RES);
					strcpy(list[k].path_sav_cap, det_tr->list[k].res3);
				}

				redo_mod_cap(list , rec_tr->length , rec_tr->local_res );
			}	
		}	
		
		free(det_tr->list);
		free(det_tr);	
	}

	if(riuscita == -1)
	{
		update_log(PATH_DB_LOG , rec_tr->id_p, rec_tr->id_c , LOG_ABORT);
	
		TrLog * det_tr;
		int rc;
		char stringa_errore[_MAX_STRING_ERROR] = {0x0};

		det_tr = select_detTR(PATH_DB_LOG, "Log", rec_tr->id_p, rec_tr->id_c, 0, "\0", 0 , "\0", stringa_errore, &rc);
	
		if(rec_tr->type == _TR_UPLOAD ) // redo replica sull'operazione di upload
		{						
			if( det_tr->list[0].opt1 == 1)
			{
				cp_rewrite_file(det_tr->list[0].res2 ,  rec_tr->local_res ,max_line );	
			}
			else
			{
				if( isFile(rec_tr->local_res) )
					remove(rec_tr->local_res);
			}
		}
	
		if(rec_tr->type == _TR_RM)// redo replica sull'operazione di remove
		{
			if( !isFile(rec_tr->local_res) )
			{				
				cp_rewrite_file(det_tr->list[0].res2 , rec_tr->local_res ,max_line );	
			}
		}
	
		if(rec_tr->type == _TR_MKDIR)// redo replica sull'operazione di create directory
		{
			if( isDir(rec_tr->local_res) )
				removeDirectory(rec_tr->local_res);	
		}

		if(rec_tr->type == _TR_RM_DIR)// redo replica sull'operazione di remove directory
		{
			if( !isDir(rec_tr->local_res) )
				createDirectory(rec_tr->local_res);	
		}
		
		if(rec_tr->type == _TR_MOD_CAP)	// redo primary sull'operazione di mod cap, relativa ai file GA
		{
			if( isFile(rec_tr->local_res) )
			{	

				List_ModCap list[det_tr->length];
				int k ;
			
				for( k = 0 ; k < det_tr->length ; k++)
				{
					list[k].id_cap = det_tr->list[k].opt1;
					memset(list[k].path_mod_cap , '\0' , _DIM_RES);
					memset(list[k].path_sav_cap , '\0' , _DIM_RES);
					strcpy(list[k].path_sav_cap, det_tr->list[k].res3);
				}										

				int fl = open(rec_tr->local_res, O_RDWR);
				for( k = 0  ;   k < det_tr->length ; ++k)// scrittura sul file originale
				{
					int fc;
					int fr;
					fc = open(list[k].path_mod_cap, O_RDWR);
					char result_file[_DIM_RES] = {0x0};

					mod_CapGA(fl, fc , result_file ,list[k].id_cap , tmp_path);

					close(fc);
					fr = open(result_file, O_RDWR);
					write_res_on_res(fr ,fl , max_line );
					close(fr);
					remove(result_file);
				}
				close(fl);
			}	
		}	
					
		free(det_tr->list);
		free(det_tr);
	}
	
	return riuscita;
}



// processo che fa partire l'algoritmo bully
void *control_bully()
{	
	printf("Creating thread Bully %lu\n", (unsigned long)pthread_self());	

	sleep(15);//sleep(ciclo_bully_static);
	
	while(1) 
	{
		int time_random;
		int t;
		time_random = generatoreRandom(ciclo_bully_random);
		int univ_time = time_random + ciclo_bully_static;
		
		if(exit_bully)
		{	
			pthread_exit(NULL);
		}
		
		int len_dom;
		Dominio * dom_list_th ;
		DB *db_dom; // descrittore del DB Domini
		char *stringa_errore ;
		char cond[_DIM_COND];
	
		db_dom = ( DB *) malloc (sizeof(DB) ) ;
		stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char) );
		 
		if( !open_db_ro( db_dom , PATH_DB_TMPDOM , stringa_errore))
		{
		  fprintf(stderr, "%s\n", stringa_errore);
		  exit(1);
		}
		
		memset( cond , '\0' , _DIM_COND );
		snprintf(cond , _DIM_COND*sizeof(char) ,"where type_server = 2 AND server = '%s'", name_server);
		
		dom_list_th = create_list_domini(db_dom , &len_dom , cond, stringa_errore );
		
		if( !close_db(db_dom, stringa_errore))
		{
		 	fprintf(stderr, "%s\n", stringa_errore);
		 	exit(1);
  		}

		free(db_dom);
		free(stringa_errore);
		
		pthread_t threads[len_dom];
		//printf("creo %d thread bully 1 \n" , len_dom);
		for(t=0;t < len_dom ;t++)
		{
			int rc ;

			rc = pthread_create (&threads[t], NULL, func_helper_bully, (void *)&dom_list_th[t]);
			
			if (rc)
			{
		   	printf("ERROR; return code from pthread_create() is %d\n",rc);
		   	exit(-1);
			}
		}
		
		if(exit_bully)
		{
			sleep(ciclo_bully_static);
			for(t=0;t < len_dom ;t++)
			{	
					pthread_cancel(threads[t] );
			}		
			free(dom_list_th);
			pthread_exit(NULL);
		}
		
		for(t=0;t < len_dom ;t++)
		{	
			pthread_join(threads[t], NULL );
		}
		free(dom_list_th);
		
		sleep(univ_time);
	}
}		
		
		
		
void *func_helper_bully(void*l)	
{		
	Dominio * dom = (Dominio *)l;
	
	Server *primary;

	primary = search_primary(PATH_DB_TMPDOM , dom->name) ;
	
	//printf( "1)\nthread dominio = %s \n" , dom->name);
	if( isAlive_pp(listServ_pp, sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , primary->ip , *port_server_pp ) <= 0 )
	{

		int i ;
		for( i = 0; i < timeout_bully; i = i+5)
		{
			sleep(5);
			
			if(exit_bully)
			{
				free(primary);
				pthread_exit(NULL);
			}
		}
		
		if( isAlive_pp(listServ_pp, sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , primary->ip , *port_server_pp) <= 0 )
		{
			Server *vista;
			int len_vista;
			vista = vistaBully(dom->name , PATH_DB_TMPDOM,	&len_vista ); 
			//printf("2)\nil dominio %s è colpevole, primary = %s , len_vista = %d \n" , dom->name ,  primary->name  , len_vista);
			start_bully(name_server , primary->name , dom->name , vista , len_vista );
			free(primary);
			free(vista);
			pthread_exit(NULL);
		}

	}

	pthread_exit(NULL);
}



// richiama la procedura di bully (no dispatcher)
// Parametri-> path della risora , lunghezza
void internal_bully(char *res , int l_res )
{
	char name_dom[_DIM_DOM+1] ={0x0};

	int q ;
	for( q  = 1 ; ( (q < l_res) && (q < _DIM_DOM+1 )) ; ++q)
	{
		if(strncmp(&(res[q]) , "/" , 1) == 0 )
		{
			break;
		}
		else
		{
			name_dom[q-1] = res[q];
		}
	}
	
	Server *primary;

	primary = search_primary(PATH_DB_TMPDOM , name_dom) ;

	int i ;
	for( i = 0; i < timeout_bully; i = i+5)
	{
		sleep(5);
		
		if(exit_bully)
		{
			free(primary);
			pthread_exit(NULL);
		}
	}
	
	if( isAlive_pp(listServ_pp, sem_listServ_pp , timeout_pp , _TENTATIVI_PP , my_ip , primary->ip , *port_server_pp ) <= 0 )
	{
		Server *vista;
		int len_vista;
		vista = vistaBully(name_dom , PATH_DB_TMPDOM,	&len_vista ); 
		start_bully(name_server , primary->name , name_dom , vista , len_vista );
		free(primary);
		free(vista);
	}
}



// richiama la procedura di risoluzione dei log in stato di pronto per un dominio che ha cambiato primary a causa di un crash
void resolv_pronto_newprimary(char *dominio)
{
	TrLog *rec_tr;
	char str_cond[_DIM_COND] = {0x0};
	char str_groupby[_DIM_COND] = {0x0};
	char stringa_errore[_MAX_STRING_ERROR] = {0x0};
	int len;
	int i ;
	Server *vista;
	int len_vista;
	vista = vistaBully(dominio , PATH_DB_TMPDOM,	&len_vista );
	
	snprintf(str_cond , _DIM_COND -1 , " where state = %d "  , LOG_PRONTO );
	strncpy(str_groupby , " group by id_p , id_c " , _DIM_COND -1 );
	
	rec_tr = select_recTR(PATH_DB_LOG, "Log" , str_cond , str_groupby , stringa_errore , &len);
	
	for( i = 0 ; i < len ; ++i)
	{
		if( id_server != rec_tr[i].id_p)
		{
			char name_dom[_DIM_DOM+1] ={0x0};
			int len_fs;
			len_fs = strlen(fs_path);
			char *buffer, *buffer2;
			buffer = rec_tr[i].local_res + len_fs + 1;
			buffer2 = strstr(buffer, "/");
			strncpy(name_dom, buffer, buffer2 - buffer);
			
			if(strcmp(name_dom , dominio) == 0 )
			{
				int rc = recovery_pronto_replica_replica( &(rec_tr[i]) , vista , len_vista );
				char risp_state[15] = {0x0};
				
				if( rc == 1) 
					strcpy(risp_state , "LOG_COMMIT");
				else
					strcpy(risp_state , "LOG_ABORT");

				int q;
				int connsd;

				for(q = 0 ; q < len_vista ; ++q)
				{
					connsd = connect_ServerServer(vista->ip , *port_server_pr , 3) ;
	
					if( connsd > 0)
					{	
						PR_Message* msg;
						msg = init_PR_Message( _PR_SET_LOG_STATE , "\0",  dominio , rec_tr[i].id_p, rec_tr[i].id_c , risp_state);

						set_timeout(connsd  , timeout_pr  , 0  , timeout_pr  , 0 );	

						send_PR_Message(connsd , msg );
									
						free(msg);
						close(connsd);
					}
				}
			}
		}
	}
	
	free(rec_tr);
	free(vista);		
}



// cerca il primary di un dominio, ritorna: la stringa formattata domini - server primary
// Parametri -> nome dominio , path del db , lunghezza struttura , eventuale stringa di errore
Server *search_primary(char *path_db ,	char *dominio )
{
	DB *db_dom; // descrittore del DB Domini
	char *stringa_errore ;
	char string_query[_DIM_QUERY_COND];
	Server *primary; 
	ResultQuery *struct_list;
	
	db_dom = ( DB *) malloc (sizeof(DB) ) ;
	stringa_errore = (char *) calloc (_MAX_STRING_ERROR , sizeof(char) );
	
	
	 
  if( !open_db_ro( db_dom , path_db , stringa_errore))
	{
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	memset( string_query , '\0' , _DIM_QUERY_COND*sizeof(char) );
	snprintf(string_query , _DIM_QUERY_COND*sizeof(char) , "select server , ip , port from dom where type_server = 1 AND dominio = '%s'", dominio);
	
	struct_list = (ResultQuery*) calloc ( 1 , sizeof(ResultQuery) );

	primary = calloc( 1 , sizeof(Server));

	struct_list->object = (void*) primary;
	struct_list->count = 1;
	struct_list->l_row = 0;

	if(!select_on_bufferServer(db_dom , string_query , struct_list , stringa_errore ) )
	{
		fprintf(stderr, "%s\n", stringa_errore);
		free(primary);
		primary = NULL;;
	}
	
	free(struct_list);

  if( !close_db(db_dom, stringa_errore)){
    fprintf(stderr, "%s\n", stringa_errore);
    exit(1);
  }

	free(db_dom);
	free(stringa_errore);

	return primary;
}
