#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "sem.h"
#include "list.h"

static void usage(void);
static void die(const char *msg);
static void err(const char *msg);
static void *search(void *arg);

static SEM *sem_mutex;
static SEM *sem_found;

struct pthread_args {
	char *file;
};

static char *search_pattern;
static int active_threads;

#define LINE_MAX (4094 + 1 + 1)

int main(int argc, char *argv[]) {
	if(argc < 3) {
		usage();
	}

	// Semaphore als Mutex für den Zugriff auf die Liste und active_threads
	sem_mutex = semCreate(1);
	if(sem_mutex == NULL) {
		err("semCreate() failed");
	}

	// Semaphore als Signal an den main-thread, wenn etwas gefunden wurde
	sem_found = semCreate(0);
	if(sem_found == NULL) {
		err("semCreate() failed");
	}

	search_pattern = argv[1];

	// Die Anzahl an Threads die wir erstellen wollen ist die Anzahl an Dateien
	active_threads = argc-2;

	// Jeder Thread erhält einen Zeiger auf eine pthread_args-struct,
	// um ihm den Dateinamen mitzuteilen.
	struct pthread_args args[active_threads];

	// threads erstellen
	for(int i = 2; i < argc; i++) {
		args[i-2].file = argv[i];

		pthread_t tid;
		errno = pthread_create(&tid, NULL, &search, &args[i-2]);
		if(errno != 0) {
			die("pthread_create");
		}
	}

	char *line;
	do {
		P(sem_found); // auf ein Signal wartem
		P(sem_mutex); // Mutex für die Liste "öffnen"
		line = removeElement(); // das Element aus der Liste lesen
		V(sem_mutex); // Mutex für die Liste "schließen"

		// Falls tatsächlich etwas gefunden wurde wird es ausgegeben
		if(line != NULL) {
			printf("%s", line);
			free(line);
		}
	} while(line != NULL); // die schleife wird beendet, sobald wir ein Signal erhalten, ohne dass eine Zeile hinzugefügt wurde

	printf("done\n");

	// Semaphore freigeben
	semDestroy(sem_found);
	semDestroy(sem_mutex);
}

static void *search(void *a) {
	// Da der Thread nie gejoint wird, sollen die Resourcen direkt freigegeben werden
	errno = pthread_detach(pthread_self());
	if(errno != 0) {
		die("pthread_detach()");
	}

	// Als Argumente haben wir den Dateipfad erhalten:
	struct pthread_args *arg = (struct pthread_args *) a;
	char *path = arg->file;

	FILE *file = fopen(path, "r");
	if(file == NULL) {
		die("fopen");
	}

	// Zeilenweise nach dem Muster suchen
	char buf[LINE_MAX];
	while(fgets(buf, sizeof(buf), file) != NULL) {
		if(strstr(buf, search_pattern) != NULL) {
			// Wir haben es gefunden; hänge es in die Liste ein und Signalisiere den Hauptthread
			P(sem_mutex);
			if(-1 == insertElement(buf)) {
				err("insertElement() failed");
			}
			V(sem_mutex);
			V(sem_found); // Hauptthread signalisieren
		}
	}

	if(ferror(file)) {
		die("fgets");
	}

	if(fclose(file) != 0) {
		die("fclose");
	}

	P(sem_mutex);
	active_threads--;
	if(active_threads == 0) {
		// sobald keine threads mehr aktiv sind, signalisiere dem Hauptthread,
		// dass er sich beenden soll:
		V(sem_found);
	}
	V(sem_mutex);

	return NULL;
}

static void usage(void) {
	fprintf(stderr, "Usage: synchro <search-pattern> <file> [<file> ...]\n");
	exit(EXIT_FAILURE);
}

static void die(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

static void err(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}
