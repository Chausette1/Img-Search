#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"

#include "parent.h"
#include "child.h"

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
        Créer les pipes 
    */

    int fd[2][2];  // 2 fd, un pour chaque processus fils

    if (pipe(fd[0]) < 0 || pipe(fd[1]) < 0) 
    {
        perror("Création des pipes");
        exit(1);
    }

    /* 
        Créer la mémoire partagée
    */

    int key = 1729;
    int shm_id = shmget(key, SHMSIZE, IPC_CREAT | IPC_EXCL | 0600);

    if (shm_id < 0) 
    {
        perror("shmget");
        exit(1);
    }

    const char* shm_addr = shmat(shm_id, NULL, 0);  // Attacher le shm pour tout processus

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

            /*  Fonctionnalité  */

            int res = compareImages(argv[1], fd[i][READ]);
            printf("res = %d\n", res);

            /*  Cleanup  */

            cleanup_child(fd[i][READ], shm_addr);
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
    
    cleanup_parent(fd, shm_addr, shm_id);
    exit(res);
}
