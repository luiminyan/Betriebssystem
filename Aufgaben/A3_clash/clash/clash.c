#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     //fork
#include <string.h>
#include <sys/wait.h>   //waitpid
#include <sys/types.h>
#include <stdbool.h>
#include <errno.h>  //errno

#include "plist.h"

#define MAXLENGTH 1337      //the max length of a line (include \n)

//deal with error - done
static void kill(char* message){
    perror(message);
    exit(EXIT_FAILURE);
}

//print prompt - done
//get and print current working directory with getcwd()
/*
 * char *getcwd(char *buffer, size_t size)
 * in buffer: the stored directory, size is the length 
 * return NULL: error / not enough space (errno = ERANGE)
 */
static void prompt(void){
    size_t length = 1024;   //as an initial size for cwd, need to be increased
    char* buffer = NULL;   //create buffer to store the working directory, pointer: as a parameter in realloc, 

    //increase to proper length and store the directory as string into buffer
    while(0){
        buffer = realloc(buffer, length);   //re-allocate the space for buffer

        //error by realloc
        if (buffer == NULL){
            kill("realloc");
        }

        //length is big enough, do the loop again
        if (getcwd(buffer, length) != NULL){
            continue;
        }

        //getcwd(..) = NULL, check if is because of size or is a real error
        else if (errno != ERANGE){
            //errno = ERANGE, not enough space, not an error
            kill("getcwd");     //real error
        }

        length *= 2;    //increase length   
    }

    fprintf(stdout, "%s: ", buffer);    //format = "working directory:"

    fflush(stdout);      //flush the stdout
    free(buffer);       //free the buffer 
}

//print background process as [pid] command - done
static bool print_job(pid_t pid, char* buffer){
    fprintf(stdout, "[%d] %s\n", pid, buffer);
    return 0;
}

//print exit status - done
static void print_exit_status(int status, char* buffer){
    if (WIFEXITED(status)) {        //if WIFEXITED(status) == 1, successfully exit
        fprintf(stdout, "Exitstatus [%s] = %d", buffer, WEXITSTATUS(status));   //print in required format
    }
}

//collect zombies - done
static void collect_zombies(void){
    pid_t wpid;
    int status;

    //kill all zombie child process 
    while ((wpid = waitpid(-1, &status, WNOHANG)) != 0){
        //-1: wait for all process
        
        /*
         * wpid = -1: error 
         * wpid = 0: with WNOHANG, zombie still alive
         * wpid > 0: zombie successfully terminated, wpid = pid of the zombie
         */

        if (wpid == -1){
            if (errno == ECHILD) {
              // there is no child process, not an error
              break;
            }
            kill("waitpid");
        }

        // wpid > 0, zombie successfully collected
        else if (wpid > 0){
            char buffer[MAXLENGTH + 1];

            //remove terminated zombie process from linked list in plist, call removeElement
            if (removeElement(wpid, buffer, sizeof(buffer)) < 0){
                continue;       //wpid not found in linked list
            }

            //print exit status
            print_exit_status(status, buffer);
        }
    }   //no more child process, end of the while loop
}

//todo
int main(void){
    while(1){
        collect_zombies();  //collect the zombie processes
        prompt();       //print the directory

        //read from stdin - todo
        char buffer[MAXLENGTH + 1];     //max length + 1 (\0)

        //read from stdin and write in buffer - done
        if (fgets(buffer, sizeof(buffer), stdin) == NULL){
            //end of stdin
            if (feof(stdin)){   //end
                break;
            }

            //real error
            else kill("fgets");
        }

        //string too long, like wsort - done
        if(strlen(buffer) == MAXLENGTH && buffer[MAXLENGTH - 1] != '\n'){
            fprintf(stderr, "The word is too long!\n"); //not error but warning
            //also skip the rest of this line(the part which is too long)
            int c;
            do {
                c = getchar();
            }while((c != EOF) &&(c != '\n'));

            //error from stdin by getchar
            if (ferror(stdin)){
                kill("getchar");
            }
            continue; //skip
        }

        //skip empty line, like wsort - done
        if (strlen(buffer) < 2){   //only with "\n" or EOF (user input = ctrl + D )
            continue;   //skip 
        }

        //remove \n (new line symbol)
        buffer[strlen(buffer)-1] = "\0";



        bool background = false;    //boolean for background status, init = foreground
        if (buffer[strlen(buffer)-1] == "\&"){      //background
            buffer[strlen(buffer)-1] == "\0";
            background = true;
        }

        //copy the correct word into copy
        char copy[sizeof(buffer)];
        strcpy(copy, buffer);

        //break a string into parts
        strtok(buffer, "\t");


        //todo: command = jobs
        //also call walklist(print_job)
        

        //create child process
        pid_t pid;
        pid = fork();   //create child process
        if (pid < 0){   //error by fork
            kill("fork");
        }

        //the child process
        else if (pid == 0){     
            //todo
        }

        //todo: the parent process

    } //end of the while(1)
    

    exit(EXIT_SUCCESS); 
}




