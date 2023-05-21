#include <stdio.h>
#include <stdbool.h>
/*
 * Gruppe T02.05
 * Minhua Liu 108020210282
 */

int main(int argc, char **argv) {
	// TODO: implement sieve here!
    const int MAX = 100;
    bool prime[MAX + 1];    //from 0 to 100, there are 101 elements

    //initialize the the array
    for (int i = 0; i <= MAX; i++){         //from element 0 to 100 = MAX
        prime[i] = true;
    }

    //cross out elements 0 and 1, as they are not prime
    prime[0] = false;
    prime[1] = false;

    //the sieve
    for (int i = 2; i <= MAX; i++){                  //start with element 2
        for (int j = i; (j * i) <= MAX; j++){       //i * i <= MAX
            prime[j * i] = false;
        }
    }

    //print out result
    for (int i = 0; i <= MAX; i++){
        if (prime[i]){
            printf("%d\n", i);
        }
    }
}
