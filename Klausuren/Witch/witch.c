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
// declaire functions
static void* thread_start(void* arg);
static void search_dir(char* search, char* dir, bool hunt);
//error in file system
static void warn(char* msg) {
    fprintf(stderr, "%s\n", msg );
}
static int found_files = 0;
// head for the file list
static search_t* head = NULL;

// declaire the semaphores
static SEM* listMutex;

//warn: to print error msg in stderr, without exiting the func
static void warn(char* msg) {
    fprintf(stderr, msg);   
}

// Funktion main()
int main(int argc, char* argv[]) {
    // Deklarationen etc.
    // initialize the semaphores
    listMutex = semCreate(1);   //only one thread is allowed to modify the list at the same time


    // Befehlszeilenargumente prüfen
    if (argc == 2) {
        //./witch <search>
        head->search = strdup(argv[1]);
        head->hunt = false;
    }

    else if ((argc == 3) && (strcmp(argv[1], "--hunt") == 0)) {
        // ./witch [--hunt] <search>
        head->search = strdup(argv[2]);
        head->hunt = true;
    }

    else {
        usage();
    }

    // Initialisierungen und PATH auslesen
    char* PATH = getenv(head->search);
    if (PATH == NULL) {
        die("getenv");
    }

    // Verzeichnisse aus PATH extrahieren
    int index = 0;
    char* dir_arr[strlen(PATH)];
    dir_arr[index] = strtok(PATH, ":");
    index++;
    while ((dir_arr[index] = strtok(NULL, ":")) != NULL) {
        index++;
    }
   
    // Such-Threads starten und auf Beendigung warten
    pthread_t pids[index];
    for (int i = 0; i < index; i++) {
        errno = 0;
        errno = pthread_create(&pids[i], NULL, thread_start, dir_arr[i]);
        if (errno != 0) {
            die("pthread_create");
        }
    }

    // pthread_join
    for (int j = 0; j < index; j++) {
        errno = 0;
        errno = pthread_join(pids[j], NULL);
        if (errno != 0) {
            die("pthread_join");
        }
    }

    // Ausgabe der gefundenen Datei
    char* elem = NULL;
    while ((elem = removeElement()) != NULL) {
        if (printf("%s\n", elem) < 0) {
            die("printf");
        }
    }

    //Aufräumen und beenden
    if (found_files > 0) {
        exit(EXIT_SUCCESS);
    }
    else err("No file with given name found!");
}


// Funktion thread_start()
void* thread_start(void* arg) {
    // cast the arg into char*
    char* dir_path = (char*) arg;
    // call search_dir
    search_dir(head->search, dir_path, head->hunt);
    // thread is done
    return NULL;
}
// Ende Funktion thread_start()

// Funktion search_dir()
void search_dir(char* search, char* dir, bool hunt) {
    // Verzeichnis offnen
    DIR* directory = opendir(dir);
    if (directory == NULL) {
        warn("opendir");
        return;
    }

    // Über Verzeichniseinträge iterieren
    struct dirent* entry = NULL;
    struct stat statistics;
    errno = 0;
    entry = readdir(directory);

    while (entry != NULL) {
        // self / root dir: skip
        if (entry->d_name == "." || entry->d_name == "..") {
            continue;
        }
        // create new_pfad = dir / entry->name
        char new_path[strlen(dir) + 2];
        strcat(new_path, dir);
        strcat(new_path, "/");
        strcat(new_path, entry->d_name);

        // read statistics
        if (lstat(dir, &statistics) == -1) {
                warn("lstat");
                continue;;
        }

        // entry: regular file
        if (S_ISREG(statistics.st_mode)) {
            if (strcmp(entry->d_name, search) == 0) {
                P(listMutex);
                // increase found file number
                found_files++;
                if (insertElement(new_path) == -1) {
                    die("insertElement");
                }
                V(listMutex);

                // if hunt
                if (head->hunt == true) {
                    if (unlink(new_path) == -1) {
                        warn("unlink");
                    }
                }
            }
        }
        // entry: directory
        else if (S_ISDIR(statistics.st_mode)) {
            search_dir(head->search, new_path, head->hunt);
        }

        // reset errno
        errno = 0;

        // read next element
        entry = readdir(directory);
    }

    if (errno != 0) {
        warn("readdir");
        return;
    }

    // Aufräumen
    // closedir
    if (closedir(directory) == -1) {
        warn("closedir");
    }
}
