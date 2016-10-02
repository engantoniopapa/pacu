#include "framework.h"



/*Ricerca un elemento in base all'identificativo. Restituisce il puntatore all'elemento cercato o NULL.
init = id dell'elemento
map = header della lista*/
struct list_el *searchItemFromID(int init, struct list_el *map)
{
	//printf("valore header = %d\n", map->init);
	do
	{
		if(map->init == init)
			return map;
		map = map->next; 
	}while(map);
	
	return NULL;
}

/*Ricerca un elemento in base alla posizione. Restituisce il puntatore all'elemento cercato o NULL.
init = id dell'elemento
map = header della lista*/
struct list_el *searchItemFromPos(int pos, struct list_el *map)
{
	int i = 0;
	while(map && i < pos)
	{
		++i;
		map = map->next; 
	}
	if(pos == i)
	{
		return map;
	}
	return NULL;
}

/*Aggiunge un elemento dopo la posizione specificata. Restituisce il puntatore all'header della lista.
pos = posizione dopo la quale aggiungere il nuovo elemento
init = id del nuovo elemento
map = header della lista*/
struct list_el *addItemAfterPos(int pos, int init, struct list_el *map)
{
	struct list_el *temp, *header;
	header = map;
	//printf("valore header = %d\n", header->init);
	int i = 0;
	
	if(map->init == -1)
	{
		map->init = init;
		//printf("Inizializzo l'header con %d\n", init); 
		return map;
	}
	
	if(pos == 0)
	{
		//printf("valore lista attuale : %d, valore init: %d\n", map->init, init);
		temp = (struct list_el *)malloc(1*sizeof(struct list_el));
		temp->init = init;
		temp->next = map;
		pthread_cond_init(&temp->cond_write, NULL);
		return temp;
	}
	
	while(map->next && i < pos)
	{
		++i;
		map = map->next; 
	}
	//printf("valore lista attuale : %d, valore init: %d\n", map->init, init);
	temp = (struct list_el *)malloc(1*sizeof(struct list_el));
	temp->init = init;
	temp->locked = 0;
	temp->next = map->next;
	pthread_cond_init(&temp->cond_write, NULL);
	map->next = temp;
	return header;
}

/*Elimina un elemento dalla lista. Restituisce il puntatore alla testa della lista
init = id dell'elemeto da eliminare
header = testa della lista*/
struct list_el *removeItem(int init, struct list_el *header)
{
	struct list_el *map, *before;
	before = NULL;
	map = header;
	while(map->init != init)
	{
		before = map;
		map = map->next;
	}
	if(before)
	{
		before->next = map->next;
	}
	else
		header = map->next;
	pthread_cond_destroy(&map->cond_write);
	free(map);
	return header;
}

/*Inizializza la lista. Restituisce il puntatore all'header.*/
struct list_el *initHeader()
{
	struct list_el *map;
	map = (struct list_el *)malloc(1*sizeof(struct list_el));
	map->init = -1;
	map->ticketGenerator = 0;
	map->turn = 0;
	map->locked = 0;
	map->next = NULL;
	pthread_cond_init(&map->cond_write, NULL);
	return map;
}

/*Dealloca tutta la lista. Restituisce la lista vuota.
map = header della lista*/
struct list_el *freeList(struct list_el *map)
{
	struct list_el *support;
	while(map->next)
	{
		support = map->next;
		free(map);
		map = support;
	}
	map->init = -1;
	map->ticketGenerator = 0;
	map->turn = 0;
	map->next = NULL;
	return map;
}
