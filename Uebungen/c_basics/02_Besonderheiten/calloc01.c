#include <stdio.h>	//printf
#include <stdlib.h>	//calloc, malloc, realloc
#include <string.h>	//memset

int main(int argc,char* argv[]){
	size_z ptriSize = sizeof(int*);
	int* a = malloc(100 * sizeof(ptriSize));
	printf("The size of an integer pointer is %d", ptriSize);
	printf("The size of a is %d", sizeof(*a));
	return(0);
}



