#include <stdio.h>
#include <stdlib.h> //exit()

//marcos: define functions
#define MUL(a, b) a * b
#define ADD(a, b) a + b
#define POW(a) 1 << a

int main(int argc, char* argv[]){
	printf("ADD(3, 5) = %d\n", ADD(3, 5));  //8
	printf("MUL(3, 5) = %d\n", MUL(3, 5));	//15
	printf("POW(3) = %d\n", POW(3));	//8
	exit(EXIT_SUCCESS);
}
