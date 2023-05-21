#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
            exit(EXIT_SUCCESS);     //子进程exit
        } 
    } 

    //收尸
    //如果想保留子进程的状态 
    /* int status;
    for (i = LEFT; i <=RIGHT; i++){
        wait(&status);  //
    }
    */

    //收尸，但是不保存状态

    for (i = LEFT; i <=RIGHT; i++){
        wait(NULL);     //父进程等着收尸
    }



    exit(EXIT_SUCCESS); 
}
