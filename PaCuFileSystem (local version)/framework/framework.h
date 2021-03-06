#include <time.h>
#include <stdlib.h>      /* C standard library */
#include <stdio.h>	 /* standard I/O library */
#include <string.h>      /* string functions */
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h> 
#include <ctype.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>
#include <sys/wait.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>   


#define L_TEMP 8 
#define LOCK_SUCCESS 1
#define LOCK_ERROR 0


#include "md5/md5.h"
#include "utility.h"
#include "networkApp.h"
#include "LayerDB/api_db.h"
#include "LayerDB/log_db.h"
#include "sincronization.h"
#include "ga.h"
#include "lockfiles.h"
#include "list.h"
#include "python/myer.h"
