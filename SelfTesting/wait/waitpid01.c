#include <stdio.h>
#include <stdlib.h>     //exit
#include <sys/wait.h>   //wait / waitpid
#include <unistd.h>     //fork

void kill(char* message){
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    exit(EXIT_SUCCESS);
}