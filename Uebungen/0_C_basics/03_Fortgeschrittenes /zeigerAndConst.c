#include <stdio.h>

int main(int argc, char* argv[]){
	int a = 42, b = 23;

        //pointer on constant interger, the value is not changeable
	const int* p1 = &a;
	
 	//constant pointer on interger, pointer not changeable
	int* const p2 = &a;	
	
	p1 = &b;
	*p2 = 23;
}

