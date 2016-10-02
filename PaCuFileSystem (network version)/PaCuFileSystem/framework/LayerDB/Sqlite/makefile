#Makefile sqlite3




sqlite3 : sqlite3.o shell.o
	gcc -m32 -Wall -o sqlite3 sqlite3.o shell.o -pthread -ldl -DTHREADSAFE=1
	

sqlite3.o  : sqlite3.c sqlite3.h
	gcc -m32 -c -Wall sqlite3.c
	
shell.o  : shell.c sqlite3.h
	gcc -m32 -c -Wall shell.c

