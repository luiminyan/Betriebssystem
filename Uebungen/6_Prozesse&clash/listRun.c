#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // für fork()
#include <sys/wait.h>   // für wait() und waitpid()
#include <sys/types.h>  // für fork()

static void die(char* func_name){
    perror(func_name);
    exit(EXIT_FAILURE);
}

/*
 * ./listRun echo Das ist ein Test
 * argc = 6 (listRun included)
 * argv[0]: listRun
 * argv[1]: echo
 * ...
 * todo:
 * echo(arg1)
 * echo(arg2)
 * ...
 */

int main(int argc, char** argv){
    //too few argument
    if (argc < 2) {
        fprintf(stderr, "too few argument");
        exit(EXIT_FAILURE);
    }    
    else {
        for (int i = 2; i < argc; i++) {
             //create child process
            pid_t pid = fork();

            //error by fork
            if (pid < 0) {
                die("fork");
            }

            //child process
            else if (pid == 0) {
                //do the job
                //execlp(prog.name(echo), argu[0] (!repeat echo!), argu1, ... null)
                //when it works, child process directly ends and becomes zombie
                execlp(argv[1], argv[1], argv[i], NULL);  
                die("execlp");  //error
            }
            
            //parent process
            else {
                //wait for child process
                int status;
                pid_t value = wait(&status);
                // pid_t value = waitpid(pid, &status, WNOHANG);   //使用WNOHANG，不堵塞parent process
                // error by wait()
                if (value == -1) {
                    die("wait");
                    continue;   //这个iteration的wait出问题了，但是可以等下个iteration
                }

                //child process does not exit yet
                else if (value == 0) {
                    printf("Child process is still running!");
                    if (ferror(stdout)) {
                        die("printf");
                    }
                }

                //child process exits
                if (WIFEXITED(status)) {
                    if (printf("child process with pid%d exit with code%d\n", pid, WEXITSTATUS(status)) < 0){
                        die("printf");
                    }
                }
            }
        }
    }
    exit(EXIT_SUCCESS);
}
