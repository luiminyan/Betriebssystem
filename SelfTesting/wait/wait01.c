#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void kill(char *message){       //deal with the error
    perror(message);
    exit(EXIT_FAILURE);
} 

int main(int argsc, char *argv[]){      //main 进程
    pid_t fpid;
    fpid = fork();      //create a child process
    int status;

    if (fpid < 0){      //error by fork
        kill("fork");
    }
    else if (fpid == 0){     //child process
        fprintf(stdout, "[pid:%d]:child process running\tppid:%d\t\n", getpid(), getppid());
    }
    else{      //parent process
        pid_t wpid = wait(&status);
        if (wpid == -1){
            kill("wait");
        }
        fprintf(stdout, "[pid:%d]:parent process running\tppid:%d\n", getpid(), getppid());
        fprintf(stdout, "The child process with pid = %d is killed with status = %d\n", wpid, WIFEXITED(status));
    }
    
    fprintf(stdout, "END: [pid:%d], ppid:%d\n", getpid(), getppid());
    exit(EXIT_SUCCESS);
}