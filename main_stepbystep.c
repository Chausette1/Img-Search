#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include "common.h"
#include "parent.h"
#include "child.h"
#include "signal.h"

/*
    Création des variables globale qu'on a besoin dans la gestion des signaux.
*/

int fd[2][2];  // 2 fd, un pour chaque processus fils
int *shmem_lowest_distance; //variable globale distance

void *create_shared_memory(size_t size) {
/*
Pour créer de la mémoire partager il suffis de définir le type, donner un noms a sa variable et
d'appeller la fonction create_shared_memory() et de donner la taille en octets comme pour les malloc.
ex : int *shmem_tab = create_shared_memory(sizeof(int)*5) //pour un tableau de int de taille 5
et munmap(shmem_tab, sizeof(int)*5); // pour désallouer la mémoire partager
ps : le shmem veut dire shared memory 
*/   
  const int protection = PROT_READ | PROT_WRITE;
  const int visibility = MAP_SHARED | MAP_ANONYMOUS;
  int ret = mmap(NULL, size, protection, visibility, -1, 0);
  if (ret == MAP_FAILED){
        perror("mmap() error");
        exit(1);
  }
  return ret;
}


int main(int argc, char* argv[]) 
{
    /*
        Verifier arguments 
    */

    if (argc != 2) 
    {
        printf("Un paramètre requis : chemin vers l'image de base\n");
        exit(1);
    }

    setbuf(stdout, NULL);  // debug, uncomment to output printfs during deadlocks

    /*
        Gestion des signaux
    */

    if(signal(SIGINT, terminateChildProcesses) == SIG_ERR){
        perror("Signal error");
        exit(1);
    }

    /*
        Créer les pipes 
    */

    if (pipe(fd[0]) < 0 || pipe(fd[1]) < 0) 
    {
        perror("Création des pipes");
        exit(1);
    }

    /* 
        Créer la mémoire partagée
    */

    shmem_lowest_distance = create_shared_memory(sizeof(int));

    /* 
        Créer les processus
    */

    pid_t pid[2];

    for (int i = 0; i < 2; ++i)  // Gérer la création de 2 processus fils
    {
        pid[i] = fork();

        if (pid[i] == 0)  // Processus fils
        {
            /*  Init  */

            close(fd[i][WRITE]);
            
            if(signal(SIGINT, SIG_IGN) == SIG_ERR){
                perror("Signal error");
                exit(1);
            }
            if(signal(SIGTERM, handler_sigterm) == SIG_ERR){
                perror("signal error");
                exit(1);
            }

            /*  Fonctionnalité  */

            int res = compareImages(argv[1], fd[i][READ]);
            printf("res = %d\n", res);

            /*  Cleanup  */

            cleanup_child(fd[i][READ]);
            exit(res);
        }
        else if (pid[i] < 0)
        {
            perror("main -> fork");
            exit(1);
        }

    }

    // Processus parent

    /* 
        Ecrire dans les pipes les chemins d'images un par un
    */



    /*  Init  */

    close(fd[0][READ]);
    close(fd[1][READ]);

    /*  Fonctionnalité  */

    printf("A");

    int res = feedImagePaths(pid, fd);
    int status[2];

    /*  Cleanup  */

    printf("B");

    wait(&status[0]);
    wait(&status[1]);

    printf("C");
    
    cleanup_parent(fd);
    exit(res);
}
