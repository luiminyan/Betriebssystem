#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEFT 30000000
#define RIGHT 30000200

void kill(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argsc, char *argv[]){ 
    int i, j, mark; 
    pid_t pid; 
    for (i = LEFT; i <= RIGHT; i++) 
    { 
        fflush(NULL);
        pid = fork(); 
 
        if (pid < 0){   //error 
            kill("fork"); 
        } 
 
        if (pid == 0){     //child process 
            mark = 1; 
            for (j = 2; j < i/2; j++){ 
                if (i % j == 0){ 
                    mark = 0; 
                    break; 
                } 
            }  
            if (mark){ 
                fprintf(stdout, "%d is prime\n", i); 
            }
            // sleep(1000);    //让父进程先结束
            exit(EXIT_SUCCESS); 
        } 
    } 

    // sleep(10);

    exit(EXIT_SUCCESS); 
}
