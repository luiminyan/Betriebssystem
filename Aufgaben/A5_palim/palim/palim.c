#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>		//struct dirent, info from readdir
#include <errno.h>

//add 
#include <sys/stat.h>	//info by lstat
#include <pthread.h>	//thread
#include "sem.h"		//semaphore
#include <string.h>		//strstr

struct statistics {
	int lines;				//Anzahl der durchsuchten Zeilen (lines)
	int lineHits;			//Anzahl der Zeilen, die Treffer enthalten (lineHits)
	int files;				//Anzahl der durchsuchten Dateien (files) 
	int fileHits;			//Anzahl der Dateien mit mindestens einem Treffer (fileHits)
	int dirs;				//Anzahl der durchsuchten Verzeichnisse (dirs)
	int activeGrepThreads;	//Anzahl der aktuell aktiven grep-Threads (activeGrepThreads)
	int maxGrepThreads;		//argv[2]
	int activeCrawlThreads;
};

// (module-)global variables
static struct statistics stats;		//global information

//add variables if necessary
#define MAX_LINE 4096		//max line length of a file
static char* search_string;		//the string to be searched

//add semaphores
static SEM* statsMutex;			//mutual exclusion, only one thread is allowed to modify the stats info
static SEM* grepThreadSem;		//limition
static SEM* newDataSignal;		//notification of new data

// function declarations
static void* processTree(void* path);
static void processDir(char* path);
static void processEntry(char* path, struct dirent* entry);
static void* processFile(void* path);

static void usage(void) {
	fprintf(stderr, "Usage: palim <string> <max-grep-threads> <trees...>\n");
	exit(EXIT_FAILURE);
}

