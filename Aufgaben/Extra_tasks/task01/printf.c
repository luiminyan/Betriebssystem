#include <stdio.h>    //fprintf()
#include <stdlib.h>   //exit()
#include <errno.h>    //errno

static void usage(int code) {
    //not an real error => use fprintf()
    fprintf(stderr, "usage: printf <ip0.ip1.ip2.ip3>\n");   
    exit(code);
}

int main(int argc, char *argv[]) {
    // Prüfung auf genau ein Argument
    /*
     * argc = 目录 + 指令数
     * argv[0] = address
     * argv[1] = 第一个指令
    */ 
    if (argc != 2){
      usage(EXIT_FAILURE);  //fail
    }

    unsigned char ip[4];

    // Formatstrings für die Ausgabe der IP im Oktal-, Dezimal- und Hexadezimalsystem
    const char *format[] = {
      "Oct: % 3hho-% 3hho-% 3hho-% 3hho\n",
      "Dec: %hhu.%hhu.%hhu.%hhu\n",
      "Hex: %02hhx:%02hhx:%02hhx:%02hhx\n"
    };

    // IP mit sscanf parsen, auf Fehler prüfen
    int end;
    int scanf_return = sscanf(argv[1], "%hhu.%hhu.%hhu.%hhu%n", &ip[0], &ip[1], &ip[2], &ip[3], &end);

    //input too long
    
    //

    // IP-Adressen ausgeben (Dabei über das Array "format" iterieren)



    // Prüfung auf Fehler bei der Ausgabe
    if (fflush(stdout) == -1){          //flush stdout
      perror("fflush");     //an error with errno set
      exit(EXIT_FAILURE);
    }

    // Beenden des Programms
    exit(EXIT_SUCCESS);   //exit without error
}
