#include <stdio.h>	//printf
#include <errno.h>	//errno
#include <stdlib.h>	//exit
#include <string.h>	//string			



int main(int argc, char** argv){
	//or dynamic array: char* read_buffer = malloc(size);
	char read_buffer[16];	//15 Zeichen
	
	while(1){
		fgets(read_buffer, 16, stdin);	//read from stdin, store into read_buffer
						//eof will be read / included in the 16

		//end of file stdin
		if (feof(stdin)){
			//fputs and fprintf are both okay here
			fputs("fgets read EOF\n", stderr);
			//fprintf(stderr, "fgets");
			fflush(stderr);		//flush stderr
			exit(EXIT_FAILURE);	//exit failure
		}

		//deal with error
		if(ferror(stdin)){
			perror("fgets");	//if errno is set
			fflush(stderr);		//flush stderr
			exit(EXIT_FAILURE);
		}
		
		fprintf(stdout, "Read: %s", read_buffer);
		fflush(stdout);

		if (ferror(stdout)){
			perror("ignore this");
			fflush(stderr);
			clearerr(stdout);
			exit(EXIT_FAILURE);
		}
	}
}
