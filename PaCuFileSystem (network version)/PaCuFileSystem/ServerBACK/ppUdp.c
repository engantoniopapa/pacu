#include "header.h"


//crea un messaggio di tipo PPMessage, ritorna il puntatore al messaggio
PPMessage *create_PPmessage()
{
	PPMessage *b;
	b = (PPMessage *) calloc( 1 , sizeof(PPMessage) );
	
	return b;
}



// inizializza un messaggio di tipo PPMessage, ritorna il puntatore al messaggio, il puntatore del messaggio creato oppure NULL se c'e' qualche errore
// Parametri-> descrittore di payload del messaggio, id , ip mittente , porta mittente
PPMessage *init_PPmessage(unsigned char pay_desc, int id, char *ip , int port	)
{
	if( pay_desc == _PING_MESS_UDP || pay_desc == _PONG_MESS_UDP )
	{
		PPMessage *b;
		b = create_PPmessage();

		b->pay_desc = pay_desc;
		b->id = id;
		strncpy(b->ip , ip, _DIM_IP );
		b->port = port;
		
		return b;
	}

	return NULL;	
}



// setta un messaggio di tipo PPMessage
// Parametri-> messaggio da settare , descrittore di payload del messaggio, id , ip mittente , porta mittente
void set_PPmessage(PPMessage *b , unsigned char pay_desc, int id, char *ip , int port	)
{
	if( pay_desc == _PING_MESS_UDP || pay_desc == _PONG_MESS_UDP )
	{

		b->pay_desc = pay_desc;
		b->id = id;
		memset( b->ip , '\0' , _DIM_IP );
		strncpy(b->ip , ip, _DIM_IP );
		b->port = port;
		
	}
}



// setta una struttura di tipo Unit_TH_Pong
// Parametri-> messaggio da settare , parametri della struttura
void set_Unit_TH_Pong(Unit_TH_Pong *b , int sd ,ListPong *l, Two_rooms *s, Semaphore *lock, char *ip , int port	)
{
	b->sockfd = sd;
	b->list = l;
	b->sem = s;
	b->lock = lock;
	b->ip = ip;
	b->port = port;
}



// inizializza la struttura della lista dei pong UDP ricevuti, ritorna: la lista pong UDP inizializzata
// Parametri -> grandezza dei buffer della lista  
ListPong* init_listPong( int l)
{
	ListPong* b;
	b = (ListPong*) malloc ( sizeof(ListPong)) ;
	
	b->used = 0;
	b->length = l ;
	
	int i , j ;
	for( i = 0 ; i < CHOICE_LIST ; ++i)
	{
		b->list[i] = (struct UnitPong *) malloc ( l * sizeof(struct UnitPong) );
	}
	
	for( i = 0 ; i < CHOICE_LIST ; ++i)
	{
		for( j = 0 ; j < b->length ; ++j)
		{
			b->list[i][j].id = -1 ; 
			b->list[i][j].time_stamp = 0 ; 
		}
	}
	
	return b ;
}



// dealloca la struttura della lista dei pong UDP ricevuti
// Parametri -> il puntatore alla struttura 
void remove_listPong( ListPong *b)
{
	int i ;
	for( i = 0 ; i < CHOICE_LIST ; ++i)
		free(b->list[i]);
	
	free(b); 
}



// resetta un buffer + vecchio della struttura della lista dei pong UDP ricevuti,
// ritorna: 1 se l'operazione 0 se le informazioni sono ancora recent (nota bisogna prendere il semaforo prima)
// Parametri -> il puntatore alla struttura  , semaforo per accedere alla struttura
int reset_age_bufferPong( ListPong *b )
{
	int i ;
	time_t temp = time(NULL); 
	for( i = 0 ; i < b->length ; ++i)
	{
		if( (temp - b->list[b->used][i].time_stamp)  < DIF_TIME )
			return 0; 
	}
	
	b->used = (b->used +1) % CHOICE_LIST;
	
	for( i = 0 ; i < b->length ; ++i)
	{
		b->list[b->used][i].id = -1 ;
		b->list[b->used][i].time_stamp = 0 ; 
	}
	return 1;
}



