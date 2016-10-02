#Makefile del programma PaCu File System

VERSION = 0.1

HOME = /hosthome/Documenti/netkit/PaCuFileSystem/

pacufs : PaCu_FS.o  $(HOME)framework/func_networkApp.o $(HOME)framework/sincronization.o dispatcher_servboot.o general_functions.o $(HOME)framework/LayerDB/api_db.o $(HOME)framework/LayerDB/Sqlite/sqlite3.o func_networkAppBoot.o $(HOME)framework/utility.o $(HOME)framework/md5/md5.o
	gcc -Wall -o pacufs PaCu_FS.o $(HOME)framework/func_networkApp.o $(HOME)framework/sincronization.o dispatcher_servboot.o general_functions.o $(HOME)framework/LayerDB/api_db.o $(HOME)framework/LayerDB/Sqlite/sqlite3.o $(HOME)framework/utility.o func_networkAppBoot.o $(HOME)framework/md5/md5.o -pthread -lpython2.5 -ldl -D_THREAD_SAFE -D_REENTRANT 


sqlite3.o  : $(HOME)framework/LayerDB/Sqlite/sqlite3.c $(HOME)framework/LayerDB/Sqlite/sqlite3.h
	gcc -c -Wall $(HOME)framework/LayerDB/Sqlite/sqlite3.c

md5.o : $(HOME)framework/md5/md5.c $(HOME)framework/md5/md5.h header.h
	gcc -c -Wall $(HOME)framework/md5/md5.c

api_db.o : $(HOME)framework/LayerDB/api_db.c $(HOME)framework/LayerDB/api_db.h header.h 
	gcc -c -Wall $(HOME)framework/LayerDB/api_db.c

sincronization.o : $(HOME)framework/sincronization.c $(HOME)framework/sincronization.h header.h 
	gcc -c -Wall $(HOME)framework/sincronization.c

general_function.o : general_function.c general_function.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h 
	gcc -c -Wall general_function.c

dispatcher_serverboot.o : dispatcher_serverboot.c dispatcher_serverboot.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h 
	gcc -c -Wall dispatcher_serverboot.c

func_networkAppBoot.o : func_networkAppBoot.c func_networkAppBoot.h header.h $(HOME)framework/networkApp.h $(HOME)framework/LayerDB/api_db.h
		gcc -c -Wall func_networkAppBoot.c

utility.o : $(HOME)framework/utility.c $(HOME)framework/utility.h 
		gcc -c -Wall $(HOME)framework/utility.c
		
func_networkApp.o : $(HOME)framework/func_networkApp.c $(HOME)framework/func_networkApp.h
		gcc -c -Wall $(HOME)framework/func_networkApp.c

PaCu_FS.o :	PaCu_FS.c header.h $(HOME)framework/LayerDB/api_db.h $(HOME)framework/networkApp.h 
	gcc -c -Wall PaCu_FS.c


clean: 
	rm -f *.o  pacufs


