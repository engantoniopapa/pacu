#Makefile del Tool gestione accesso

VERSION = 0.1

HOME = /home/genesis/Scrivania/PaCuFileSystem/

tool_accesso : main.o  $(HOME)framework/md5/md5.o  $(HOME)framework/LayerDB/Sqlite/sqlite3.o $(HOME)framework/LayerDB/api_db.o
	gcc -Wall -o tool_accesso  main.o $(HOME)framework/LayerDB/Sqlite/sqlite3.o $(HOME)framework/LayerDB/api_db.o $(HOME)framework/md5/md5.o -pthread -ldl -D_THREAD_SAFE -D_REENTRANT 


sqlite3.o  : $(HOME)/framework/LayerDB/Sqlite/sqlite3.c $(HOME)/framework/LayerDB/Sqlite/sqlite3.h
	gcc -c -Wall $(HOME)/framework/LayerDB/Sqlite/sqlite3.c
	
api_db.o : $(HOME)framework/LayerDB/api_db.c $(HOME)framework/LayerDB/api_db.h header.h 
	gcc -c -Wall $(HOME)framework/LayerDB/api_db.c

md5.o : $(HOME)framework/md5/md5.c $(HOME)md5/md5.h 
	gcc -c -Wall $(HOME)md5/md5.c

main.o :	main.c 
	gcc -c -Wall main.c