// verifica se un id è presente nella struttura della lista dei pong UDP ricevuti, 
// ritorna: 1 se l'Id è presente 0 se non è presente nella lista (nota bisogna prendere il semaforo prima)
// Parametri -> il puntatore alla struttura  , l'id da ricercare 
int isID_listPong( ListPong *b  , int id)
{
	int i , j;
	
	for( i = 0 ; i < CHOICE_LIST ; ++i)
	{
		for( j = 0 ; j < b->length ; ++j)
		{
			if( b->list[i][j].id == id) 
				return 1; 
		}
	}
	return 0;
}



// cerca un id nella struttura della lista dei pong UDP ricevuti, 
// ritorna: 1 se l'Id è presente 0 se non è presente nella lista (nota bisogna prendere il semaforo prima)
// Parametri -> il puntatore alla struttura  , l'id da ricercare , gli indici della posizione dell'id 
int searchID_listPong( ListPong *b  , int id , int *index_buf  , int *index_id)
{
	int i , j;
	
	for( i = 0 ; i < CHOICE_LIST ; ++i)
	{
		for( j = 0 ; j < b->length ; ++j)
		{
			if( b->list[i][j].id == id) 
			{
				*index_buf = i ;
				*index_id = j ;
				return 1; 
			}
		}
	}
	return 0;
}



// inserisci un id nella struttura della lista dei pong UDP ricevuti, ritorna: 1 se l'Id è stato inserito 0 se l'id era già  presente nella lista 
// Parametri -> il puntatore alla struttura, semaforo per accedere alla lista, l'id da inserire
int insert_listPong(ListPong *b  , Two_rooms *sem , int id)
{
	int r ;
	
	lock_Two_rooms(sem);
	
		if(isID_listPong( b  , id) )
		{
			r = 0;
		}
		else
		{
			int i;
			int j = 0;
			
			for(i = 0 ; (i < b->length) && (j == 0) ; ++i)
			{
				if( b->list[b->used][i].id == -1)
				{
					b->list[b->used][i].id = id;
					b->list[b->used][i].time_stamp = time(NULL); 
					j = 1;
				}
			}
			
			if(j == 0)
			{
				while(reset_age_bufferPong( b ) == 0 )
				{
					sleep(WAIT_PP);
				}
			}
			
			r = 1;
		}

	unlock_Two_rooms(sem);
	
	return r;
}



// ritira un id dalla lista dei pong UDP ricevuti, ritorna: 1 se l'Id è stato ritirato 0 se l'id non è presente nella lista 
// Parametri -> il puntatore alla struttura, semaforo per accedere alla lista, l'id da inserire
int get_listPong(ListPong *b  , Two_rooms *sem , int id)
{
	int index_buf = -1;
	int index_id = -1;
	int r;
	
	lock_Two_rooms(sem);
	
		if(  (!searchID_listPong( b  , id , &index_buf  , &index_id) ) || ( index_buf < 0 ) || ( index_id < 0 )   )
		{
			r = 0;
		}
		else
		{
			b->list[index_buf][index_id].id = -1 ;
			b->list[index_buf][index_id].time_stamp = 0 ; 
			
			r = 1;
		}
	

	unlock_Two_rooms(sem);
	
	return r;

}




// invia un messaggio UDP di tipo PPmessage, ritorna: 1 se l'operazione è eseguita correttamente, 0 se c'e' un errore
// Parametri -> messaggio da inviare , ip e porta del destinatario
int send_PPmessage(PPMessage *msg , char*ip , int port )
{
	int sockfd;
	struct sockaddr_in addr;
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) /* crea il socket udp */
	{ 
    perror("\e[1;31mErrore: Nella creazione socket \033[m \n");
    return 0;
  }
  
  memset((void *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) 
  {
    fprintf(stderr, "\e[1;31mErrore: nella conversione dell'indirizzo ip: %s\033[m \n", ip);
    return 0;
  }  
  addr.sin_port = htons(port); /* numero di porta */
  
  if ( (sendto(sockfd, msg, sizeof(PPMessage), 0, (struct sockaddr *)&addr, sizeof(addr))) < 0) 
  {
    perror("\e[1;31mErrore: Nella sendto \033[m \n");
    return 0;
  }
	
	return 1;
}



