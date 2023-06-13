#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 5
//Fehlerbehandlung
static void kill(char* prog_name){
    perror(prog_name);
    exit(EXIT_FAILURE);
}

static int some_sorting_function(const void * a, const void * b){
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;

    //return *ia - *ib;
    if (*ia < *ib){
        return -1;
    }
    else if (*ia == *ib){
        return 0;
    }
    else return 1;
}

/// @brief Prints decimal values given as integer array
/// @param arr Array containing values
/// @param arr_size Number of elements in array
static void print(int * arr, int arr_size){
    for (int i = 0; i < arr_size; i++){
        fprintf(stdout, "%d\t", arr[i]);
        if (fflush(stdout) == EOF){
            kill("fprintf");
        }
    }
}

int main(int argc, char** argv){
    //allocate array with malloc
    int* array = malloc(sizeof(int) * SIZE);
    //error by malloc
    if (array == NULL){
        kill("malloc");
    }

    //init array with random values
    for (int i = 0; i < SIZE; i++){
        array[i] = rand() % 20;
    }
    //print before sort
    print(array, SIZE);
    fprintf(stdout, "\n");
    
    //sort
    qsort(array, SIZE, sizeof(int), some_sorting_function);
    //print after sort
    print(array, SIZE);

    if (fflush(stdout) == EOF){
        kill("fflush");
    }

    free(array);    //free the allocation
    exit(EXIT_SUCCESS);
}