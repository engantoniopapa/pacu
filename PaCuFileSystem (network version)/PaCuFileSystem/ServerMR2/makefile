#Makefile del programma PaCu File System

VERSION = 0.1

HOME = /hosthome/Documenti/netkit/PaCuFileSystem/

pacufs : PaCu_FS.o  $(HOME)framework/func_networkApp.o $(HOME)framework/sincronization.o dispatcher.o general_functions.o $(HOME)framework/ga.o $(HOME)framework/python/myer.o $(HOME)framework/list.o $(HOME)framework/lockfiles.o $(HOME)framework/LayerDB/api_db.o $(HOME)framework/LayerDB/log_db.o $(HOME)framework/LayerDB/Sqlite/sqlite3.o func_networkApp_server.o init_fase.o  $(HOME)framework/utility.o transaction.o ppUdp.o priRep_tr.o PoolComunication_Server.o recovery.o bullyAlgorithm.o
	gcc -Wall -o pacufs PaCu_FS.o  $(HOME)framework/func_networkApp.o $(HOME)framework/sincronization.o dispatcher.o general_functions.o $(HOME)framework/LayerDB/api_db.o $(HOME)framework/LayerDB/log_db.o $(HOME)framework/LayerDB/Sqlite/sqlite3.o func_networkApp_server.o $(HOME)framework/utility.o $(HOME)framework/ga.o $(HOME)framework/python/myer.o $(HOME)framework/list.o $(HOME)framework/lockfiles.o init_fase.o transaction.o ppUdp.o priRep_tr.o PoolComunication_Server.o recovery.o bullyAlgorithm.o -pthread -ldl -lpython2.5 -D_THREAD_SAFE -D_REENTRANT 


sqlite3.o  : $(HOME)framework/LayerDB/Sqlite/sqlite3.c $(HOME)framework/LayerDB/Sqlite/sqlite3.h
	gcc -c -Wall $(HOME)framework/LayerDB/Sqlite/sqlite3.c

general_function.o : general_function.c general_function.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h 
	gcc -c -Wall general_function.c

api_db.o : $(HOME)framework/LayerDB/api_db.c $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c -Wall $(HOME)framework/LayerDB/api_db.c
	
log_db.o : $(HOME)framework/LayerDB/log_db.c $(HOME)framework/LayerDB/log_db.h header.h $(HOME)framework/LayerDB/api_db.h
	gcc -c -Wall $(HOME)framework/LayerDB/log_db.c

sincronization.o : $(HOME)framework/sincronization.c $(HOME)framework/sincronization.h header.h $(HOME)framework/LayerDB/api_db.h
	gcc -c -Wall $(HOME)framework/sincronization.c

dispatcher.o : dispatcher.c dispatcher.h header.h $(HOME)framework/LayerDB/api_db.h $(HOME)framework/networkApp.h
	gcc -c -Wall dispatcher.c

init_fase.o : init_fase.c init_fase.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h $(HOME)framework/networkApp.h
	gcc -c -Wall init_fase.c

func_networkApp_server.o : func_networkApp_server.c func_networkApp_server.h header.h  $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h
		gcc -c -Wall func_networkApp_server.c

func_networkApp.o : $(HOME)framework/func_networkApp.c $(HOME)framework/func_networkApp.h
		gcc -c -Wall $(HOME)framework/func_networkApp.c

utility.o : $(HOME)framework/utility.c $(HOME)framework/utility.h 
		gcc -c -Wall $(HOME)framework/utility.c

ga.o : $(HOME)framework/ga.c $(HOME)framework/ga.h
		gcc -c -Wall $(HOME)framework/ga.c
		
lockfiles.o : $(HOME)framework/lockfiles.c $(HOME)framework/lockfiles.h 
		gcc -c -Wall $(HOME)framework/lockfiles.c
		
list.o : $(HOME)framework/list.c $(HOME)framework/list.h
		gcc -c -Wall $(HOME)framework/list.c		
		
myer.o : $(HOME)framework/python/myer.c $(HOME)framework/python/myer.h
		gcc -c -Wall $(HOME)framework/python/myer.c		
		
transaction.o : transaction.c transaction.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h 
		gcc -c -Wall transaction.c
		
PoolComunication_Server.o : PoolComunication_Server.c PoolComunication_Server.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h 
		gcc -c -Wall PoolComunication_Server.c
	
ppUdp.o : ppUdp.c ppUdp.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h
		gcc -c -Wall ppUdp.c
		
recovery.o : recovery.c recovery.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h $(HOME)framework/LayerDB/log_db.h
		gcc -c -Wall recovery.c
		
priRep_tr.o : priRep_tr.c priRep_tr.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h 
		gcc -c -Wall priRep_tr.c
		
bullyAlgorithm.o : bullyAlgorithm.c bullyAlgorithm.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h 
		gcc -c -Wall bullyAlgorithm.c

PaCu_FS.o :	PaCu_FS.c header.h $(HOME)framework/LayerDB/api_db.h $(HOME)framework/networkApp.h
	gcc -c -Wall PaCu_FS.c


clean: 
	rm -f *.o pacufs