// riceve un messaggio UDP di tipo PPmessage, ritorna: 1 se l'operazione è eseguita correttamente, 0 se c'e' un errore
// Parametri -> messaggio ricevuto , porta di ricezione
int recv_PPmessage(PPMessage *msg , int port)
{
	int sockfd;
	struct sockaddr_in addr;
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) /* crea il socket udp */
	{ 
    perror("\e[1;31mErrore: Nella creazione socket \033[m \n");
    return 0;
  }
	
	memset((void *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta pacchetti su una qualunque delle sue interfacce di rete */
  addr.sin_port = htons(port); /* numero di porta */
  
  socklen_t len = sizeof(addr);
  
  if ( (recvfrom(sockfd, msg, sizeof(PPMessage), 0, (struct sockaddr *)&addr, &len)) < 0) 
  {
    perror("\e[1;31mErrore: Nella recvfrom \033[m \n");
    return 0;
  }
  
  return 1 ;
}



// restituisce l'id incrementato
int get_count_pp( )  
{
	int r ;
	
	lock_Two_rooms(sem_count_pp);
		if( id_pp < _MAX_COUNT)
		{
			++id_pp;
		}
		else
		{
			id_pp = 1;
		}
		r = id_pp;
	unlock_Two_rooms(sem_count_pp);
	
	return r ;
}



// invio ping e controllo pong, ritorna: -1 se c'e' stato qualche errore, 1 se la macchina pingata ha risposto, 0 se la macchina pingata è assente
// Paramteri-> lista pong, semaforo per accedere alla lista, timeout, tentativi, ip e porta per pingare la macchina
int isAlive_pp(ListPong *b , Two_rooms *sem , int time , int tentativi , char* loc_ip , char* rem_ip , int port )
{	
	int id = get_count_pp(); 
	int i = 0;
	
	PPMessage *msg = init_PPmessage(_PING_MESS_UDP, id , loc_ip , port	);

	while( i < tentativi)
	{
		if ( !send_PPmessage( msg , rem_ip , port ) )
		{
			return -1;
		}
		sleep(time);				
		//printf("inviato ping , id = %d \n" ,  id );
		if(get_listPong( b ,  sem , id) )
		{
			//printf("ricevuto pong , id = %d \n" ,  id );
			return 1;
		}
		++i;
	}
	return 0;
}



//dispatcher UDP per i pacchetti ping-pong provenienti dai client
void *dispCl_rcvPP_UDP( )
{
	int rc ;
	pthread_t th_pp_c[thread_ppCl]; // descrittori thread ping-pong udp per comunicazione client-server
	Unit_TH_Pong uPong_c[thread_ppCl];
	int sockfd;
	struct sockaddr_in addr;
	
	memset((void *)&addr, 0, sizeof(addr));
		
	addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta pacchetti su una qualunque delle sue interfacce di rete */
  addr.sin_port = htons(*port_client_pp); /* numero di porta del server */
  
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { /* crea il socket */
    perror("errore in socket");
    exit(1);
  }

	/*int yes = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes , sizeof(int) )  == -1)
	{
		perror("setsockopt");
		exit(1);
	}*/
	
	/* assegna l'indirizzo al socket */
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("errore in bind");
        printf("port = %d \n", *port_client_pp);
    exit(1);
  }
		
	int i ;
	for( i = 0 ; i < thread_ppCl ; ++i)
	{ 
		set_Unit_TH_Pong( &uPong_c[i] , sockfd , listCl_pp , sem_listCl_pp , sem_accept_ppCl , my_ip , *port_client_pp);
		rc = pthread_create (&th_pp_c[i], NULL, thread_rcvPP_UDP, (void *) &uPong_c[i]);

		if (rc)
		{
		 	printf("ERROR; return code from pthread_create() is %d\n",rc);
		 	exit(-1);
		}
	}
	
	sem_wait(sem_exit_disp_ppC);
	
	exit_th_pp = 1;
	sleep(time_helper_pp);
	
	for( i = 0 ; i < thread_ppCl ; ++i)
	{
		pthread_cancel( th_pp_c[i]);
	}

	close(sockfd);
	pthread_exit(NULL);
}



