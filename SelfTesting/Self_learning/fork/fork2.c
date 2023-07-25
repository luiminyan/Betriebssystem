#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void kill(const char* message){
   perror(message);
   exit(EXIT_FAILURE);
}

int main(){
   int i;
   for (i = 0; i < 2; i++) {
       pid_t fpid = fork();    //create new process

       if (fpid < 0){          //error by fork
           kill("fork");
       }
       if (fpid == 0){    //fpid = 0, child process
           fprintf(stdout, "iterration %d: This is child process, pid = %4d, parent pid = %4d, fpid = %4d\n", i, getpid(), getppid(), fpid);
       }
       else {      //fpid > 0, parent process
           fprintf(stdout, "iterration %d: This is parent process, pid = %4d, parent pid = %4d, fpid = %4d\n", i, getpid(), getppid(), fpid);
       }
   }
   exit(EXIT_SUCCESS);
}

