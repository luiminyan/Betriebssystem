#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     //fork header file, unix standard

void kill(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}
int main(){
    pid_t pid;
    fprintf(stdout, "[%d]Start\n", getpid());

    fflush(NULL);   //刷新所有的流,在fork以前
    pid = fork();           //代码分裂

    if (pid < 0){               //错误
        kill("fork");
    }
    else if(pid == 0){          //
        fprintf(stdout, "[pid:%d]child process\n", getpid());
    }
    else {      //parent
        sleep(1);   //让父进程sleep，但是写法不好
        fprintf(stdout, "[pid:%d]parent process\n", getpid());
    }
    // getchar();

    fprintf(stdout, "[%d]End\n", getpid());
    exit(EXIT_SUCCESS);
}

