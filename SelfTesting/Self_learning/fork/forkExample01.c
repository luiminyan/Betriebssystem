#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void kill(char* message){
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
    int count = 0;  //number of process
    pid_t fpid; //process number
    fpid = fork();  //create new process

    if (fpid < 0){
        kill("fork");
    }
    else if (fpid == 0){
        fprintf(stdout, "This is the child process with pid %d.\n", getpid());
        count++;
    }
    else {
        fprintf(stdout, "I'm the parent process with pid of %d\n", getpid());
        fprintf(stdout, "The new created child process is with the pid of %d\n", fpid);
        count++;
    }
    fprintf(stdout, "There are %d processes in total.\n", count);
    exit(EXIT_SUCCESS);
}