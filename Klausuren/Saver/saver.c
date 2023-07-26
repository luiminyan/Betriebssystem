#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 4096

static void die(const char msg[]) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void usage(void) {
    fprintf(stderr, "Usage:_saver_<hostname_file>\n");
    exit(EXIT_FAILURE);
}

struct host {
    pid_t pid;  //pid des bearbeitenden Prozesses

    //eigene Mitglieder
    char* rechnername;
    int status;         //wait status
    struct host* next;  //ptr to the next element
    bool is_active;     //default false (inactive)
};

//Makros, Funktionsdeklarationen, globale Variablen
static void parseFile(char* filename);
static void waitProcess(void);
static void run(char* bin, struct host* arg);

static int proc_counter = 0;

static struct host* head = NULL;
#include <errno.h>

//Funktion main
int main(int argc, char* argv[]) {
    //Befehlszeilenargumente prüfen
    if (argc != 2) {
        usage();
    }

    //Datei parsen
    char* filename = argv[1];
    parseFile(filename);    //new host on list head

    //Rechner auf Inaktivität prüfen
    struct host* elem = head;
    while (elem != NULL) {
        run("check_idle ", elem);
        elem = elem->next;
    }
    
    //Inaktive Rechner herunterfahren
    struct host* new_elem = head;
    while (new_elem != NULL) {
        if (new_elem->is_active) {
            //when active -> call shutdown_remote
            run("shutdown_remote", new_elem);
        }
        new_elem = new_elem->next;
    }

    //Aufräumen und beenden
    //collect zombies
    for (int i = 0; i < proc_counter; i++) {
        waitProcess();
    }

    //free allocation
    struct host* very_new_elem = head;
    while (very_new_elem != NULL) {
        struct host* to_free = very_new_elem;
        very_new_elem = very_new_elem->next;
        free(to_free);
    }

    exit(EXIT_SUCCESS);
}
//Ende Funktion main

//Funktion parseFile
void parseFile(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        die("fopen");
    }
    //Zeilenweises Auslesen der Datei
    char line[MAX_LINE + 2];
    while (fgets(line, MAX_LINE + 2, file) != NULL) {
        //empty line
        if (strlen(line) < 2) {
            continue;
        }
        //line is too long
        if ((strlen(line) == MAX_LINE + 1) && (line[MAX_LINE] != '\n')) {
            int c;
            //skip the rest of the line
            do {
                c = getchar();
            } while ((c != '\n') && (c != EOF));
            continue;
        }
        //remove '\n'
        line[strlen(line) - 1] = '\0';

        //create struct host and add into list
        struct host* new = malloc(sizeof(struct host*));
        if (new == NULL) {
            die("malloc");
        }

        new->rechnername = strdup(line);
        if (new->rechnername == NULL) {
            die("strdup");
        }

        //add into list head
        new->next = head;
        head = new;
    }

    //Fehlerbehandlung + Aufräumen
    if (ferror(stdin)) {
        die("fgets or getchar");
    }
    //fclose
    if (fclose(file) == EOF) {
        die("fclose");
    }
}

void waitProcess(void) {
    int status;
    errno = 0;
    pid_t wpid = waitpid(-1, &status, WNOHANG);
    if (wpid == 0) {
        return;
    }
    else if (wpid < 0) {
        if (errno == ECHILD) {
            return;
        }
        else die("waitpid");
    }
    //look for pid in host list
    struct host* elem;
    while (elem->pid != wpid) {
        elem = elem->next;
    }

    if (WIFEXITED(status)) {
        //store the exit status
        elem->status = WEXITSTATUS(status);
    }
}

void run(char* bin, struct host* arg) {
    pid_t pid = fork();
    if (pid < 0) {
        die("fork");
    }

    //child process
    else if (pid == 0) {
        execlp(bin, bin, arg->rechnername, NULL);
        if (strcmp("check_idle", bin) == 0) {
            arg->is_active = true;  //do not need to be shut down
        }
        die("execlp");
    }

    //parent process
    arg->pid = pid;  
    proc_counter++;
}