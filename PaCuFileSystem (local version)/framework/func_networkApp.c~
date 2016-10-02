#include "framework.h"

// crea un messaggio di tipo BootAM, restituisce un messaggio di tipo BootAM
BootAM* create_boot_am()
{
	BootAM *msg;
	msg = (BootAM *) calloc( 1 ,sizeof(BootAM) );
	return msg;
}

//  Crea la connessione, restituisce: 0 se l'operazione ha avuto esito negativo, 1 se l'operazione ha avuto successo
//  Parametri -> ip e porta a cui connettersi
int connessioneClient(char *ipServer , uint16_t port_server , int n_prove)
{
	int			sockfd;
  struct sockaddr_in	servaddr;
	int i ;
	int flags;
  
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("errore in socket");
    exit(1);
  }

  memset((void *)&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port_server);
  if (inet_pton(AF_INET, ipServer, &servaddr.sin_addr) <= 0) {
    fprintf(stderr, "errore in inet_pton per %s\n", ipServer);
    exit(1);
  }   
/*
		//SETTO IL SOCKET COME NON BLOCCANTE  
  if ( (flags=fcntl(sockfd,F_GETFL,0)) <0 ) {
      printf ("fcntl(F_GETFL) failed, Err: %d \"%s\"\n", errno,strerror(errno));
      exit(1); 
  }
 
  if ( fcntl(sockfd,F_SETFL,flags | O_NONBLOCK) <0 ) {
      printf ("fcntl(F_SETFL) failed, Err: %d \"%s\"\n", errno,strerror(errno));
      exit(1);
  } */

setsockopt( sockfd , SOL_TCP , TCP_SYNCNT , &n_prove , sizeof(TCP_SYNCNT) );

connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	/*	for( i = 0 ; i < n_prove ; ++i )
	{
 		if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
	{
    	printf("errore in connect prova %d \n" ,i+1 );

			if( i != (n_prove-1) )
				sleep(1); 
  	}
		else
			i = n_prove;
	}

	if ( fcntl(sockfd,F_SETFL,flags) <0 ) {
      printf ("fcntl(F_SETFL) failed, Err: %d \"%s\"\n", errno,strerror(errno));
      exit(1);
  }*/
  
  return sockfd;
}



//	Converto un indirizzo ip in formato stringa in uno in formato u_long (unsigned long)
//	Parametri-> stringa contenete l'indirizzo ip, struttura dove inserire l'indirizzo ip
void get_ip_string( char * string_ip , struct in_addr * ip) 
{
	if (inet_pton(AF_INET, string_ip , ip) <= 0) 
	{
  	fprintf(stderr, "errore in inet_pton per %s", string_ip);
  	exit(1);
  }
}



//	Inizializzazione della variabile sockaddr_in
//	Parametri-> puntatore alla struttura sockaddr, puntatore ala porta, e valore dell'ip
void addr_init(struct sockaddr_in *addr, in_port_t *port, long int ip)  
{
        addr->sin_family=AF_INET;
        addr->sin_port = *port ;
        addr->sin_addr.s_addr=htons(ip);
}



// Effettua la connect, restituisce: il descrittore della connessione instaurata
// Parametri-> descrittore su cui fare l'accept
int connessioneServer(int listensd)
{
	int connsd;
	if((connsd=accept(listensd,(struct sockaddr*)NULL,NULL))<0)
		{
			perror("errore in accept");
			exit(-1);
		}
		puts("Connessione!");
	return connsd;
}



//	Imposta il timeout sul descrittore della connessione, restituisce: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, timeout per l'invio in secondi e microsecondi, timeout per la ricezione in secondi e microsecondi,
int set_timeout(int sockd , time_t w_sec , suseconds_t w_usec , time_t r_sec , suseconds_t r_usec)
{

	struct timeval w_tv , r_tv;
	w_tv.tv_sec = w_sec;
	w_tv.tv_usec = w_usec;

	r_tv.tv_sec = r_sec;
	r_tv.tv_usec = r_usec;

	if( setsockopt( sockd , SOL_SOCKET , SO_RCVTIMEO , &w_tv , sizeof(w_tv) ) == -1)
	{
		perror("errore in setsockopt");
		return 0;
	}	

	if( setsockopt( sockd , SOL_SOCKET , SO_RCVTIMEO , &r_tv , sizeof(r_tv) ) == -1)
	{
		perror("errore in setsockopt");
		return 0;
	}	
	
	return 1;
}



