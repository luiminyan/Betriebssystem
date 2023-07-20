#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static void die(char* prog_name){
	perror(prog_name);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	//reset errno
	errno = 0;

	//create buffer for getcwd
	int buffer_length = 1;
	char* buffer = malloc(sizeof(char*) * buffer_length);
	//error by malloc
	if (buffer == NULL) {
		die("malloc");
	}

	/* //getcwd()
	if (getcwd(buffer, buffer_length) == NULL) {
		if (errno == ERANGE) {
			printf("buffer not big enough");
			die("getcwd");
		}
	}  */

	//可以使用while处理，扩大buffer
	while (getcwd(buffer, buffer_length) == NULL) {
		if (errno == ERANGE) {
			buffer_length *= 2;		//enlarge the buffer size
			//realloc
			buffer = realloc(buffer, sizeof(char*) * buffer_length);
			//error by realloc
			if (buffer == NULL) {
				die("realloc");
			}
		}
		//getcwd dies of other reasons
		else die("getcwd");
	}

	printf("The path is: %s\n", buffer);
	if (ferror(stdout)) {
		die("printf");
	}
	
	//flush the stdout
	if (fflush(stdout) == EOF) {
		die("fflush");
	}

	//free the allocation
	free(buffer);

	exit(EXIT_SUCCESS);
}