//dispatcher UDP per i pacchetti ping-pong provenienti dai server
void *dispSrv_rcvPP_UDP( )
{
	int rc ;
	pthread_t th_pp_s[thread_ppSrv]; // descrittori thread ping-pong udp per comunicazione server-server
	Unit_TH_Pong uPong_s[thread_ppSrv];
	int sockfd;
	struct sockaddr_in addr;
	
	memset((void *)&addr, 0, sizeof(addr));
		
	addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta pacchetti su una qualunque delle sue interfacce di rete */
  addr.sin_port = htons(*port_server_pp); /* numero di porta del server */

  
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { /* crea il socket */
    perror("errore in socket");
    exit(1);
  }

	/*int yes = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes , sizeof(int) )  == -1)
	{
		perror("setsockopt");
		exit(1);
	}*/
	
	/* assegna l'indirizzo al socket */
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("errore in bind");
    printf("port = %d \n", *port_server_pp);
    exit(1);
  }
  
	int i;
	for( i = 0 ; i < thread_ppSrv ; ++i)
	{ 		
		set_Unit_TH_Pong( &uPong_s[i] , sockfd , listServ_pp , sem_listServ_pp , sem_accept_ppSrv , my_ip , *port_server_pp);
		rc = pthread_create (&th_pp_s[i], NULL, thread_rcvPP_UDP, (void *) &uPong_s[i]); 

		if (rc)
		{
		 	printf("ERROR; return code from pthread_create() is %d\n",rc);
		 	exit(-1);
		}
	}
	
	sem_wait(sem_exit_disp_ppS);
	
	exit_th_pp = 1;
	sleep(time_helper_pp);

	for( i = 0 ; i < thread_ppSrv ; ++i)
	{
		pthread_cancel(th_pp_s[i]);
	}
	
	close(sockfd);
	pthread_exit(NULL);
}



// funzione thread per la ricezione dei ping-pong
void *thread_rcvPP_UDP(void *l )
{

	Unit_TH_Pong *temp_buff = (Unit_TH_Pong *)l;
	ListPong *strPong = temp_buff->list;
	char *my_addr = temp_buff->ip;
	int port = temp_buff->port;
	Two_rooms *sem = temp_buff->sem;
	Semaphore *lock = temp_buff->lock;
	int sockfd = temp_buff->sockfd;
	
	printf("Creating Thread UDP PING-PONG %lu , port = %d \n", (unsigned long)pthread_self() , port);		
	
  socklen_t len;
  struct sockaddr_in addr;
  PPMessage *buff_rcv;
  PPMessage *buff_snd;
  
  buff_rcv = ( PPMessage *) calloc ( 1 , sizeof(PPMessage) );
	buff_snd = ( PPMessage *) calloc ( 1 , sizeof(PPMessage) );

  memset((void *)&addr, 0, sizeof(addr));
	
	len = sizeof(addr);
  
  while (1) 
  {
  	if(exit_th_pp)
		{
			close(sockfd);
			free(buff_rcv);
			free(buff_snd);
			pthread_exit(NULL);	
		}
		
		memset(buff_rcv , 0 , sizeof( PPMessage )  );
		int rcv = -1;
		sleep(10);
		sem_wait(lock);
			rcv = recvfrom(sockfd, buff_rcv, sizeof(PPMessage), 0, (struct sockaddr *)&addr, &len) ;
		sem_post(lock);
		
		if ( rcv < 0)
		{
    	perror("\e[1;31mErrore: Nella recvfrom \033[m \n");
		}
		else
		{
			if( buff_rcv->pay_desc == _PING_MESS_UDP || buff_rcv->pay_desc == _PONG_MESS_UDP )
			{
				if( buff_rcv->pay_desc == _PING_MESS_UDP)
				{//printf("ricevuto ping , id = %d \n" ,  buff_rcv->id );
				
					set_PPmessage(buff_snd  , _PONG_MESS_UDP , buff_rcv->id , my_addr , port );
					addr.sin_family = AF_INET;
  				if (inet_pton(AF_INET, buff_rcv->ip, &addr.sin_addr) <= 0) 
  				{
		  			fprintf(stderr, "errore in inet_pton per %s\n", buff_rcv->ip);
		  			exit(1);
  				}
  				addr.sin_port = htons(buff_rcv->port); /* numero di porta del server */
					if (sendto(sockfd, buff_snd, sizeof(PPMessage), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
					{
    				perror("\e[1;31mErrore: Nella sendto \033[m \n");
		  		}
		  		//printf("inviato pong , id = %d \n" ,  buff_snd->id );
				}
				if( buff_rcv->pay_desc == _PONG_MESS_UDP)
				{ 
					if (!insert_listPong(strPong  , sem , buff_rcv->id) )
					{
					  perror("\e[1;31mErrore: Nell'elaborazione del messaggio di pong \033[m \n");
					}
				}
			}
			else
			{
				puts("\e[1;31mErrore: Messaggio UDP non formattato  \033[m \n");
			}
			
		}
  }
  
  free(buff_rcv);
	free(buff_snd);
  pthread_exit(NULL);
}
