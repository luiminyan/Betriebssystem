#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMD_MAX 100     //max command line size

static void die(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void usage() {
    fprintf(stderr, "Usage:_hupsi_<n>\n");
    exit(EXIT_FAILURE);
}

struct process {
    pid_t pid;  //PID des Prozesses

    //eigene Mitglieder
    struct process* next;
    char* cmdline;
    time_t start_time;
    time_t end_time;
};

//Makros, Funtionsdeklarationenen, globale Variable
static pid_t run(char *cmdline);
static void waitProcess(void);
//create head of linked list
static struct process* head;
static long p_num = 0;


//Funktion main
int main(int argc, char* argv[]) {
    // Befehlszeilenargument(e) prüfen
    if (argc != 2) {
        usage();
    }
    // Befehlszeilenarg. <n> mit strtol parsen
    errno = 0;
    char* endptr;
    long n = strtol(argv[1], &endptr, 10);
    if ((errno != 0) || (endptr == argv[1]) || (*endptr != '\0')) {
        usage();
    }
     
    // Befehle von stdin einlesen und verarbeiten
    char line[CMD_MAX + 2];
    while (fgets(line, CMD_MAX + 2, stdin) != NULL) {
        //check if n is reached
        while (p_num == n) {
            //remove a zombie program
            waitProcess();
        }

        //!ATTENTION: create struct process
        struct process* new = malloc(sizeof(struct process));
        if (new == NULL) {
            die("malloc");
        }

        new->cmdline = strdup(argv[1]);
        if (new->cmdline == NULL) {
            die("strdup");
        }
        new->pid = run(line);
        new->start_time = time(NULL);

        //insert into list
        new->next = head;
        head = new;

        p_num++;
    }
    if (ferror(stdin)) {
        die("fgets");
    }

    if (fflush(stdin) == EOF) {
        die("fflush");
    }

    //collect rest zombies
    if (p_num > 0) {
        waitProcess();
    }

    exit(EXIT_SUCCESS);
}
// Ende Funktion main

// Funktion run
pid_t run(char* cmdline){
    //create child process
    pid_t pid = fork();
    if (pid < 0) {
        die("fork");
    }
    //child process
    else if (pid == 0) {
        //extract the cmds from cmdline
        char* cmds_arr[(CMD_MAX + 1) / 2];
        int index = 0;
        cmds_arr[index] = strtok(cmdline, " ");
        index++;
        while((cmds_arr[index] = strtok(NULL, " ")) != NULL) {
            index++;
        }

        //exec
        execvp(cmds_arr[0], cmds_arr);
        die("execvp");   
    }
    //parent process
    return pid;
}
// Ende Funktion run

// Funktion waitProcess
void waitProcess() {
    // Auf beliebigen Prozess warten
    int status;
    //reset errno
    errno = 0;
    pid_t wpid = waitpid(-1, &status, WNOHANG);
    if (wpid == -1) {
        if (errno == ECHILD) {
            return ;
        }
        else die("waitpid");
    }
    //the child is not yet terminated
    else if (wpid == 0) {
        return ;
    }

    // struct process zu PID aus Prozessliste ermitteln
    struct process* elem = head;
    while (elem->pid != wpid) {
        elem = elem->next;
    }

    //process successfully exited
    if (WIFEXITED(status)) {
        // (Vorbereiten der) Ausgabe
        //record the end_time
        elem->end_time = time(NULL);
        if (printf("pid: %d, cmdline: %s, exitcode: %d, life duration: %ld\n", (int) wpid, elem->cmdline, WEXITSTATUS(status), (long)(elem->end_time - elem->start_time)) < 0) {
            die("printf");
        }
    }

    // Prozess aus Liste entfernen & Speicher freigeben
    //find the process before elem
    struct process* before_elem = head;
    while (before_elem->next != elem) {
        before_elem = before_elem->next;
    }
    //remove from list: connect before_elem with elem->next
    before_elem->next = elem->next;
    //free allocation of elem
    free(elem);
    
    //increase p_num
    p_num--;
}
// Ende Funktion waitProcess

