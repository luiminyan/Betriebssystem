#include <stdio.h>

int main(int argc, char* argv[]){
	//ptr1
	int i = 12;
	long j = 3271700000;

	int *ptr1 = &i;	//the pointer 
	printf("address of ptr1 = %p\n", ptr1);
	printf("address of ptr1 + 1 = %p\n", ptr1 + 1); //ptr + 4 * 1 byte
	
	long *ptr2 = &j;
	printf("address of ptr2 = %p\n", ptr2);
	printf("address of ptr2 + 1 = %p\n", ptr2 + 1);
	
	return 0;
}
