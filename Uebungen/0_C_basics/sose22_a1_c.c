#include <stdio.h>
#include <stdlib.h>     //exit

void die (char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

void func(void){
    static int a = 42;
    if ( printf("%d\n", a) < 0){
        die("printf");
    }
    a++;
}

int main(int argc, char* argv[]){
    for (int i = 0; i < 3; i++) {
        //do 3 times
        func();
        //printf("%d\n", a);      //not possible, as a is not defined
    }

    exit(EXIT_SUCCESS);
}