//	Invia un messaggio di tipo Init_Message, ritorna 0 se c'e' un errore 1 se l'operazione è andata a buon fine
//	Parametri-> il messaggio da inviare , descrittore connessione
int send_InitMessage( InitMessage *msg , int connsd)
{
	int rc;
	int b;

	if( (rc = send(connsd,msg,sizeof(InitMessage ),MSG_NOSIGNAL) 	)   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	
	return b;
}



//	Ricezione messaggio di Init, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al messaggio da ricevere
int  recv_InitMessage( int connsd , InitMessage * buffer)
{
	int rc;
	int b;
	memset(buffer, 0 , sizeof(InitMessage ) );

	if (   (rc = recv(connsd,buffer,sizeof(InitMessage),0) )   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella recv: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	return b;
}



//	Invio messaggio IDT, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, messaggio da inviare
int send_IDT_Message(int connsd , IDT_Message* msg )
{
	int rc;
	int b;

	if( (rc = send(connsd,msg,sizeof(IDT_Message),MSG_NOSIGNAL) 	)   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	
	return b;
}



//	Ricezione messaggio di IDT, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al messaggio da ricevere
int  recv_IDT_Message( int connsd , IDT_Message* buffer)
{
	int rc;
	int b;
	memset(buffer, 0 , sizeof(IDT_Message ) );

	if (   (rc = recv(connsd,buffer,sizeof(IDT_Message),0) )   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella recv: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	return b;
}



//	Invio messaggio di Diagnostica, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, messaggio da inviare
int send_DiagMessage(int connsd , DiagMessage* msg )
{
	int rc;
	int b;

	if( (rc = send(connsd,msg,sizeof(DiagMessage),MSG_NOSIGNAL) 	)   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send: ");
			b = 0;
		}
	} 
	else
	{
		b = 1 ;
	}
	
	return b;
}



//	Ricezione messaggio di Diagnostic, ritorna: 0 se c'e' un errore, 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al messaggio da ricevere
int  recv_DiagMessage( int connsd , DiagMessage * buffer)
{
	int rc;
	int b;
	memset(buffer, 0 , sizeof(DiagMessage) );

	if (   (rc = recv(connsd,buffer,sizeof(DiagMessage),0) )   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella recv: ");
			b = 0;
		}
	} 

	if( buffer->pay_desc == _DIAG_ACK)
	{
		b = 1 ;
	}
	else
	{
		if( buffer->pay_desc == _DIAG_ERROR)
			print_diaError(buffer->param_state);
		
		b = 1 ;
	}
	
	return b;
}



//	Invio messaggio di Transaction, ritorna: -1 se c'e' un errore, 0 se c'e' un timeout , 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, messaggio da inviare
int send_TrMessage(int connsd , TrMessage* msg )
{
	int rc;
	int b;

	if( (rc = send(connsd,msg,sizeof(TrMessage),MSG_NOSIGNAL) 	)   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella send: ");
			b = -1;
		}
	} 
	else
	{
		b = 1 ;
	}
	
	return b;
}



//	Ricezione messaggio di Transaction, ritorna: -1 se c'e' un errore, 0 se c'e' un timeout , 1 se l'operazione va a buon fine
//	Parametri-> descrittore della connessione, puntatore al messaggio da ricevere
int  recv_TrMessage( int connsd , TrMessage * buffer)
{
	int rc;
	int b;
	memset(buffer, 0 , sizeof(DiagMessage) );

	if (   (rc = recv(connsd,buffer,sizeof(TrMessage),0) )   ==  0)
	{
		b = 0;
	}

	if( rc < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			printf("timeout !!!!!!!\n");
			b = 0;
		}
		else
		{
			perror("errore nella recv: ");
			b = -1;
		}
	} 
	else
	{
		b = 1 ;
	}
	return b;
}



//  Scrive su buffer il contenuto di un file o socket , ritorna null in caso di errore, il buffer in caso di successo
//	Parametri -> descrittore socket, blocco di lettura
char* writeFile_on_buffer(int fd, size_t n)
{
	size_t l_read = 0;
	int offset = 0;
	int count = 0;
	char *buffer;
	buffer = calloc ( 1 , sizeof(char));	

	do
	{
		count += n;
		buffer = realloc( buffer , count * sizeof(char) );
		l_read = readn(fd, &buffer[offset], n);
		offset += n ;
	}while(l_read == 0 );

	if(n == -1)
		return NULL;
	else
		return buffer;
}



