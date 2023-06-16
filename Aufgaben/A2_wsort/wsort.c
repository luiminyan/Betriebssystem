#include <stdio.h>	//fprintf()
#include <stdlib.h>	//exit()
#include <string.h>	//string operations

#define MAX_LENGTH 100

//deal with 'real' error(setted errno)
static void kill(char* program_name){
	perror(program_name);	//print error
	exit(EXIT_FAILURE);	//exit with error
}

//sort algorithm
static int compare_func(const void* a, const void* b){
	//const char* = pointer of const char
	//char* const = const pointer of char
	//char* const* = const pointer to (pointer to char = string) !!not necessary!!
	//const char** = pointer to (pointer to const char = array of const char)
	const char** ca = (const char**) a;
	const char** cb = (const char**) b;
	return strcmp(*ca, *cb);
}
/*
 * todo list:
 * 1. read input
 * 2. corrct string
 * 3. store word into word_list
 * 4. sort input
 * 5. write in output
 * 6. free the space
 * 7. clean stdout
 * */

int main(int argc, char* argv[]){
	//create word_list
	size_t list_length = 1;		//word_list length
	size_t collected_word = 0;		//word collected, to check if the list size needs ti be increased
	char** word_list = calloc(list_length, sizeof(char*));		//all words are stored in word_list as pointers

	//read input
	while(1){
		//1. read a line / word from from stdin
		//create buffer to store the word
		char buffer[MAX_LENGTH + 2];	//100 + '\n' + '\0' ('\0' won't be read by strlen, but the place is need by fgets())
		if (fgets(buffer, MAX_LENGTH + 2, stdin) == NULL){			
			//fgets():读到n-1，或者EOF就停止, need to read till '\n'
			//fgets会自动补'\0'
			//1st case: end of file
			if (feof(stdin)){		//end of file (input), normal case, read next line
				break;				
			}
			//2nd case: error
			//else if ferror(stdin)
			else kill("fgets");
		};	

		//2. correct the string
		//strlen() instaed of sizeof(): get the real length of string, '\0' is not included in calculation
		size_t buff_length = strlen(buffer);		
		//string too long
		if (buff_length >= 101 && buffer[100] != '\n'){
			fprintf(stderr, "String too long!\n");	//as "too long" is not an error, cannot use perror here

			//skip the too long part of this line / word / string
			int c;
			//first do then check condition
			do {
				c = getchar();
				// c = getc(stdin); 
			} while (c != '\n' && c != EOF);
			if (c == EOF && !feof(stdin)){
				//getchar return EOF, can also be an error
				kill("getc");
			}
			continue;	//skip this string
		}

		//empty line
		if (buffer[0] == '\n'){ 		//'\n' is not yet erased
			continue;	//skip
		}


		//erase '\n' in string
		if (buffer[buff_length - 1] == '\n'){
			buffer[buff_length - 1] = '\0';
			buff_length--;
		}

		/* //empty line
		if (buff_length == 0){ 		//as strlen does not count '\0'
			continue;	//skip
		} */

		//3. store word into word_list
		//list is full, increase the list length
		if (collected_word >= list_length){		
			list_length *= 2;	//increase list length
			//reallocate the word_list
			/* 
			word_list = realloc(word_list, list_length * sizeof(char*));
			//or: list_length * sizeof(*word_list), size of the first stored in word_list
			if (word_list == NULL){
				kill("realloc");
			}
			*/

			//better: create a copy of word_list, them store back
			char** new_list = realloc(word_list, list_length * sizeof(char*));
			if (new_list == NULL){
				kill("realloc");
			}
			word_list = new_list;
		}

		//copy buffer (the word)
		char* buff_duplic = strdup(buffer);
		//store buff_duplic into word_list
		word_list[collected_word] = buff_duplic;
		collected_word++;
	}

	//4. sort input
	//qsort(list, how many, size of each element, compare function);
	qsort(word_list, collected_word, sizeof(char*), compare_func);

	//5. write into output
	for (int i = 0; i < collected_word; i++){
		if (puts(word_list[i]) == EOF){
			kill("puts");
		}

		/* 
		 * if (printf("%s\n", word_list[i]) < 0){
			kill("fprintf");
		 }
		 */

		//6. free the allocation by line
		free(word_list[i]);	
	}
	//6. free allocation totally
	free(word_list);

	//7. fflush the stdout
		if (fflush(stdout) == EOF){
			kill("fflush");
		}

	exit(EXIT_SUCCESS);	//exit successfully
}

