#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     //process
#include <errno.h>      //errno
#include <sys/wait.h>
#include <sys/types.h>

static void die(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void warn(char* msg) {
    fprintf(stderr, "%s\n", msg);
}

int main(int argc, char* argv[]) {
    /*
     * argv[] = address prog_name arg0 arg1 ...
     * argc = |argv[]|
    */

    //if arg num < 2, error
    if (argc < 2) {
        warn("argument number < 2");
        exit(EXIT_FAILURE);
    }

    //TODO: for each argument, create a process, and execute the job
    for (int i = 2; i < argc; i++) {
        pid_t pid = fork();
        //error by fork
        if (pid == -1) {
            die("fork");
        }
        //child process
        else if (pid == 0) {
            //do the execlp
            execlp(argv[1], argv[1], argv[i], NULL);
            die("execlp");
        }

        //parent process, collect zombies
        else {
            pid_t wpid;
            int status;
            while (1) {
                //reset errno
                errno = 0;

                //waitpid(-1 = all, wait_status, WNOHANG)
                //wait no hang, parent process does not wait
                pid_t wpid = waitpid(-1, &status, WNOHANG);

                //error by waitpid
                if (wpid == -1) {
                    //if no more zombie process
                    if (errno == ECHILD) {
                        break;
                    }
                    //else
                    die("waitpid");
                }

                //child process is still running
                else if (wpid == 0) {
                    continue;
                }

                if (WIFEXITED(status)) {
                    //child process terminated
                    if (printf("Child process ends with status %d\n", WEXITSTATUS(status)) < 0) {
                        die("printf");
                    }
                }
            }

            //flush
            if (fflush(stdout) == EOF) {
                die("fflush");
            }
        }
    }


    exit(EXIT_SUCCESS);
}