#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int compare(const void* a, const void *b){
    //type casting
    const int* ia = (const int*) a;
    const int* ib = (const int*) b;
    //compare
    return *ia - *ib;
}

static void die(char* prog_name){
    perror(prog_name);
    exit(EXIT_FAILURE);
}

static void print_array(int* array, int length){
    for (int i = 0; i < length; i++){
        if (printf("%d\t", array[i]) < 0){
            die("printf");
        }
    }
    printf("\n");

}


int main (int argc, char** argv){
    size_t arrlen = 10;
    int random_range = 100;
    //generate array with random number
    int* array = malloc(sizeof(int) *  arrlen);
    if (array == NULL){
        die("malloc");
    }

    for (int i = 0; i < arrlen; i++){
        array[i] = rand() % random_range;
    }

    //init. array printing
    print_array(array, arrlen);

    //sort
    qsort(array, arrlen, sizeof(int), compare);

    //print after sort
    printf("Array after sorting:\n");
    print_array(array, arrlen);



    //free allocation of array
    free(array);

    if (fflush(stdout) == EOF){
        die("fflush");
    }
    exit(EXIT_SUCCESS);
}
