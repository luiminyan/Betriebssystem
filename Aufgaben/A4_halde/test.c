#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {
	printList();

	//allocation by malloc
	char *m1 = malloc(200*1024);
	printList();

	char *m2 = malloc(200*1024);
	printList();

	char *m3 = malloc(200*1024);
	printList();

	char *m4 = malloc(200*1024);
	printList();

	//free
	free(m1);
	printList();

	free(m2);
	printList();

	free(m3);
	printList();

	free(m4);
	printList();

	//again 4 times malloc
	char *m1 = malloc(200*1024);
	printList();

	char *m2 = malloc(200*1024);
	printList();

	char *m3 = malloc(200*1024);
	printList();

	char *m4 = malloc(200*1024);
	printList();

	//Randfall von malloc

	exit(EXIT_SUCCESS);
}
