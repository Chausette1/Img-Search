#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>

#include "common.h"

#define READ 0
#define WRITE 1

#define INBUFSIZE 1000

void killAll(pid_t pid_1, pid_t pid_2) {
    kill(pid_1, SIGTERM);
    kill(pid_2, SIGTERM);


}

void compareImages(int read_fd) {

}



pid_t createChildProcess(int fd[2]) {
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        signal(SIGINT, SIG_IGN);  //On ignore le signal d'interuption

        close(fd[WRITE]);
        compareImages(fd[READ]);
        close(fd[READ]);

        exit(0);
    } 
    else if (pid > 0) {  // Parent process
        signal(SIGINT, killAll); //on redéfini les signaux de sortie pour close de manière propre

        close(fd[READ]);
    } 
    else {
        perror("Fork failed");
        exit(1);
    }

    return pid;
}

void *create_shared_memory(size_t size) {
/*
Pour créer de la mémoire partager il suffis de définir le type, donner un noms a sa variable et
d'appeller la fonction create_shared_memory() et de donner la taille en octets comme pour les malloc.
ex : int *shmem_tab = create_shared_memory(sizeof(int)*5) //pour un tableau de int de taille 5
ps : le shmem veut dire shared memory 
*/   
  const int protection = PROT_READ | PROT_WRITE;
  const int visibility = MAP_SHARED | MAP_ANONYMOUS;
  return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) { ///Il faut 2 arg pour lancer le programme (nom du programme + chemin de l'img)
        exit(1);
    }

    char *path = argv[1]; /// création de path qui est le chemin de l'img a comparer

    // Create pipes

    int fd_1[2], fd_2[2];

    if (pipe(fd_1) < 0 || pipe(fd_2) < 0) {
        perror("Pipe failed");
        exit(1);
    }

    // Create shared memory
    int *shmem_lowest_distance = create_shared_memory(sizeof(int));
    if (*shmem_lowest_distance == MAP_FAILED){
        perror("mmap() error");
        exit(1);
    }
    // ...

    // Create child processes

    pid_t pid_1 = createChildProcess(fd_1),
          pid_2 = createChildProcess(fd_2);
    
    // !! Beyond this point, only the parent process is executing
    
    // Feeding image paths to pipes

    char buffer[INBUFSIZE];
    int writes[2] = {fd_1[WRITE], fd_2[WRITE]};

    for (int i = 0; fgets(buffer, INBUFSIZE, stdin) != NULL; ++i) {  // is NULL on EOF or error
        if (write(writes[i % 2], buffer, strlen(buffer) + 1) < 0) { // why strlen(buffer) "+ 1"
            perror("");
        }
    }

   return 0;
}
