#include "fib.h"	//need the fib() function from fib.h / fib.c
#include <stdio.h>	//for the printf()

static void fib_loop(){
	for(int i = 1; i < 42; ++i){	//先加后执行
		printf("fib(%d) = %d\n", i, fib(i));
	}
}

int main(int argc, char* argv[]){
	fib_loop();
}

