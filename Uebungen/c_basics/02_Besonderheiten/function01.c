#include <stdio.h>	//printf
void swap(int a, int b){
	int x = b;
	b = a;
	a = x;
}

void swapReference(int* a, int* b){
	int x = *b;
	*b = *a;
	*a = x;
}

int main(int argc, char* argv[]){
	int a = 32;
	int b = 1337;
	
	//call by value
	swap(a, b);
	printf("call by value:\t a = %d,\t b = %d\n", a, b);
	//call by refernce
	swapReference(&a, &b);
	printf("call by reference:\t a = %d,\t b = %d\n", a, b);
}

