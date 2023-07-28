#include <errno.h> 
#include <pthread.h>
#include <unistd.h> 
#include <stdbool.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <inttypes.h>

#include "sem.h" 
#include "triangle.h" 
#include "queue.h"

/* Funktionen aus sem.h */
SEM *semCreate(int initVal); // sets errno on failure void semDestroy(SEM *sem);
void P(SEM *sem);
void V(SEM *sem);

// Funktionen triangle.h (parseTriangle, countPoints): SIEHE MANPAGE 
// Funktionen queue.h (qCreate, qPut, qGet, qDestroy): SIEHE MANPAGE
static const size_t MAX_LINE = 1024; 
static const size_t CALC_THREADS = 5;

static void die(const char message[]) { 
    perror(message); 
    exit(EXIT_FAILURE);
}

static void usage(void) {
fprintf(stderr, "Usage: ./parrots <file>\n"); exit(EXIT_FAILURE);
}

// Funktions- & Strukturdekl., globale Variablen, etc.
SEM* list_mutex;


// Hauptfunktion (main)
int main(int argc, char* argv[]) {
    //Befehlzeilenargumente prüfen
    if(argc != 2) { 
        usage(); 
    }

    // Initialisierung

    // Threads starten
    // "Haupt"schleife
    // Threads + Ressourcen aufräumen
    
}
// Ende Hauptfunktion

// Funktion Rechenthread
// Ende Rechenthread

// Ausgabethread
// Ende Ausgabethread
