// make isort && isort

#include <stdlib.h>
#include <stdio.h>

static const int COUNT = 20;
static const int RANDOM_RANGE = 100; // Zufallszahlen in [0, RANDOM_RANGE)

// Übliche Hilfsfunktion: Fehlerausgabe und Beenden
void fatal_exit(const char* context)
{
    perror(context);        //when errno set
    exit(EXIT_FAILURE);
}


// Hilfsfunktion um Array anzuzeigen
void print_array(const int* array, int count)
{
    for(int i = 0; i < count; ++i)
        printf("%i ", array[i]);
    printf("\n");
}


// Vergleichsfunktion
int compare_func(const void* a, const void* b){
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;

    return *ia - *ib;
    /*
     * if (*ia < *ib) return -1;
     * else if (*ia = *ib)return 0;
     * else return -1;
     */

}


int main()
{
    // Speicher allokieren
    int* array = malloc(sizeof(int) * COUNT);
    //error by malloc
    if (array == NULL){
        fatal_exit("malloc!");
    }

    // Array mit Zufallszahlen initialisieren
    for (int i = 0; i < COUNT; i++){
        array[i] = rand()%RANDOM_RANGE;
    }
    

    printf("Array vor dem Sortieren\n");
    print_array(array, COUNT);

    // Sortieren
    qsort(array, COUNT, sizeof(int), compare_func);

    printf("Array nach dem Sortieren\n");
    print_array(array, COUNT);

    fflush(stdout);
    free(array);        //malloc - free

    return EXIT_SUCCESS;
}