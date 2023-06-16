#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compare_func(const void* a, const void* b){
    //casting
    const char** ca = (const char**) a;
    const char** cb = (const char**) b;
    //compare
    return strcmp(*ca, *cb);  //compare 2 strings
}

int main(int argc, char** argv){
    /* int result = compare_func("AAAA", "ABBBBB");
    printf("%d\n", result); */
    exit(EXIT_SUCCESS);
}