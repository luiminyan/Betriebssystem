#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

static void die(char* func_name){
    perror(func_name);
    exit(EXIT_FAILURE);
}

static void* thread_func(void* args){   
    //do something in the thread
    printf("Thread running!\n");

    //create return value ptr of thread
    int* retval = malloc(sizeof(int*));
    if (!retval) {      //or: retval == NULL
        die("malloc");
    }
    * retval = 42;

    //thread exit and set with the DIY retval
    pthread_exit(retval);
}

int main(int argc, char** argv){
    //create thread with tid
    pthread_t tid;
    int* retval;    //to store the return value of the thread

    errno = pthread_create(&tid , NULL, thread_func, NULL);     //pthread_create(&tid , NULL, thread_func, void* arg)
    //dealing with error by ptrhread_create
    if (errno) {
        die("pthread_create");
    }
    //if pthread_create runs successfully, the thread will be created and the thread_func is executed

    //thread_join, wait till the thread exits, free the resource and get the thread return value
    errno = pthread_join(tid, (void**)&retval);
    if (errno) {
        die("pthread_join");
    }
    
    if (printf("Thread exits with code %d\n", *retval) < 0) {
        die("printf");
    }

    //free the allocation by malloc (the retval of the thread)
    free(retval);

    exit(EXIT_SUCCESS);
}
