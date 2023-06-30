#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>     //fork(), getcwd
#include <sys/wait.h>   //wait
#include <errno.h>
#include <string.h>
#include "plist.h"      //need to use a function from plist.h

// #include <sys/types.h>  //fork
#define MAX_LENGTH 1337 //max 1337 Zeichen (User input)

//Fehlerbehandlung (gesetzte errno)
static void die(char* func_name){
    perror(func_name);
    exit(EXIT_FAILURE);
}

//Warnung
static void warn(char* func_name){
    fprintf(stderr, "%s", func_name);
    //fputs(func_name, stderr);
}

//1st step: print the current path - done!
static void print_path() {
    //set errno back to 0, as we need it to deal with error by getcwd
    errno = 0;

    //create buffer for path
    int buff_path_len = 255;    //length of buff_path
    char* buff_path = malloc(sizeof(char*) * buff_path_len);
    //error by malloc
    if (buff_path == NULL) {
        die("malloc");
    }

    //getcwd, if NULL error
    while (getcwd(buff_path, buff_path_len) == NULL) {
        //if errno = ERANGE, buffer not big enough, increase buffer
        if (errno == ERANGE) {
            buff_path_len *= 2; 
            buff_path = realloc(buff_path, sizeof(char*) * buff_path_len);
            //error by realloc
            if (buff_path == NULL) {
                die("realloc");
            }
        }
        else die("getcwd");
    }

    //print out the path
    if (printf("%s:", buff_path) < 0) {
        die("fprintf");
    }

    //fflush stdout
    if (fflush(stdout) == EOF){
        die("fflush");
    }

    //!!!!!free the allocation
    free(buff_path);
}

//print_jobs, the callback function
static bool print_jobs(pid_t pid, const char* str){
    if (printf("pid: %d, job: %s", (int) pid, str) < 0) {
        die("printf");
    }
    return 0;
} 

//4th step: collect zombies
static void collect_zombies(){
    int status;         //store the child process return status
    pid_t wait_val;     //for fork

    while (1) {
        //reset errno
        errno = 0;

        wait_val = waitpid(-1, &status, WNOHANG);      // (-1: for all child process, ..., wait no hang)
        //error by waitpid
        if (wait_val == -1) {
            //special case: all zombies are collected / no zombie found
            if (errno == ECHILD) {
                break;      //zombie collection is done, break loop
            }
            else die("waitpid");
        }

        //child still running
        else if (wait_val == 0) {
            continue;
        }

        //else: child terminated
        //create buffer to store the job
        char buff_job[MAX_LENGTH + 1];      //"\n" is removed by plist operations

        //done: remove zombie from list
        if (removeElement(wait_val, buff_job, sizeof(buff_job)) == -1) {
            //zombie is not found in list
            continue;
        }

        

        if (WIFEXITED(status) == true) {
            printf("Exitstatus [%s] = %d\n", buff_job, status);
        }
        else printf("[%s] did not exit with success", buff_job);

        //error by printf
        if (ferror(stdout)) {
            die("printf");
        }

        //flush stdout
        if (fflush(stdout) == EOF) {
            die("fflush");
        }
    }
}

int main(int argc, char** argv){
    //buffer for input
    char buff_input[MAX_LENGTH + 1];
    int foreground = 1;         //Vordergrund, the parent process
    
    //repeat when no error
    while (1) {
        //Vordergrund
        foreground = 1;

        //collect zombies before print path
        collect_zombies();

        //print the path
        print_path();

        //flush stdin
        if (fflush(stdin) == EOF) {
            die("fflush");
        }
        

        //2nd read user input
        /* read input begins */
        if (fgets(buff_input, MAX_LENGTH + 1, stdin) == NULL) {
            //no character, terminate program 
            if (feof(stdin)) {
                exit(EXIT_SUCCESS);
            }
            //real error
            die("fgets");
        }

        //check if too short input
        if (strlen(buff_input) < 2) {
            continue;
        }

        //check if input too long
        if (strlen(buff_input) == MAX_LENGTH && buff_input[MAX_LENGTH - 1] != '\n') {
            //strlen: "\0" not calculated
            warn("Input too long!\n");

            //clear left input in stdin
            int buff_char;
            do {
                buff_char = getc(stdin);
            } while (buff_char != EOF && buff_char != '\n');
            //error by getc
            if (ferror(stdin)) {
                die("getc");
            }

            //next input
            continue;
        }

        //remove the end symbol: 2 cases
        //in "Vordergrund": remove "\n"
        if (buff_input[strlen(buff_input) - 1] == '\n') {
            buff_input[strlen(buff_input) - 1] = '\0';
        }

        //in "Hintergrund": remove "&"
        if (buff_input[strlen(buff_input) - 1] == '&') {
            //now we are in "Hintergrund"
            foreground = 0;     
            buff_input[strlen(buff_input) - 1] = '\0';
        }

        //copy the buff_input
        char* job = buff_input;
        /* read input ends */

        //do the strtok, seperate the cmd into tokens
        char* token = NULL;
        int index = 0;
        char* cmd_list[strlen(buff_input) / 2 + 2];

        //extract the first token
        token = strtok(buff_input, " \t");

        //empty cmd
        if (cmd_list[0] == NULL || strlen(cmd_list[0]) == 0){
            continue;       //skip
        }

        //do the rest tokens
        while(token) {
            cmd_list[index] = token;    //store into list
            index++;        //increase the index for iteration
            token = strtok(NULL, " \t");    //extract the next token
        }
        /* END OF STRTOK */      

        //cmd = cd + path = cmd_list[0] + cmd_list[1]
        if (strcmp(cmd_list[0], "cd") == 0) {
            if (cmd_list[1] == NULL) {
                warn("Parameter missing.");
            }
            else if (cmd_list[2] != NULL) {
                warn("Too many parameters!");
            }
            else {
                if (chdir(cmd_list[1]) == -1) {
                    die("chdir");
                }
            }
            continue;
        }

        //cmd = jobs
        if (strcmp(cmd_list[0], "jobs") == 0) {
            if (cmd_list[1] != NULL) {
                warn("Too many parameters.");
            }
            else {
                walkList(print_jobs);
            }
            continue;
        } 

        //create child processes to do the job
        pid_t pid = fork();
        
        //error by fork
        if (pid < 0) {
            die("fork");
        }

        //child process
        else if (pid == 0) {
            //do the job
            execvp(cmd_list[0], cmd_list);
            die("execvp");

        }

        //parent process
        if (!foreground){
            printf("enter background\n");

            if (-2 == insertElement(pid, job)) {
                warn("malloc or strdup");
                exit(EXIT_FAILURE);
            }
        }
        else {
            printf("enter frontend\n");
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                die("waitpid");
            }
            // Hier Exitstatus ausgeben.
            if(WIFEXITED(status)) {
                // exit with success
                 printf("Exitstatus [%s] = %d\n", job, WEXITSTATUS(status));
                }
            else {
                printf("[%s] did not exit with success", job);
            }
        }
    }
    exit(EXIT_SUCCESS);
}

