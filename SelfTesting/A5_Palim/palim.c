#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#include <pthread.h>	//pthreads operations
#include "sem.h"		//semaphores
#include <sys/stat.h>
#include <string.h>	

struct statistics {
	int lines;
	int lineHits;
	int files;
	int fileHits;
	int dirs;
	int activeGrepThreads;
	int maxGrepThreads;
	int activeCrawlThreads;
};

// (module-)global variables
static struct statistics stats;
// TODO: add variables if necessary
#define MAX_LINE 4096	//'\n' and '\0' not included


// function declarations
static void* processTree(void* path);
static void processDir(char* path);
static void processEntry(char* path, struct dirent* entry);
static void* processFile(void* path);
// TODO: add declarations if necessary
//declair semaphores
SEM* statsMutex;		//stats as critical section 
SEM* maxGrepSem;		//control max grep threads number
SEM* newDataSignal;		//main thread passive waiting till new signal arrives

//declair searchWord
static char* searchWord;


static void usage(void) {
	fprintf(stderr, "Usage: palim <string> <max-grep-threads> <trees...>\n");
	exit(EXIT_FAILURE);
}

static void die(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

//increase a stats info
static void incStats(int* statsInfo) {
	//enter critical section
	P(statsMutex);
	//increase the stats info by using ptr
	*statsInfo += 1;
	//exit critical section
	V(statsMutex);

}

//decrease a stats info
static void decStats(int* statsInfo) {
	//enter critical section
	P(statsMutex);
	//increase the stats info by using ptr
	*statsInfo -= 1;
	//exit critical section
	V(statsMutex);
}

//declaire print_stats
static void print_stats() {
	//enter critical section / stats
	P(statsMutex);
	//do the copy, as we do not want to use printf in critical section
	int linesCopy = stats.lines;
	int lineHitsCopy = stats.lineHits;
	int filesCopy = stats.files;
	int fileHitsCopy = stats.fileHits;
	int dirsCopy = stats.dirs;
	int activeGrepThreadsCopy = stats.activeGrepThreads;
	//exit the critical section
	V(statsMutex);

	//printf, format: lineHits/lines lines, fileHits/files files, dirs directories, activeGrepThreads active threads
	printf("%d/%d lines, %d/%d files, %d directories, %d active threads\n", lineHitsCopy, linesCopy, fileHitsCopy, filesCopy, dirsCopy, activeGrepThreadsCopy);
	//error by printf
	if (ferror(stdout)) {
		die("printf");
	}

	//fflush: error, returns EOF
	if (fflush(stdout) == EOF) {
		die("fflush");
	}
}

/**
 * \brief Initializes necessary data structures and spawns one crawl-Thread per tree.
 * Subsequently, waits passively on statistic updates and on update prints the new statistics.
 * If all threads terminated, it frees all allocated resources and exits/returns.
 */

int main(int argc, char** argv) {
	if(argc < 4) {
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
	/* palim 	<string> 	<max-grep-threads> 	<trees...>
	   argv[0]  argv[1] 	argv[2] 			argv[3] ... argv[argc - 1]*/
	//implement searchWord
	searchWord = argv[1];

	//implements the semaphores
	statsMutex = semCreate(1);		//only one thread is allowed to enter and to modify the critical section at the same time
	//error by semCreate, NULL is returned
	if (statsMutex == NULL) {
		die("semCreate");
	}

	//passive waiting for new data signal
	newDataSignal = semCreate(0);	
	if (newDataSignal == NULL) {
		die("semCreate");
	}

	//control grep threads number below max-grep-thread number
	maxGrepSem = semCreate(stats.maxGrepThreads);	
	if (maxGrepSem == NULL) {
		die("semCreate");
	}

	//create the pthread list for the crawl-thread
	pthread_t crawl_list[argc - 3];

	//reset errno
	errno = 0;

	//for all the trees, create crawl-threads (func: processTree, param: argv[i])
	for (int i = 3; i < argc; i++) {
		//create crawl-threads
		//!!!! ATTENTION: 
		errno = pthread_create(&crawl_list[i - 3], NULL, processTree, argv[i]);
		//error by thread_create
		if (errno != 0) {
			die("pthread_create");
		}

		//increase active-crawl-threads in stats
		incStats(&stats.activeCrawlThreads);

		//set thread into detach mode (automatic collection) 
		errno = pthread_detach(crawl_list[i - 3]);
		if (errno != 0) {
			die("pthread_detach");
		}
	}

	//wait passivly for data signal
	while (1) {
		//wait for other threads to call V(newDataSignal) to end this waiting
		P(newDataSignal);	
		//enter critical section
		P(statsMutex);	
		//copy the stats-threads information
		int activeGrepThreadsCopy = stats.activeGrepThreads;
		int activeCrawlThreadsCopy = stats.activeCrawlThreads;
		//exit the critical section
		V(statsMutex);

		/* !!!
			if no more active threads, break the passive waiting loop, 
			all searching is finished, end of the program 
			!!! */
		if ((activeCrawlThreadsCopy == 0) && (activeGrepThreadsCopy == 0)) {
			//print the stats
			print_stats();
			break;		//break the loop of waiting
		}
	}

	//print stats
	print_stats();

	//destroy semaphores
	semDestroy(statsMutex);
	semDestroy(newDataSignal);
	semDestroy(maxGrepSem);

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
	//casting
	char* pathCopy = (char*) path;
	//call processDir
	processDir(pathCopy);

	//end of this grep-thread, decrease the activeCrawlThreads
	decStats(&stats.activeCrawlThreads);
	//update the stats in main thread
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
	//create DIR* buffer to open the path
	DIR* directory = opendir(path);
	//error by opendir: NULL is returned
	if (directory == NULL) {
		die("opendir");
	}

	//increase dir in stats
	incStats(&stats.dirs);

	//create dirent* entry
	struct dirent* entry = NULL;

	//reset errno
	errno = 0;
	entry = readdir(directory);
	//use entry to iterate over directory
	while (entry != NULL) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
			//calls process directory
			processEntry(path, entry);
		}

		//read next item
		entry = readdir(directory);
	}

	//entry = NULL: 1) error by readdir (errno !=0) 2) end of directory
	if (errno != 0) {
		//real error
		die("readdir");
	}

	//!!!ATTENTION:
	//close directory
	if (closedir(directory) == -1) {
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
	//create stats buffer to read file type
	struct stat entryStat;

	//modify the path
	int newPathLength = strlen(path) + strlen(entry->d_name) + 2;	//./entry_name '\0'
	//allocation for newPath
	char* newPath = malloc(sizeof(char*) * newPathLength); 
	//error by malloc
	if (newPath == NULL) {
		die("malloc");
	}

	strcat(newPath, path);
	strcat(newPath, "/");
	strcat(newPath, entry->d_name);
	//automatic: + \0

	//error by lstat: return -1
	if (lstat(newPath, &entryStat) == -1) {
		die("lstat");
	}

	//if entry is directory
	if (S_ISDIR(entryStat.st_mode)) {
		//call processEntry
		processDir(newPath);
	}

	else if (S_ISREG(entryStat.st_mode)) {
		//controlled by maxGrepSem
		P(maxGrepSem);

		//reset errno
		errno = 0;

		//create a grep thread (func: processFile, argu: path)
		pthread_t grepPid;
		errno = pthread_create(&grepPid, NULL, processFile, path);
		//error by pthread_create
		if (errno != 0) {
			die("pthread_create");
		}

		//increase activeGrepThread in stats
		incStats(&stats.activeGrepThreads);
		//update info in main thread
		V(newDataSignal);

		//put grep thread on detach
		errno = pthread_detach(grepPid);
		//error by pthread_detach
		if (errno != 0) {
			die("pthread_detach");
		}
	}

	//free allocation, as all called functions returned
	free(newPath);
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
	//casting 
	char* filePath = (char*) path;

	//fopen
	FILE* file = fopen(filePath, "r");
	//error by fopen
	if (file == NULL) {
		die("fopen");
	}

	//increase file number in stats
	incStats(&stats.files);

	//check if search word is found
	int stringFound = 0;

	//create line buffer to read file line by line
	char line[MAX_LINE + 2];

	while (fgets(line, MAX_LINE + 2, path) != NULL) {
		//increase lines in stats
		incStats(&stats.lines);

		//search keyword in line
		if (strstr(line, searchWord) != NULL){
			//increase lineHits
			incStats(&stats.lineHits);
			//found string
			stringFound = 1;
		}
	}
	//fgets = NULL: 1)EOF, 2)error, ferror(path)
	if (ferror(path)) {
		die("fgets");
	}

	//update fileHits
	if (stringFound == 1) {
		incStats(&stats.fileHits);
	}

	//closefile
	if (fclose(file) == EOF) {
		die("fclose");
	}

	//decrease activeGrepThread in stats
	decStats(&stats.activeGrepThreads);

	//free one grep thread in semaphore
	V(maxGrepSem);

	return NULL;
}

