#Makefile del programma PaCu File System

VERSION = 0.1

HOME = /home/genesis/Scrivania/PaCuFileSystem/

pacufs : PaCu_FS.o general_functions.o $(HOME)framework/func_networkApp.o $(HOME)framework/LayerDB/api_db.o $(HOME)framework/LayerDB/Sqlite/sqlite3.o $(HOME)framework/sincronization.o func_networkApp_client.o menu.o boot_fase.o am_fase.o init_fase.o $(HOME)framework/utility.o $(HOME)framework/ga.o $(HOME)framework/python/myer.o $(HOME)framework/list.o $(HOME)framework/lockfiles.o console.o transaction.o ppUdp.o
	gcc -Wall -o pacufs PaCu_FS.o general_functions.o $(HOME)framework/func_networkApp.o $(HOME)framework/LayerDB/api_db.o $(HOME)framework/LayerDB/Sqlite/sqlite3.o $(HOME)framework/sincronization.o func_networkApp_client.o menu.o boot_fase.o am_fase.o $(HOME)framework/utility.o init_fase.o $(HOME)framework/ga.o $(HOME)framework/python/myer.o $(HOME)framework/list.o $(HOME)framework/lockfiles.o console.o transaction.o ppUdp.o  -pthread -ldl -lpython2.6 -D_THREAD_SAFE -D_REENTRANT 


sqlite3.o  : $(HOME)framework/LayerDB/Sqlite/sqlite3.c $(HOME)framework/LayerDB/Sqlite/sqlite3.h
	gcc -c -Wall $(HOME)framework/LayerDB/Sqlite/sqlite3.c

api_db.o : $(HOME)framework/LayerDB/api_db.c $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c $(HOME)framework/LayerDB/api_db.c
	
sincronization.o : $(HOME)framework/sincronization.c $(HOME)framework/sincronization.h header.h $(HOME)framework/LayerDB/api_db.h
	gcc -c -Wall $(HOME)framework/sincronization.c

general_function.o : general_function.c general_function.h header.h $(HOME)framework/networkApp.h
	gcc -c -Wall general_function.c

func_networkApp_client.o : func_networkApp_client.c func_networkApp_client.h header.h $(HOME)framework/networkApp.h
	gcc -c -Wall func_networkApp_client.c
	
func_networkApp.o : $(HOME)framework/func_networkApp.c $(HOME)framework/func_networkApp.h
		gcc -c -Wall $(HOME)framework/func_networkApp.c

menu.o : menu.c menu.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c -Wall menu.c	
	
console.o : console.c console.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c -Wall console.c	

boot_fase.o : boot_fase.c boot_fase.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c -Wall boot_fase.c	

am_fase.o : am_fase.c am_fase.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c -Wall am_fase.c

init_fase.o : init_fase.c init_fase.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c -Wall init_fase.c

utility.o : $(HOME)framework/utility.c $(HOME)framework/utility.h 
		gcc -c -Wall $(HOME)framework/utility.c
		
ga.o : $(HOME)framework/ga.c $(HOME)framework/ga.h
		gcc -c -Wall $(HOME)framework/ga.c

ppUdp.o : ppUdp.c ppUdp.h
		gcc -c -Wall ppUdp.c
		
lockfiles.o : $(HOME)framework/lockfiles.c $(HOME)framework/lockfiles.h myer.o list.o lockfiles.o
		gcc -c -Wall $(HOME)framework/lockfiles.c
		
list.o : $(HOME)framework/list.c $(HOME)framework/list.h
		gcc -c -Wall $(HOME)framework/list.c		
		
myer.o : $(HOME)framework/python/myer.c $(HOME)framework/python/myer.h
		gcc -c -Wall $(HOME)framework/python/myer.c		
		
transaction.o : transaction.c transaction.h header.h  $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h header.h
		gcc -c -Wall transaction.c

PaCu_FS.o :	PaCu_FS.c header.h  $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h header.h
	gcc -c -Wall PaCu_FS.c


clean: 
	rm -f *.o core

cleanall: 
	rm -f *.o core pacufs

