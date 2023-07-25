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
#define CMD_MAX 100     //max line / cmd size

static void die(const char *msg) { 
    perror(msg);
    exit(EXIT_FAILURE);
}

static void usage(void) { 
    fprintf(stderr, "Usage: hupsi <n>\n"); 
    exit(EXIT_FAILURE);
}

struct process {
    pid_t  pid; // PID des Prozesses

    // Eigene Mitglieder
    struct process* next;
    char* cmdline;
    time_t start_time;
    time_t end_time;
};

// Makros, Funktionsdeklarationen, globale Variablen
static void waitProcess(void);
static pid_t run(char* cmdline);

//linked list for the processes
static struct process* head;   //head of the list
//running programs counter
static int prog_counter = 0;


// Funktion main
int main(int argc, char* argv[] ) {
    // Befehlszeilenargument(e) prüfen
    if (argc != 2) {
        usage();
    }

    // Befehlszeilenarg. <n> mit strtol parsen
    // reset errno
    errno = 0;
    char* endptr;
    long n = strtol(argv[1], &endptr, 10);
    // error by strtol
    if ((errno != 0) || (endptr == argv[1]) || (*endptr != '\0')) {
        usage();
    }

    // Befehle von stdin einlesen und verarbeiten
    // create buffer for line
    char line[CMD_MAX + 2];    //+2 for \0 and \n
    // read from stdin
    while (fgets(line, CMD_MAX, stdin) != NULL) {
        //n is reached
        while (prog_counter == n) {
            //collect zombie
            waitProcess();
        }

        //DONE: create new process element
        struct process* new = malloc(sizeof(struct process));
        //error by malloc
        if (new == NULL) {
            die("malloc");
        }

        //set the start time stamp
        new->start_time = time(NULL);

        //set the cmd 
        new->cmdline = strdup(line);
        if (new->cmdline == NULL) {
            die("strdup");
        }

        //run
        new->pid = run(line);

        //insert new into list (on the list head)
        new->next = head;
        head = new;

        //increase prog_counter
        prog_counter++;
    }

    //DONE: collect rest zombies
    if (prog_counter > 0) {
        waitProcess();
    }

    //error by fgets
    if (ferror(stdin)) {
        die("fgets");
    }

    exit(EXIT_SUCCESS);
}

// Funktion waitProcess
void waitProcess(void) {
    // Auf beliebigen Prozess warten
    int status;
    pid_t wpid = waitpid(-1, &status, WNOHANG);
    if (wpid == -1) {
        if (errno == ECHILD) {
            //no more to be collected child
            return ;
        }
        //error
        else die("waitpid");
    }
    else if (wpid == 0) {
        //child still not exited
        return ;
    }

    // if child collected
    // struct process zu PID aus Prozessliste ermitteln
    struct process* elem = head;
    while(elem->pid != wpid) {
        elem = elem->next;
    }
    //if child process exited
    if (WIFEXITED(status)) {
        // (Vorbereiten der) Ausgabe
        if (printf("pid: %d, cmd: %s, exitcode: %d, lifetime: %ld\n", wpid, elem->cmdline, WEXITSTATUS(status), (elem->end_time - elem->start_time))) {
            die("printf");
        }
        //flush the stdout
        if (fflush(stdout) == EOF) {
            die("fflush");
        }
    }
   
    // Prozess aus Liste entfernen & Speicher freigeben
    free(elem);

    //reduce number of prog_counter
    prog_counter--;
}

pid_t run(char *cmdline) {
    //create child process
    pid_t pid = fork();
    //error by fork
    if (pid == -1) {
        die("fork");
    }
    //child process
    else if (pid == 0) {
        //seperate cmd line
        int index = 0;
        //create cmd_array
        char* cmd_array[CMD_MAX / 2];
        //extract the first cmd
        cmd_array[index] = strtok(cmdline, " ");
        index++;
        //extract the rest
        while (strtok(NULL, " ") != NULL) {
            index++;
        }

        //run the cmd
        execvp(cmd_array[0], cmd_array);
        //error by execvp
        die("execvp");
    }

    //parent process, return the child process
    return pid;
}

