#include <errno.h>
#include <stdio.h>
#include "strings.h"

char *bs_strchr(const char *str, char c) {
    // Entfernen wenn implementiert
    size_t counter = 0;
    while(str[counter] != '\0'){    //string not ended
        if (str[counter] == c) return counter + (char*)str;    //found char, return position
        counter++;
    }
    return NULL;    //char not found 
}

size_t bs_strlen(const char *str) { 
    char* ptr_position = bs_strchr(str, '\0');  //search for string end '\0'
    return ptr_position - str;
}

char *bs_strcpy(char *dst, const char *src) {

    // Entfernen wenn implementiert
    size_t i = 0;
    while (src[i] != '\0'){
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';  //write the end into the word 
    return dst;
}

char *bs_strcat(char *dst, const char *src) {

    // Entfernen wenn implementiert
    size_t len_dst = bs_strlen(dst);
    bs_strcpy(len_dst + dst, src);
    return dst;
}

int main(int argc, char* argv[]){
    printf("%s", bs_strcat("", "Dr. Milan M.\n"));
    return(0);
}