//  Scrive su buffer il contenuto di un file o socket (bloccante solo prima della ricezione), ritorna null in caso di errore, il buffer in caso di 	
//	successo
//	Parametri -> descrittore socket, blocco di lettura
char* writeFile_on_buffer_noblock(int fd, size_t n)
{
	size_t l_read = 0;
	int offset = 0;
	int count = 0;
	char *buffer;
	buffer = calloc ( 1 , sizeof(char));	

	do
	{
		count += n;
		buffer = realloc( buffer , count * sizeof(char) );
		l_read = readn_noblock(fd, &buffer[offset], n);
		offset += n ;
	}while(l_read == 0 );

	if(n == -1)
		return NULL;
	else
		return buffer;
}



/*Scrive su file o socket il contenuto di un buffer. Ritorna 0 in caso di errore, 1 in caso di successo*/
int writeBuffer_on_file(int fd, size_t n , char*buffer)
{
	size_t l_write = 0;
	int offset = 0;
	int count = strlen(buffer)+1;
	int length;

	do
	{
		if(count < n)
		{
			length = count;
		}
		else
		{
			length = n;
		}
		
		l_write = writen(fd, &buffer[offset], length);
		offset += l_write;

		count = count - l_write;

	}while(count > 0);
	
	if(n == -1)
		return 0;
	else
		return 1;
}



// stampa il tipo di DiagError
//  Parametri -> id dell'errore
void print_diaError(int i )
{
	if( i == _GENERIC_ERROR )
		printf("Errore generico !! \n");
	
	if( i == _BIZANTINE_ERROR)
		printf("Errore bizantino!! \n");
	 
	if( i ==  _NO_DOM_ERROR )
	 	printf("Errore il primary non gestisce il dominio!! \n");
	 
	if( i == _IGNORE_DOM_ERROR )
	 	printf("Errore il dominio è inesistente!! \n");
	 
	if( i == _NO_RESOURCE )
 	printf("Errore la risorsa cercata è inesistente!! \n");
}



//  Stampa il tipo di TrError riscontrato in una stringa
//  Parametri -> id dell'errore , stringa
void print_TrError(int i , char* errore)
{
	if( i == _ERR_TR_GE )
		strcpy(errore , "\e[1;31mErrore generico nella transazione!!\033[m \n");
	
	if( i == _ERR_TR_RECON )
		strcpy(errore , "\e[1;31mErrore: riprova la transazione\033[m \n");
			
	if( i == _ERR_TR_BIZ )
		strcpy(errore , "\e[1;31mErrore: nella formattazione dei pacchetti durante la transazione\033[m \n");
			
	if( i == _ERR_TR_MISSPACK )
		strcpy(errore , "\e[1;31mErrore: perdita di pacchetti durante la transazione\033[m \n");
			
	if( i == _ERR_TR_ABORT )
		strcpy(errore , "\e[1;31mErrore: transazione abortita\033[m \n");
			
	if( i == _ERR_TR_NODOM )
		strcpy(errore , "\e[1;31mErrore: il server non gestisce il dominio\033[m \n");
			
	if( i == _ERR_TR_IGNDOM )
		strcpy(errore , "\e[1;31mErrore: il server ignora chi gestisce il dominio\033[m \n");
			
	if( i == _ERR_TR_NORES )
		strcpy(errore , "\e[1;31mErrore: la risorsa non esiste\033[m \n");
			
	if( i == _ERR_TR_NOCAP )
		strcpy(errore , "\e[1;31mErrore: il capitolo non esiste\033[m \n");
			
	if( i == _ERR_TR_OLDRES )
		strcpy(errore , "\e[1;31mErrore: la versione della risorsa è troppo datata\033[m \n");
			
	if( i == _ERR_TR_FULLDIR )
		strcpy(errore , "\e[1;31mErrore: impossibile eliminare la risorsa, la directory non è vuota\033[m \n");
		
	if( i == _ERR_TR_BUSYRES )
		strcpy(errore , "\e[1;31mErrore: la risorsa è occupata\033[m \n");
	
	if( i == _ERR_TR_NOFORMAT_CAP )
		strcpy(errore , "\e[1;31mErrore: Il capitolo non ha una formattazione standard\033[m \n");
	
}
