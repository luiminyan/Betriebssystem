#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "sem.h"

/**
 * Funktionen aus den Folien:
 * - int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *args);
 * - pthread_t pthread_self(void);
 * - void pthread_exit(void *retval);
 * - int pthread_join(pthread_t thread, void **retvalp);
 * - int pthread_detach(pthread_t thread);
 *
 * - SEM *semcreate(int initVal);
 * - void P(SEM *sem);
 * - void V(SEM *sem);
 * - void semDestroy(SEM *sem);
 */

// global variables, defines, etc.
static double a[100][100], sum;	// Eingabe und Ausgabe Variable

static SEM *sem_mutex; // gegenseitiger Ausschluss, Sem um kritische Abschnitte zu schützen

struct param{	// Struktur für Thread Parameter
    int index;
};

// errno gesetzt
static void die(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

static void err(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

static void *sumRow(void *arg) {
	struct param *par = arg;	// void Pointer zu Parameter Struktur Pointer parsen
	double localSum = 0;	// Variable für Summe einer Zeile
	
	// Summe einer Zeile berechnen
	for(int j = 0; j < 100; j++)
		localSum += a[par->index][j];
	
	P(sem_mutex);	// kritischen Bereich blockieren
	sum += localSum;	// Summe zu Gesamtsumme addieren
	V(sem_mutex);	// kritischen Bereich freigeben
	return NULL;
}

int main(int argc, char *argv[]) {
	// Semaphore initialisieren
	sem_mutex = semCreate(1); 	//nur 1 Thread erlaubt
    if(sem_mutex == NULL)
        err("semCreate() failed");
	
	pthread_t tids[100];	// Array für Speicherung von Thread IDs
	struct param args[100];	// Array für Speicherung der Thread Parameter Struktur
	
	// 100 Threads mit Parametern erzeugen
	for(int i = 0; i < 100; i++) {
		args[i].index = i;	//Parameter setzen
		errno = pthread_create(&tids[i], NULL, sumRow, &args[i]);	//Thread erzeugen
		if(errno != 0)
            die("pthread_create");
	}
	
	// alle Threads einsammeln
	for(int i = 0; i < 100; i++){
		if(pthread_join(tids[i], NULL) == -1)
			die("pthread_join");
	}
	
    semDestroy(sem_mutex);	// Semaphore freigeben
}

