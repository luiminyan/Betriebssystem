#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <limits.h> 
#include <errno.h> 
#include <unistd.h>     //process, fork, wait
#include <string.h> 
#include <sys/types.h> 
#include <sys/wait.h>
static void die(const char *msg) { 
    perror(msg);
    exit(EXIT_FAILURE);
}

static void usage(void) {
    fprintf(stderr, "Usage: party <#parallel processes> <commandlines>\n"); exit(EXIT_FAILURE);
}

struct process {
    pid_t pid; //PID des Processes
    //eigene Mitglider
    char* cmd;
};

// Makros, Funktionsdeklarationen, globale Variablen
static pid_t run(char *cmdline);
static void waitProcess(struct process *processes, size_t size);
static int running_process = 0;

//party <#parallel processes> <commandlineS>
//func main
int main(int argc, char* argv[]) {
    // Befehlszeilenargument(e) prüfen
    if (argc < 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    // Befehlszeilenarg. <n> mit strtol parsen
    char* endptr;
    errno = 0;
    int n = strtol(argv[1], &endptr, 10);   //Obergrenze
    if ((errno != 0) || (endptr == argv[1]) || (*endptr != '\0')) {
        usage();
        exit(EXIT_FAILURE);
    }

    //process array
    struct process* processes = malloc(argc * sizeof(struct process*));
    if (processes == NULL) {
        die("malloc");
    }

    for (int i = 0; i < argc; i++) {
        // check if n is reached -> call waitProcess
        while (running_process == n) {
            waitProcess(processes, n);
        }
        // add process info in processes (array)
        processes[i].cmd = strdup(argv[i + 2]);
        if (processes[i].cmd == NULL) {
            die("strdup");
        }
        processes[i].pid = run(argv[i + 2]);
    }

    //collect the rest processes
    while (running_process > 0) {
        waitProcess(processes, n);
    }

    free(processes);

    exit(EXIT_SUCCESS);
}

void waitProcess(struct process *processes, size_t size) {
    int status;
    errno = 0;
    // Auf beliebigen Prozess warten
    
    pid_t wpid = waitpid(-1, &status, WNOHANG);
    if (wpid == 0) {
        return;
    }
    else if (wpid < 0) {
        //no more child
        if (errno == ECHILD) {
            return;
        }
        else die("waitpid");
    }
   
    if (WIFEXITED(status)) {
        // Befehlszeile raussuchen
        char* cmdline; 
        for (int i = 0; i < (int)size; i++) {
            if (processes[i].pid == wpid) {
                cmdline = strdup(processes[i].cmd);
                if (cmdline == NULL) {
                    die("strdup");
                }
            } 
        }

        //decrease running process
        running_process--;

        // Terminierungsgrund ausgeben
        if (printf("pid[%d], cmdline[%s], exitcode[%d]", (int)wpid, cmdline, WEXITSTATUS(status)) < 0) {
            die("printf");
        }

        if (fflush(stdout)) {
            die("fflush");
        }
    }
}

pid_t run(char* cmdline) {
    //create child process
    pid_t pid = fork();
    //error by fork
    if (pid < 0) {
        die("fork");
    }
    //child process
    if (pid == 0) {
        // Befehlszeile parsen
        int index = 0;
        char* cmd_array[(strlen(cmdline) + 1) / 2];
        cmd_array[index] = strtok(cmdline, " ");
        while ((cmd_array[index] = strtok(NULL, " ")) != NULL) {
            index++;
        }
        //exec
        execvp(cmd_array[0], cmd_array);
        fprintf(stderr, "Error in child process by exec!\n");
    }
    //parent process
    running_process++;
    return pid;
}

