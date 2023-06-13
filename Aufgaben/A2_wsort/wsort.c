#include <stdio.h>	//fprintf()
#include <stdlib.h>	//exit()
#include <string.h>	//string operations

//deal with 'real' error(setted errno)
static void kill(char* program_name){
	perror(program_name);	//print error
	exit(EXIT_FAILURE);	//exit with error
}

/*
 * todo list:
 * 1. read
 * */



int main(int argc, char* argv[]){

	exit(EXIT_SUCCESS);	//exit successfully
}

