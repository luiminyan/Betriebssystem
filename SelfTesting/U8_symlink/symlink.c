#include <stdio.h>
#include <stdlib.h>
#include <errno.h>      //errno

#include <unistd.h>     //symlink
#include <dirent.h> //store directory info
#include <sys/stat.h>   //stat
#include <sys/types.h>

//Task: Ausgabe aller Dateinamen von symbolischen Verknüpfungen im aktuellen Verzeichnis

static void die(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    //open current dir
    DIR* direct = opendir(".");
    //error by opendir
    if (direct == NULL) {
        die("opendir");
    }

    //directory iterator
    struct dirent* entry;

    //store current item information
    struct stat buff;

    while (1) {
        //reset errno
        errno = 0;

        //readdir
        entry = readdir(direct);
        if (entry == NULL) {
            //error
            if (errno) {
                die("readdir");
            }
            //EOF
            else break;
        }

        //store current item information into buff
        //lstat(fpath, buff)
        if (lstat(entry->d_name, &buff) == -1) {
            die("lstat");
        }

        //check if link is link
        if (! S_ISLNK(buff.st_mode)) {
            //if not link
            continue;
        }

        //store the symlink information
        int buff_len = buff.st_size;
        char buff_link[buff_len + 1];
        buff_link[buff_len] = '\0';

        //readlink
        if (readlink(entry->d_name, buff_link, buff_len) == -1) {
            die("readlink");
        }

        //print
        if (printf("%s\n", buff_link) < 0) {
            die("printf");
        }
    }

    //close directory
    if (closedir(direct) == -1){
        die("closedir");
    }
    
    die(EXIT_SUCCESS);
}