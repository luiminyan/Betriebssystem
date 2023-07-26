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
};

//Makros, Funktionsdeklarationen, globale Variablen
static void parseFile(char* filename);
static void waitProcess(void);
static void run(char* bin, struct host* arg);

//Funktion main
int main(int argc, char* argv[]) {
    //Befehlszeilenargumente prüfen
    if (argc != 2) {
        usage();
    }

    //Datei parsen

    exit(EXIT_SUCCESS);
}

//Funktion parseFile
void parseFile(char* filename) {
    //Zeilenweises Auslesen der Datei
    char line[MAX_LINE + 2];
    while (fgets())

}