#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    //1st way to create an string
    char arr01[10];
    printf("%lu\n", sizeof(arr01));       //10,开10个空间就是10个，最后一个给'\0'
    printf("%lu\n", strlen(arr01));       //啥也没填充，空的

    //2nd way: dynamic 
    char* arr02 = malloc(10 * sizeof(char));    //standard: type* ptr = malloc(num * sizeof(type));
    if (arr02 != NULL){
        strcpy(arr02, "HEllO");                //strcpy
        printf("%lu\n", sizeof(arr02));       //8? : size of ptr in 64-bit system
        printf("%lu\n", strlen(arr02));       //5 : HELLO, '\0' not included in calculation
        //free allocation
        free(arr02);
    }

    char arr03[3];
    strcpy(arr03, "\0");
    printf("%lu\n", strlen(arr03));
    return 0;
}