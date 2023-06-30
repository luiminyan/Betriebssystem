#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxBufferSize 102   //100 for real word length + 1 for \n + 1 for \0 (end of a string)


//if error comes, kill the program and exit with EXIT_FAILURE!
static void kill(const char *programName){
    perror(programName);    //perror: print the message string and the errno information
    exit(EXIT_FAILURE);     //exit the program with failure
}

//the compare function used as a parameter in qsort, alphabetical(a->z) sort
static int compare(const void* a, const void* b){
    char* const* ia = (char* const*) a;  //casting from (pointer to) const void to (pointer to) const string(char*)
    char* const* ib = (char* const*) b;
    /*if (*ia < *ib){
        return -1;
    }
    else if (*ia > *ib){
        return 1;
    }
    else return 0; */

    return strcmp(*ia, *ib);    //equal as above
}

int main(int argc, char**argv){
    char buffer[maxBufferSize]; //create buffer for a
    char ** wordsList = NULL;    //to store the corrected words, {pointer to {a,b,c}, ptr to {d,e,f},...}
    unsigned int allocCount = 0;    //the number of lines in wordsList
    unsigned int wordCount = 0;     //how many words are stored

    while(fgets(buffer, maxBufferSize, stdin)){             //if fgets == NULL, 1st situation: error, 2nd situation: the end of stdin is reached
        //fgets(buffer, length, fp=stdin), read from stdin(Standardeingabe)
        //get a string is from stdin into buffer
        size_t lenOfBuffer = strlen(buffer);    //get the length of current buffer
        //1st. case: the word is too long (>100)
        if(lenOfBuffer == 101 && buffer[100] != '\n'){
            fprintf(stderr, "The word is too long!\n");
            //also skip the rest of this line(the part which is too long)
            int c;
            do {
                c = getchar();
            }while((c != EOF) &&(c != '\n'));
            continue; //skip
        }

        //remove the '\n', as it is not part of the word,
        if (buffer[lenOfBuffer - 1] == '\n'){
            buffer[lenOfBuffer - 1] = '\0';     //replace '\n' with '\0', '\0' is not calculated in length
            lenOfBuffer--;
        }

        //2nd case: the line is empty, word length = 0
        if(lenOfBuffer <= 0){
            continue;   //skip
        }

        //store the corrected words
        if (wordCount >= allocCount) {       //the wordList is full
            allocCount += 100;
            wordsList = realloc(wordsList, allocCount * sizeof(*wordsList));
            //allocCount *  sizeof(*wordsList) = total line number * line size (the first ptr stored in the list)

            //error of realloc
            if (wordsList == NULL) {
                kill("realloc");
            }
        }

        char *line = strdup(buffer);    //create duplicate of buffer in line

        //error of strdup
        if (line == NULL){
            kill("strdup");
        }

        wordsList[wordCount] = line;    //store the pointer to the word
        wordCount++;        //one word is read corrected and stored

    }

    //the first situation of fgets == NULL, the error
    if(ferror(stdin)){              //if there is an error by fgets, call kill, EXIT_FAILURE
        kill("fgets / getchar");
    }

    //sort the wordsList
    qsort(wordsList, wordCount, sizeof(*wordsList), compare);         //*wordsList: the first pointer stored in the wordsList

    //print out
    for (int i = 0; i < wordCount; i++){
        if (puts(wordsList[i]) == EOF){
            kill("puts");
        }
        free(wordsList[i]);     //free the allocation of the wordsList by line
    }

    free(wordsList);    //totally free the allocation of wordsList

    //flush the stdout / clean the buffer in stdout
    if (fflush(stdout) == EOF){
        kill("fflush");
    }

    exit(EXIT_SUCCESS); //exit(0)
}