#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>     //malloc, calloc, exit...
#include "malloc.h"     //malloc.h is not from the standard library

// Die Initialisierung globaler Variablen auf Null ist in C auch implizit möglich
static Student *array;
static size_t arrayLength;

int addStudent(const Student *student) {

    // Entfernen wenn implementiert
    Student copy;
    copy.age = student->age;   //store age into copy

    size_t nameLength = strlen(student->name);   //use strlen (count till \0) but not sizeof() (size of ptr)
    copy.name = malloc(nameLength);
    if (copy.name == NULL){         //error
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(copy.name, student->name);   //copy student name into copy

    //enlarge the array length
    Student* dummy_array = realloc(array, arrayLength + 1);
    if (dummy_array == NULL){
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    array = dummy_array;
    array[arrayLength] = copy;
    exit(EXIT_SUCCESS);
}

int printStudent(void) {

    // Entfernen wenn implementiert
    for (size_t i = 0; i < arrayLength; i++){
        fprintf(stdout, "Name: <%s>, Alter: <%d>\n", array[i].name, array[i].age);
    }
    //-1 bei Schreibfehler, 0 sonst
    return fflush(stdout);      //flush the stdout, success by 0
}

void freeStudent(void) {
    //free the allocation
    //first free the name of single elements, then the whole array
    for (size_t i = 0; i < arrayLength; i++){
        free(array[i].name);
    }
    free(array);
    array = NULL;
    arrayLength = 0;    //set arrayLength to 0
}

char *join(const char *a, const char *b) {
    // Format zum Konkatenieren zweier Zeichenketten
    const char *format = "%s%s";

    // Berechnen der Ausgabelänge mit snprintf

    // Puffer von zuvor berechneter Größe mit malloc allokieren

    // Sicheres Schreiben mit snprintf in Puffer korrekter Länge

    // Puffer zurückgeben


    // Entfernen wenn implementiert
    return NULL;
}

int main(void){
   
    Student* stu1;
    stu1->age = 24;
    stu1->name =  "Minhua";
    Student* stu2;
    stu2->age = 51;
    stu2->name = "Milan";
    addStudent(stu1);
    addStudent(stu2);

    printStudent();

    freeStudent();


    exit(EXIT_SUCCESS);
}
