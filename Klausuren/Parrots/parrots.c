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
// declaire semaphores
SEM* newDataSignal = NULL;     //for updating the Ausgabethread / information printing
SEM* statMutex = NULL;  //protecting the global variables (to be used in printing)

// declaire queue
static struct QUEUE* tri_queue;

// declaire functions
static void* print_func();
static void* calculate_func();

// declaire boundary, interior coordinates
static int boundary_count;
static int interior_count;

// declaire all threads finished
static bool is_finished = false;


// Hauptfunktion (main)
int main(int argc, char* argv[]) {
    //Befehlzeilenargumente prüfen
    if(argc != 2) { 
        usage(); 
    }

    // Initialisierung
    // create queue
    tri_queue = qCreate();
    // error by qCreate defined in "queue.h"
    if (tri_queue == NULL) {
        die("qCreate");
    }

    // create the semaphores
    newDataSignal = semCreate(0);       //print the info once updated
    statMutex = semCreate(1);    //only one thread is allowed to update the global info

    // create (calc and print) threads array
    pthread_t threads_array[CALC_THREADS + 1];  
   
    // Threads starten
    // create Ausgabethread
    errno = 0;
    errno = pthread_create(&threads_array[CALC_THREADS + 1], NULL, print_func, NULL);
    if (errno != 0) {
        die("pthread_create");
    }
    // cretae CALC_threads
    for (int i = 0; i < (int) CALC_THREADS; i++) {
        errno = 0;
        errno = pthread_create(&threads_array[i], NULL, calculate_func, NULL);
        if (errno != 0) {
            die("pthread_create");
        }
    }

    // Arbietspakete aus der Datei auslesen
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        die("fopen");
    }

    char line[MAX_LINE + 2];
    // "Haupt"schleife
    while (fgets(line, MAX_LINE + 2, file) != NULL) {
        // check the line format
        // line too short
        if (strlen(line) < 2) {
            usage();
            continue;
        }
        // line too long
        if ((strlen(line) == MAX_LINE + 1) && (line[MAX_LINE] != '\n')) {
            fprintf(stderr, "Line too long!\n");
            int c;
            do {
                c = getchar();
            } while ((c != '\n') && (c != EOF));
            continue;
        }

        // create triangle
        struct triangle* tri = (struct triangle*) malloc(sizeof(struct triangle*));
        if (tri == NULL) {
            die("malloc");
        }

        // parse the line into triangle
        if (parseTriangle(line, tri) == false) {
            // free allocation is fails
            free(tri);
            fprintf(stderr, "Failed to parse the triangle!\n");
            // parse the next line
            continue;
        }

        // put triangle in the queue when successfully parsed
        qPut(tri_queue, tri);

        

    }
    if (ferror(file)) {
        die("fgets");
    }

    // Threads + Ressourcen aufräumen
    // join all caulculate processes
    for (int j = 0; j < CALC_THREADS; j++) {
        errno = 0;
        errno = pthread_join(threads_array[j], NULL);
        if (errno != 0) {
            die("pthread_join");
        } 
    }

    is_finished = true;

    //unfreeze Ausgavethread to print the info one last time
    V(newDataSignal);

    // kill the queue
    qDestroy(tri_queue);

    // kill the semaphores
    semDestroy(newDataSignal);
    semDestroy(statMutex);
    
}
// Ende Hauptfunktion

// Funktion Rechenthread
void* calculate_func() {
    while (1) {
        int boundry = 0;
        int interior = 0;

        // create struct triangle iterator
        struct triangle* tri = qGet(tri_queue);
        if (tri == NULL) {
            break;
        }

        //do the calculation
        countPoints(tri, &boundry, &interior);

        //update the global information
        P(statMutex);
        boundary_count += boundry;
        interior_count += interior;
        V(statMutex);

        //inform print thread
        V(newDataSignal);

        //free the allocation in queue
        free(tri);
    }
}
// Ende Rechenthread

// Ausgabethread
// !!!ATTENTION: void* for thread functions 
void* print_func() {
    do {
        //wait for data update
        P(newDataSignal);

        // read the stat information
        P(statMutex);
        int p_boundary = boundary_count;
        int p_interior = interior_count;
        V(statMutex);

        //print
        if (printf("Total boundary: %d, total interior: %d\n", p_boundary, p_interior) < 0) {
            die("printf");
        }

        if (fflush(stdout) == EOF) {
            die("fflush");
        }
    } while (is_finished != true);
}
// Ende Ausgabethread
