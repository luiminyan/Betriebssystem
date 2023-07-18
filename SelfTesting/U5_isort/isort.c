#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//die by errno
static void die(char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

static int compare(const void* a, const void* b) {
    //type casting
    const int* ia = (const int*) a;
    const int* ib = (const int*) b;
    return *ia - *ib;
}

static void print_array(int* arr, int arr_len) {
    //print the list
    for (int i = 0; i < arr_len; i++) {
        if (fprintf(stdout, "%d\t", arr[i]) < 0){
            die("fprintf");
        }
    }

    //print "\n"
    if (printf("\n") < 0) {
            die("fprintf");
    }
}


int main(int argc, char* argv[]) {
    //allocate array
    int arr_size = 15;
    int* int_arr = malloc(sizeof(int*) * arr_size);
    if (int_arr == NULL) {
        die("malloc");
    }

    //give random value
    for (int j = 0; j < arr_size; j++) {
        int_arr[j] = rand() % 20;
    }

    //print array
    print_array(int_arr, arr_size);

    //qsort(array, array_size, element_size, compare_func)
    qsort(int_arr, arr_size, sizeof(int), compare);

    //print array
    print_array(int_arr, arr_size);

    free(int_arr);

    //flush the stdout
    if (fflush(stdout) == EOF) {
        die("fflush");
    }

    exit(EXIT_SUCCESS);
}