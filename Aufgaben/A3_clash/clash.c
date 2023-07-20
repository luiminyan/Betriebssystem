#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     //waitpid, fork
#include <errno.h>      //errno
#include "plist.h"
#define MAX_LINE 1337   //max input line length ('\n' included)

//deal with errno error
static void die (char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
//warn
static void warn (char* msg) {
    fprintf(stderr, "%s\n", msg);
}

//print the current directory
static void prompt_symbol(){
    //reset errno
    errno = 0;

    //create buffer for directory
    int cwd_len = 500;      //init length for cwd
    char* cwd = malloc(sizeof(char*) * cwd_len);
    //error by malloc
    if (cwd == NULL) {
        die("malloc");
    }

    //getcwd
    while (!getcwd(cwd, cwd_len)) {
        //if null ptr. is returned by getcwd
        //cwd length not enough
        if (errno == ERANGE) {
            cwd_len *= 2;
            cwd = realloc(cwd, sizeof(char*) * cwd_len);
            //error by realloc
            if (cwd == NULL) {
                die("realloc");
            }
            //error dealt: reset errno
            errno = 0;
        }
        //otherwise, a "real" error by getcwd
        else die("getcwd");
    }

    //print the path
    if (printf("%s: ", cwd) < 0) {
        die("printf");
    }

    //flush stdout
    if (fflush(stdout) == EOF) {
        die("fflush");
    }

    //free the allocation
    free(cwd);
}

static void collect_zombies() {
    //collect all background zombie processes
    while (1) {
        //reset errno
        errno = 0;
        int status;
        pid_t wpid = waitpid(-1, &status, WNOHANG); //do not block parent process to wait
        if (wpid == -1) {
            //no child to collect
            if (errno == ECHILD) {
                break;
            }
            //error by waitpid
            else die("waitpid");
        }
        //child process still not terminated
        else if (wpid == 0) {
            continue;
        }
        
        //zombie collected
        //remove zombie process from list
        //create buffer for "jobs"
        char jobs[MAX_LINE + 1];    
        if (removeElement(wpid, jobs, sizeof(jobs)) == -1) {
            //process not in list
            continue;
        }
        
        //if process successfully exited
        if (WIFEXITED(status)) {
            printf("Exitstatus [%s] = %d\n", jobs, WEXITSTATUS(status));
        }
        else printf("Process[%s] does not successfully exited.\n", jobs);

        //flush
        if (fflush(stdout)) {
            die("fflush");
        }
    }
}

static bool print_jobs(pid_t pid, const char* jobs){
    if (printf("pid: %d, jobs: %s\n", pid, jobs) < 0) {
        die("printf");
    }
    return 0;
}

int main (int argc, char* argv[]) {
    while (1) {
        //collect zombies (back ground processes)
        collect_zombies();

        //background
        int isBackground = 0; 

        //done: prompt the current working directory
        prompt_symbol();

        //todo: read a line of input
        //create input line buffer
        char line[MAX_LINE + 1];    // +1 for '\0'

        //fgets
        if (fgets(line, MAX_LINE + 1, stdin) == NULL) {
            //error in stdin
            if (ferror(stdin)) {
                die("fgets");
            }
            //empty line, user input: ctrl + d...
            else break;
        }

        //regular input modify
        //input too short
        if (strlen(line) < 2) {
            //only '\n'
            warn("Input too short!");
            continue;
        }

        //input is too long
        if (strlen(line) == MAX_LINE && line[MAX_LINE - 1] != '\n') {
            warn("Input too long > 1337");
            //done: skip all the rest chars in this line
            int c;
            do {
                c = getchar(); /* getchar() = fgetc(stdin) */
            } while (c != '\n' && c != EOF);
            //error in stdin
            if (ferror(stdin)) {
                die("getchar");
            }
            continue;
        }

        //remove the '\n' at the end of the input
        //case 1: if process in foreground
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        //case 2: if process in background
        if (line[strlen(line) - 1] == '&') {
            //process is in BACKGROUND
            isBackground = 1;
            line[strlen(line) - 1] = '\0';
        }

        //seperate the line
        //make a dupplication of  line
        char* job = strdup(line);
        //error by strdup
        if (job == NULL) {
            die("strdup");
        }
        //strtok
        int index = 0;
        //list for seperate arguments
        char* arg_list[MAX_LINE];

        //extract the first token
        arg_list[index] = strtok(line, " \t");
        if (!arg_list[0]) {
            //no proper arg / cmd, read nect line / input
            continue;
        }
        index++;

        //extract the rest
        while((arg_list[index] = strtok(NULL, " \t")) != NULL) {
            index++;
        }

        /*
        //test for strtok
        printf("sum of parameters: %d\n", index);
        for (int j = 0; j <= index; j++) {
            printf("%s\n", arg_list[j]);
        } */ 

        //cmd = "cd"
        if (strcmp(arg_list[0], "cd") == 0) {
            if (index < 2 || index >= 3 || arg_list[1] == NULL) {
                warn("too less|much arguments\n");
                continue;
            }
            if (chdir(arg_list[1]) != 0) {
                warn("chdir");
            }
        }     

        //cmd = "jobs"
        if (strcmp(arg_list[0], "jobs") == 0) {
            if (arg_list[1] != NULL) {
                warn("too much arguments");
                continue;
            }
            //do the walklist
            walkList(print_jobs);
            
        }

        //create child process 
        pid_t pid = fork();
        //error by fork
        if (pid < 0) {
            die("fork");
        }

        //child process
        else if (pid == 0) {
            //change the content
            execvp(arg_list[0], arg_list);
            //if fails, program runs to this line
            die("execvp");
        }

        //the parent process
        //if process in background
        if (isBackground) {
            //do not wait here
            //TODO: add the child process in the list in plist.c
            if (insertElement(pid, job) == -2) {
                warn("malloc / strdup");
                exit(EXIT_FAILURE);
            }
        }
        else {
            //if process in foreground
            int status;
            //waitpid
            //error
            if (waitpid(pid, &status, 0) < 0) {
                die("waitpid");
            }
            //successfully exited
            if (WIFEXITED(status)) {
                if (printf("Exitstatus [%s] = %d\n", job, WEXITSTATUS(status)) < 0) {
                    die("printf");
                }
            }
            else {
                if (printf("Process[%s] does not exit successfully\n", job) < 0) {
                    die("printf");
                }
            }
        } 
    }
    exit(EXIT_SUCCESS);
}
