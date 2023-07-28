#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdbool.h> 
#include <dirent.h> 
#include <pthread.h>
#include "list.h" 
#include "sem.h"

static void die(const char msg[]) { 
    perror(msg);
    exit(EXIT_FAILURE);
}

static void err(const char msg[]) { 
    fprintf(stderr, "%s\n", msg); 
    exit(EXIT_FAILURE);
}

static void usage(void) {
    err("Usage: witch [--hunt] <search>");
}


typedef struct {
    char *search; // Suchstring
    char *dir; // zu durchsuchendes Verzeichnis
    bool hunt; // Befehlszeilenargument --hunt übergeben
} search_t;     //the shorten name for struct def is search_t

// Funktions- & Strukturdekl., globale Variablen, etc.
//counter for the found files
static int file_found = 0;
//head for the file list
search_t head = NULL;

//warn: to print error msg in stderr, without exiting the func
static void warn(char* msg) {
    fprintf(stderr, msg);   
}
// Funktion main()
int main(int argc, char* argv[]) {
    // Deklarationen etc.

    // Befehlszeilenargumente prüfen
    // witch [--hunt] <search>
    if (argc < 2 || argc > 3) {
        usage();
    }




}


// Initialisierungen und PATH auslesen
// Verzeichnisse aus PATH extrahieren
// Such-Threads starten und auf Beendigung warten
// Ausgabe der gefundenen Dateien
// Aufräumen und Beenden
// Ende Funktion main()
// Verzeichnisse aus PATH extrahieren
// Such-Threads starten und auf Beendigung warten


// Funktion thread_start()
// Ende Funktion thread_start()

// Funktion search_dir()
// Verzeichnis öffnen
// Über Verzeichniseinträge iterieren
// Aufräumen
// Ende Funktion search_dir()