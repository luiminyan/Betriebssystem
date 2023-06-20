#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>   // für wait() und waitpid(), siehe man 3 wait
#include <sys/types.h>  // für fork(), siehe man fork
#include <unistd.h>     // für fork()
#include <errno.h>      // für Fehlerbehandlung

// Eingabe im Terminal: ./listRun programm arg1 ... argn
// argv[0] = listRun
// argv[1] = programm
// argv[2] = arg1
// ...
// argv[argc-1] = argn

// Hilfsfunktion für Fehlerbehandlung für Funktionen, die die errno setzen
static void die(const char *programm){
    perror(programm);
    exit(EXIT_FAILURE);
}


int main(int argc, char** argv){

    // Nutzungshinweise, optional
    if(argc<2){
        fprintf(stderr, "Usage: ./listRun <programm> <arg1> ... \n");
        exit(EXIT_FAILURE);
    }
    
    // Schleife beginnt bei 2, da in argv[2] das erste Argument für den Befehl aus argv[1] steht
    for(int i=2; i<argc; i++){
        
        // Kindprozess erzeugen und PID in p speichern
        pid_t p = fork();
		
		//Fehlerbehandlung
		if(p == -1)
			die("fork");
        
        // Kindprozess: Ausführen des Programms mit entsprechendem Parameter
        if(p == 0){
            // Programm ausfuehren
			execlp(argv[1], argv[1], argv[i], NULL);
			//Fehlerbehandlung
			die("exec");
        }
        
        // Elternprozess: auf Beendigung von Kind warten
        else{
            // Zombie-Prozess aufräumen
			int status;
			pid_t x = wait(&status);
			
			//Fehlerbehandlung
			if(x == -1){
				perror("wait");
				continue;
			}
			
            //Überprüfen des Exit-Status des Zombie-Prozesses
            if(WIFEXITED(status)){
				if(printf("EXITED WITH: %d\n", WEXITSTATUS(status)) < 0){
					die("printf");
				}
				
			}
        }
    }
}