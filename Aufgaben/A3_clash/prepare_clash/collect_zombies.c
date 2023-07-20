#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
static void die (char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void collect_zombies () {
    pid_t wpid;
    int status;

    //collect all zombies
    while (1) {
        //reset errno
        errno = 0;

        //waitpid
        wpid = waitpid(-1, &status, WNOHANG);

        //error by waitpid
        if (wpid == -1) {
            //no more zombies
            if (errno == ECHILD) {
                break;
            }
            else die("waitpid");
        }

        //child process still running
        else if (wpid == 0) {
            continue;
        }

        //terminated
        if (WIFEXITED(status)) {
            printf("Child exits with status: %d\n", status);
        }
        else printf("Child does not exit successfully.\n");
        //error by printf
        if (ferror(stdout)) {
            die("printf");
        }

       /*  //fflush stdout
        if (fflush(stdout) == EOF) {
            die("fflush");
        } */
    }
}

int main (int argv, char** argc) {
    //create child process
    pid_t pid = fork();
    //error
    if (pid == -1) {
        die("fork");
    }

    //child process
    else if (pid == 0) {
        //do the job
    }

    //parent process
    else {
        collect_zombies();
    }
    exit(EXIT_SUCCESS);
}

