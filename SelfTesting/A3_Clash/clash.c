#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     //fork, free
#include <errno.h>      //errno

#include "plist.h"      //need to use functions from plist.c

#define MAX_LINE 1337   //'\n' included

//error when errno is set
static void die(char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

//warn
static void warn(char* msg){
    fprintf(stderr, "%s", msg);
    //error by fprintf
    if (ferror(stderr)) {
        die("ferror");
    }
}

static void print_cwd(){
    //create buf for cwd
    int cwd_len = 255;  //init len
    char* cwd = malloc(sizeof(char*) * cwd_len);
    if (cwd == NULL) {
        die("malloc");
    }

    //getcwd
    while (getcwd(cwd, cwd_len) == NULL) {
        //erange, buf size too small
        if (errno == ERANGE) {
            cwd_len *= 2;
            cwd = realloc(cwd, sizeof(char*) * cwd_len);
            //error by realloc
            if (cwd == NULL) {
                die("realloc");
            }
            //reset errno
            errno = 0;
        }
        //"real" error by getcwd
        else die("getcwd");
    }

    /*  //print cwd
    if (fputs(cwd, stdout) == EOF) {
        die("fputs");
    } */

    //print cwd
    if (printf("%s: ", cwd) < 0) {
        die("printf");
    }

    //fflush, flush out all the unprinted buf in stdout
    if (fflush(stdout) == EOF) {
        die("fflush");
    }

}

static void colllect_zombies() {
    //collect all background zombie processes
    pid_t wpid;
    int status;
    //reset errno
    errno = 0;

    while (1) {
        wpid = waitpid(-1, &status, WNOHANG);   //wait no hang
        //error or signal aborts the call
        if (wpid == -1) {
            //special case: no existing unwaited-for child / all children are collected
            if (errno == ECHILD) {
                break;   //stop waiting
            }
            //real error
            else die("waitpid");
        }

        // no stopped or exited children
        else if (wpid == 0) {
            continue;   //continue waiting
        }

        //wpid > 0, successfully collected a process
        //remove collected zombie from list in plist.c
        //create job buffer by func in plist.c to receive the removed process 
        char job_buf[MAX_LINE + 1];     // +1 for '\0'
        if (removeElement(wpid, job_buf, sizeof(job_buf)) == -1) {
            //pid not found
            continue;   //collect the next process
        };
        
        //if process successfully exited
        if (WIFEXITED(status)) {
            printf("Exitstatus [%s] = %d\n",job_buf, WEXITSTATUS(status));
        }
        else printf("Process [%s] did not exit successfully\n", job_buf);
    }

}

//print_list
static bool print_list(pid_t pid, const char* jobs) {
    printf("pid: %d, jobs: %s", pid, jobs);
    return true;
}

int main(int argc, char* argv[]){
    while (1) {
        //if process in background
        int background = 0;

        //collect zombies (background processes) before prompt
        colllect_zombies();

        //print current directory
        print_cwd();

        //read a line from stdin
        //fflush the stdin before read input
        if (fflush(stdin) == EOF) {
            die("fflush");
        } 

        //create line buffer for stdin
        char line[MAX_LINE + 1];    //for '\0'
        //fgets appends '\0' 
        if (fgets(line, MAX_LINE + 1, stdin) == NULL) {
            //error in stdin
            if (ferror(stdin)) {
                die("fgets");
            }
            //empty input, break by user...
            else break;     //end the program, the reason, this read input cannot be written as a seperate func
        }

        //modify the input
        //if input too short
        if (strlen(line) < 2) {
            warn("Input too short!");
            continue;
        }

        //input too long
        if (strlen(line) == MAX_LINE && line[MAX_LINE - 1] == '\n') {
            warn("Input too long!");
            //skip rest bits in line
            int c;
            do {
                //read a char
                c = getchar();
            } while (c != '\n' && c != EOF);
            //error by getchar
            if (ferror(stdin)) {
                die("getchar");
            }
            //read next line
            continue;
        }

        //remove the end symbol at end
        //if process in foreground
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        //if process in background
        if (line[strlen(line) - 1] == '&') {
            background  = 1;        //now in background
            line[strlen(line) - 1] = '\0';  //remove end symbol
            //do not wait for the end of the process, directly take a new prompt
        }

        //break line into cmd and args
        //create a list
        char* arg_list[MAX_LINE];
        int index = 0;

        //duplicate the string
        char* job = strdup(line);
        //error by strdup
        if (job == NULL) {
            die("strdup");
        }

        //strtok: extract the first string
        arg_list[index] = strtok(line, " \t");
        //no valid arg
        if (arg_list[index] == NULL) {
            //skip this line
            break;
        }
        index++;
        //extract the rest
        while ((arg_list[index] = strtok(NULL, " \t")) != NULL) {
            index++;
        }
        
        /*   //testing for strtok
        printf("sum of parameters: %d\n", index);
        for (int j = 0; j <= index; j++) {
            printf("%s\n", arg_list[j]);
        } */

        //special commands
        //cd
        //use strcmp: compare the content of the strings, when they are not on the same address
        if (strcmp(arg_list[0], "cd") == 0) {
            //change working directory into the second argument
            if (index < 2 || arg_list[1] == NULL) {
                warn("correct formate: cd path\n");
                continue;
            }
            //chdir, change directory
            if (chdir(arg_list[1]) == -1) {
                die("chdir");
            }
            continue;  //read next line
        }

        //jobs
        if (strcmp(arg_list[0], "jobs") == 0) {
            //TODO: print out all the background processes
            if (index > 1 || arg_list[1] != NULL) {
                warn("Too mang aruguments!");
                continue;
            }
            //TODO: call walklist in plist.c
            walkList(print_list);
            continue;
        }

        //create child process to do the job
        pid_t pid = fork();
        //error by fork
        if (pid < 0) {
            die("fork");
        }

        //child process
        else if (pid == 0) {
            //with this single child process, execute all the arguments
            //execvp(arg[0] / prog_name, [args])
            execvp(arg_list[0], arg_list);
            //if error, come to this line
            die("execvp");
        }

        //parent process
        //if process in background
        if (background == 1) {
            //pid = child process ID, job = the duplicate of cmdLine
            //insert background child process into list
            if (insertElement(pid, job) == -2) {
                //errno not set in clash.c 
                warn("malloc or stdup dies");
                exit(EXIT_FAILURE);
            }
        }
        //if process is in foreground, zombies need to be collected
        else {
            int status;
            //error by waitpid
            if (waitpid(pid, &status, 0) < 0) {
                die("waitpid");
            }

            if (WIFEXITED(status)) {
                printf("Exitstatus [%s] = %d\n", job, WEXITSTATUS(status));
            }
            else printf("Process [%s] did not exit successfully\n", job);
        }
    }
    exit(EXIT_SUCCESS);
}
