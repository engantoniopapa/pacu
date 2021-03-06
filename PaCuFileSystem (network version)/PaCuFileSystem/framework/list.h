

/*Elemento di una lista che rappresenta le caratteristiche di un capitolo di un file.
init = id del capitolo
cap_mutex = semaforo che gestisce la mutua esclusione sul capitolo
next = elemento successivo della lista*/
struct list_el {
   int init;
   Two_rooms *cap_mutex;
   struct list_el *next;
};

/*Ricerca un elemento in base all'identificativo. Restituisce il puntatore all'elemento cercato o NULL.*/
struct list_el *searchItemFromID(int init, struct list_el *map);

/*Ricerca un elemento in base alla posizione. Restituisce il puntatore all'elemento cercato o NULL.*/
struct list_el *searchItemFromPos(int pos, struct list_el *map);

/*Aggiunge un elemento dopo la posizione specificata. Restituisce il puntatore all'header della lista.*/
struct list_el *addItemAfterPos(int pos, int init, struct list_el *map);

/*Elimina un elemento dalla lista. Restituisce il puntatore alla testa della lista*/
struct list_el *removeItem(int init, struct list_el *header);

/*Inizializza la lista. Restituisce il puntatore all'header.*/
struct list_el *initHeader();

/*Dealloca tutta la lista. Restituisce la lista vuota.*/
struct list_el *freeList(struct list_el *map);
