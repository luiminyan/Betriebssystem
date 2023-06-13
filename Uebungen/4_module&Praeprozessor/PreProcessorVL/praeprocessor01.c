#include <stdio.h>	//for printf()
#include <stdlib.h>	//for exit()

//marcos
#define PI 3.1415926	//define PI

//the if-else condition
#ifdef PI		//if PI is defined
	#undef PI	//then undefine the origin PI
	#define PI 3	//redefine PI of 3
#else 			//else of the if above
	#define PI 3	//define PI with 3
#endif			//end of the if-else condition

int main(int argc, char* argv[]){
	printf("Pi = %d\n", PI);	//Pi = 3.1415926
	
	exit(EXIT_SUCCESS);	//successfully
}
