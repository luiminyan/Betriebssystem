#include <stdio.h>	//for printf
#include <stdlib.h>	//for malloc, calloc and free

//def struct date
typedef struct date{
	int day;
	int month;
	int year;
} date_t;


int main(int argc, char* argv[]){
	date_t* d = malloc(sizeof(date_t*));	//create ptr to date_t with size of the ptr
	
	if (d == NULL){
		printf("error of malloc");
	}
	else {
		//print the size of malloc result
		int s = sizeof(d);
		printf("size of s = %d", s);
	}

	//do the free()
	free(d);	//need to specify the thing to free
}

