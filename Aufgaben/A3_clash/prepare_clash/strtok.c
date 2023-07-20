#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>     //strtok
#include <errno.h>
static void die(char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    char cmd[40] = "ls\t-l\t/tmp";
    size_t cmd_len = 200;

    //create index for iteration
    int index = 0;

    //create buffer array to store the split commands
    char* cmd_array[cmd_len];

    //a much better solution
    char* token = NULL;

    //do a copy of the cmd
    char* cmd_copy = strdup(cmd);
    if (cmd_copy == NULL){
        die("strdup");
    }

    //extract first token
    token = strtok(cmd, "\t");
    while(token) {  
        //while(token != NULL)
        cmd_array[index] = token;
        //for the next 
        index++;
        token = strtok(NULL, "\t");
    }

    //print
    for (int i = 0; i < index; i++) {
        printf("%s\n", cmd_array[i]);
    }

    exit(EXIT_SUCCESS);
}