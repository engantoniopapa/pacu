#ifndef SINCRONIZATION_H
#define SINCRONIZATION_H
#include <semaphore.h>


typedef sem_t Semaphore;

Semaphore *make_semaphore (int );
void free_semaphore(Semaphore *);


struct Lightswitch
{
	int counter ;
	Semaphore *sem ;

};
typedef struct Lightswitch  Lightswitch;

Lightswitch *make_Lightswitch();
void lockLightswitch( Lightswitch * , Semaphore *);
void unlockLightswitch( Lightswitch * , Semaphore *);
void free_Lightswitch(Lightswitch *);



struct Writers_Reader
{
	Semaphore *mutex ;
	Semaphore *noReaders ;
	Semaphore *noWriters ;
	Lightswitch *readSwitch ;
	Lightswitch *writeSwitch ;
};
typedef struct Writers_Reader  Writers_Reader;

Writers_Reader *make_Writers_Reader();
void free_Writers_Reader(Writers_Reader *);
void lock_WR_Writers(Writers_Reader *);
void unlock_WR_Writers(Writers_Reader *);
void lock_WR_Reader(Writers_Reader *);
void unlock_WR_Reader(Writers_Reader *);

struct Two_rooms
{
	int room1;
	int room2 ;
	Semaphore *mutex;
	Semaphore *t1 ;
	Semaphore *t2 ;
};
typedef struct Two_rooms Two_rooms;

Two_rooms *make_Two_rooms();
void free_Two_rooms(Two_rooms *);
void lock_Two_rooms(Two_rooms *);
void unlock_Two_rooms(Two_rooms *);


#endif
