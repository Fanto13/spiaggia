#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#define N 234

typedef struct _data{
	int anno;
	int mese;
	int giorno;
}data;

typedef struct _listdata{
	data data_inizio;
	data data_fine;
	struct _listdata *next;
}list_data;

typedef struct {
	char numero_ombrello[3];
	short int stato;
	data data_scadenza;//primo giorno in cui l'ombrellone ?libero
}umbrella;

typedef struct _prenotazioni{
	umbrella ombrello;
	list_data *data_prenotaz;
	pthread_mutex_t semaforo;
}prenotazioni;

typedef enum{
	febbraio = 28,
	aprile = 30,
	giugno = 30,
	settembre = 30,
	novembre = 30,
	gennaio = 31,
	marzo = 31,
	maggio = 31,
	luglio = 31,
	agosto = 31,
	ottobre = 31,
	dicembre = 31
}giorni_mese;

int confronto_date(data,data);
bool controllo_intervallo(data,data,data,data);
bool controllo_data(data);
bool inserimento_lista(list_data**, list_data**);
void visualizza_date(list_data*);
void scrittura_file(prenotazioni*);
void lettura_file(FILE*,prenotazioni*);
list_data* inserimento_lista1(list_data**,list_data**);
list_data* eliminazione_lista(list_data**, data, data);
void inizialize();
data string_to_date(char*);
char* addzeros(data);