static void die(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

// done: add declarations if necessary
//a helper function to print the stats information
static void print_stats(){
	//lock the stats
	P(statsMutex);		

	//copy the current stats info into variables
	int lines_copy = stats.lines;
	int lineHits_copy = stats.lineHits;
	int fileHits_copy = stats.fileHits;
	int files_copy = stats.files;
	int dirs_copy = stats.dirs;
	int activeGrepThreads_copy = stats.activeGrepThreads;

	//unlock the stats
	V(statsMutex);		

	//lineHits/lines lines, fileHits/files files, dirs directories, activeGrepThreads
	if (printf("%d/%d lines, %d/%d files, %d directories, %d activeGrepThreads", 
				lineHits_copy, lines_copy,
				fileHits_copy, files_copy,
				dirs_copy,
				activeGrepThreads_copy) 
		< 0) {
			die("printf");
	}
	//flush stdout
	if (fflush(stdout) == EOF) {
		die("fflush");
	}
}

//the operations on stats(global info) should be protected by statsMutex
static void incStats(int* attr){
	P(statsMutex);
	*attr += 1;
	V(statsMutex);
}

static void decStats(int* attr){
	P(statsMutex);
	*attr -= 1;
	V(statsMutex);
}


/**
 * \brief Initializes necessary data structures and spawns one crawl-Thread per tree.
 * Subsequently, waits passively on statistic updates and on update prints the new statistics.
 * If all threads terminated, it frees all allocated resources and exits/returns.
 */

int main(int argc, char** argv) {
	//input: palim <string> <max-grep-threads> <trees...>
	if(argc < 4) {			
		//not enought input
		usage();
	}

	// convert argv[2] (<max-grep-threads>) into long with strtol()
	errno = 0;
	char *endptr;
	stats.maxGrepThreads = strtol(argv[2], &endptr, 10);

	// argv[2] can not be converted into long without error
	if(errno != 0 || endptr == argv[2] || *endptr != '\0') {
		usage();
	}

	if(stats.maxGrepThreads <= 0) {
		fprintf(stderr, "max-grep-threads must not be negative or zero\n");
		usage();
	}

	// done: implement me!
	//store the string from user input into search_string
	search_string = argv[1];

	//initialize the semaphores / mutex
	statsMutex = semCreate(1);
	//error by semCreate
	if (statsMutex == NULL) {
		die("semCreate");
	}
	grepThreadSem = semCreate(stats.maxGrepThreads);
	//error by semCreate
	if (grepThreadSem == NULL) {
		die("semCreate");
	}

	//for passive waiting of the main thread, blocked till a execution (data update a thread (grep or craw)) wakes it up
	newDataSignal = semCreate(0);		
	//error by semCreate
	if (newDataSignal == NULL) {
		die("semCreate");
	}

	//create crawThreads for the tree
	pthread_t tids[argc - 3]; 	//threads list
	//clear errno
	errno = 0;
	for (int i = 3; i < argc; i++) {
		//create a craw thread with function processTree
		errno = pthread_create(&tids[i - 3], NULL, processTree, (void*) argv[i]);
		//error by pthread_create
		if (errno) {
			die("pthread_create");
		}

		//increase 1 activeCrawThreads, as a craw thread is just created
		incStats(&stats.activeCrawlThreads);

		//set detach (automatic self collection) of the threads
		errno = pthread_detach(tids[i - 3]);
		if (errno) {
			die("pthread_detach");
		}
	}

	//done: passive waiting of main
	while (1) {
		//main thread stucks here till a thread update the info
		P(newDataSignal);
		//code from here can be regarded as critical section, need a V(newDataSignal) to wake up this part, as sem is set to 0
		P(statsMutex);	//protect stats, as we need to read stats info
		int active_grep = stats.activeGrepThreads;
		int active_craw = stats.activeCrawlThreads;
		V(statsMutex);
		//no more active threads
		if ((active_grep <= 0) && (active_craw <= 0)) {
			break;
		}
		// print statistics
		print_stats();	
	}

	print_stats();
	if (printf("\n") < 0) {
		die("printf");
	}
	//flush after print
	if (fflush(stdout) == EOF) {
		die("fflush");
	}

	//kill the semaphores
	semDestroy(statsMutex);
	semDestroy(grepThreadSem);
	semDestroy(newDataSignal);

	return EXIT_SUCCESS;
}


/**
 * \brief Acts as start_routine for crawl-Threads and calls processDir().
 *
 * It updates the stats.activeCrawlThreads field.
 *
 * \param path Path to the directory to process
 *
 * \return Always returns NULL
 */
static void* processTree(void* path) {
	//done: implement me!
	//casting for param in processDir
	char* dir_path = (char*) path;

	//call processDir
	processDir(dir_path);

	//thread about to die: decrease 1 activeCrawlThread
	decStats(&stats.activeCrawlThreads);

	//update in main
	V(newDataSignal);

	return NULL;
}

/**
 * \brief Iterates over all directory entries of path and calls processEntry()
 * on each entry (except "." and "..").
 *
 * It updates the stats.dirs field.
 *
 * \param path Path to directory to process
 */

static void processDir(char* path) {
	// done: implement me!
	//create dir pointer (the iterator to read over the directory)
	DIR* dir_ptr = NULL;

	//open the current directory
	dir_ptr = opendir(path);
	//error by opendir
	if (dir_ptr == NULL) {
		die("opendir");
	}

	//increase stats.dirs
	incStats(&stats.dirs);
	//update the stats info
	V(newDataSignal);

	//directory info for readdir
	struct dirent* entry = NULL; 	

	//read directory
	while (1) {
		//reset errno
		errno = 0;

		//readdir
		entry = readdir(dir_ptr);
		if (entry == NULL) {
			//case 1: error
			if (errno) {
				die("readdir");
			}
			//case 2: EOF
			else break;
		}

		//if found element is not .(self) or ..(root), call processEntry
		if ((strcmp(entry->d_name, ".") != 0) && strcmp(entry->d_name, "..") != 0) {
			//call processEntry
			processEntry(path, entry);
		}
	}

	//closedir
	if (closedir(dir_ptr) == -1) {
		die("closedir");
	}
}

/**
 * \brief Spawns a new grep-Thread if the entry is a regular file and calls processDir() if the entry
 * is a directory.
 *
 * It updates the stats.activeGrepThreads if necessary. If the maximum number of active grep-Threads is
 * reached the functions waits passively until another grep-Threads can be spawned.
 *
 * \param path Path to the directory of the entry
 * \param entry Pointer to struct dirent as returned by readdir() of the entry
 */
static void processEntry(char* path, struct dirent* entry) {
	//done: implement me!
	//create stat buff to store the found item info by lstat
	struct stat buff;	

	//adjust the addess in lstat
	//length = len (/path + / + entry->d_name + \0)
	int pathAndEntryLength = strlen(path) + strlen(entry->d_name) + 2;		
	//create buffer for the address (pathAndEntry)
	char* pathAndEntry = malloc(sizeof(char*) * pathAndEntryLength);
	if (pathAndEntry == NULL) {
		die("malloc");
	}

	//pathAndEntry =  /path + / + entry->d_name + \0 (automatic)
	strcat(pathAndEntry, path);
	strcat(pathAndEntry, "/");
	strcat(pathAndEntry, entry->d_name);

	//lstat
	if (lstat(pathAndEntry, &buff) == -1) {
		//if lstat dies
		//free the allocation
		free(pathAndEntry);
		die("lstat");
	}

	//judge the type of the found element
	//case 1: if element is directory
	if (S_ISDIR(buff.st_mode)) {
		processDir(pathAndEntry);
	}
	
	//case 2: if the found element is a regular file
	else if (S_ISREG(buff.st_mode)) {
		//create a new grep-Thread
		//grep-Thread creation is controlled by grepThreadSem
		P(grepThreadSem);
		//reset errno
		errno = 0;
		//create thread
		pthread_t tid_grep;
		errno = pthread_create(&tid_grep, NULL, processFile, pathAndEntry);
		if (errno) {
			die("pthread_create");
		}

		//make thread detach
		errno = pthread_detach(tid_grep);
		if (errno) {
			die("pthread_detach");
		}

		//increase the number of activeGrepThreads
		incStats(&stats.activeGrepThreads);

		//update the stats info
		V(newDataSignal);
	}
}

/**
 * \brief Acts as start_routine for grep-Threads and searches all lines of the file for the
 * search pattern.
 *
 * It updates the stats.files, stats.lines, stats.fileHits, stats.lineHits
 * stats.activeGrepThreads fields if necessary.
 *
 * \param path Path to the file to process
 *
 * \return Always returns NULL
 */
static void* processFile(void* path) {
	//open the FILE!!!
	//casting to char pointer to use in opendir
	char* file_path = (char*) path;
	//fopen
	FILE* file_entry = fopen(file_path, "r");
	//error by opendir
	if (file_entry == NULL) {
		die("fopen");
	}
	//increase fileHits
	incStats(&stats.files);
	//update stats info
	V(newDataSignal);

	//create char buffer for a single line, static as the size is given
	char line[MAX_LINE + 2];	//MAX_LINE + \n + \0
	
	//read the file while no error / not end of the file
	while (1) {
		if (fgets(line, MAX_LINE + 2, file_entry) != NULL) {
			//increase line
			incStats(&stats.lines);
			//check if search_string is inside of the line
			if (strstr(line, search_string)) {
				//found the string in line, increase lineHits and fileHits
				incStats(&stats.lineHits);
				incStats(&stats.fileHits);
			}			
		}
		else {
			//case 1: end of file
			if (feof(file_entry)) {
				break;
			}
			//case 2: error
			else die("fgets");
		}
		//update the stats info in main
		V(newDataSignal);
	}

	//closedir
	if (fclose(file_entry) == EOF){
		die("fclose");
	}

	//the grepThread dies
	//free one piece of grepThreadSem 
	V(grepThreadSem);

	//decrease active grep thread number
	decStats(&stats.activeGrepThreads);	

	//update with stats info
	V(newDataSignal);
	return NULL;
}

