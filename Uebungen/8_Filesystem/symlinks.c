#include <stdio.h>
#include <stdlib.h>
#include <errno.h>      //errno
#include <dirent.h> //dirent struct , DIR
#include <sys/stat.h>   //statc struct
#include <unistd.h>     //process, symlink,...
#include <sys/types.h>


static void kill(char* prog_name){
    perror(prog_name);
    exit(EXIT_FAILURE);
}

/*
 Aufgabe:
 Wir wollen ein Programm schreiben, welches unser
 aktuelles Verzeichnis durchläuft und uns alle darin
 vorhandenen Symlinks ausgibt.
 */

int main(int argc, char* argv[]){
    //open current directory
    DIR * dir_ptr = opendir(".");    // ".":current directory
    //dir_ptr is an interator, which stores the current position
    //error handling of opendir()
    if (dir_ptr == NULL){
        kill("opendir");
    }

    //init dirent to store the current directory info
    struct dirent * read_entry = NULL;

    //create stat buf to store item information by lstat()
    struct stat buf;        

    
    while(1){
        errno = 0;      //reset errno
        //read current directory
        read_entry = readdir(dir_ptr);  
        //error / EOF handling
        if (read_entry == NULL){
            //error
            if (errno != 0){
                kill("readdir");
            }
            //EOF
            break;
        }

        //extract the item into buf
        if (lstat(read_entry->d_name, &buf) == -1){
            //lstat(address, ptr of buffer (&ptr) to store)
            //0 success, -1 error and errno is set
            kill("lstat");
        }

        //find out file type, if not a symbolic link, skip
        if (!S_ISLNK(buf.st_mode)){
            continue;       
        }

        //read and store the target of the symlink
        //the length of the target
        off_t target_size = buf.st_size;
        //string for storage
        char target_symlink[target_size + 1];   //init one more space for '\0'
        target_symlink[target_size] = '\0';

        //store in string
        if (readlink(read_entry->d_name, target_symlink, target_size) == -1){
            kill("readlink");
        }

        //print 
        if (printf("%s -> %s\n", read_entry->d_name, target_symlink) < 0){
            kill("printf");
        }
    
    }

    //close directory
    if (closedir(dir_ptr) == -1){
        //closedir(): success 0, fail -1
        kill("closedir");
    }
    
    exit(EXIT_SUCCESS);
}
