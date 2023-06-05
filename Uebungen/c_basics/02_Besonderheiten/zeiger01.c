#include <stdio.h>	//for the printf

int main(int argc, char* argv[]){
	int a = 42;
	//create pointer b to a
	int* b = &a;

	//read the value of a by opening b
	printf("a = %d\n", *b);

	//rewrite a to 23 through b
	*b = 23; 
	printf("new a = %d\n", *b);
	printf("the address of a is %p", b);
}


