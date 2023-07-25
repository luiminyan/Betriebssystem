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

