#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     //malloc, calloc, free...
#include <pthread.h>    //thread
#include <errno.h>      //errno
#include "sem.h"        //for thread


static void die (char* func_name) {
    perror(func_name);
    exit(EXIT_FAILURE);
}

static void err(char* func_name){
    fprintf(stderr, "%s\n", func_name);
    exit(EXIT_FAILURE);
}

//!!! don't forget the ";" !!!
// not static
struct param {
    int index;
};

static double a[100][100], sum;

//declare Semaphor
static SEM* sem_mutex;

static void* thread_func_sum (void* arg) {
    struct param *par = arg;	// void Pointer zu Parameter Struktur Pointer parsen
    double temp_sum = 0;
    
    for (int j = 0; j < 100; j++) {
        temp_sum += a[par->index][j];
    }

    //critical area
    P(sem_mutex);   //counter--
    sum += temp_sum;
    V(sem_mutex);   //couter++
    return NULL;
}

int main (int argc, char** argv) {
    //list of thread ids
    pthread_t tid_list[100];    //list of pthread_t
    struct param args[100];     //list of index for the iteration (type struct param)

    //initialize the semaphore
    sem_mutex = semCreate(1);     //only 1 thread allow at the same time
    if (sem_mutex == NULL) {
        err("semCreate");
    }

    //create 100 threads
    for (int i = 0; i < 100; i++) {
        args[i].index = i;
        // sizeof((void *)) != sizeof(int), cannot simply casting
        errno = pthread_create(&tid_list[i], NULL, thread_func_sum, /* argument */ &args[i]);
        if (errno) {
            die("pthread_create");
        }
        //if success: di the thread_func_sum
    }

    //after the threads died, collect them with return values
    for (int i = 0; i < 100; i++) {
        errno = pthread_join(tid_list[i], NULL); 
        if (errno) {
            die("pthread_join");
        }
    }
    semDestroy(sem_mutex);
    //exit(EXIT_SUCCESS);
}
