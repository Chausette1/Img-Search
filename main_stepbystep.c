#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

#include "common.h"

#include "parent.h"
#include "child.h"

void create_pipes(int fd[2][2])
{
    if (pipe(fd[0]) < 0 || pipe(fd[1]) < 0) 
    {
        perror("Création des pipes");
        exit(1);
    }
}

void * create_shm(size_t size)
{
    void * shmptr = mmap(
        NULL, 
        size, 
        PROT_READ | PROT_WRITE, 
        MAP_SHARED | MAP_ANON, 
        -1, 0
    );

    if (shmptr == MAP_FAILED)
    {
        perror("Création de la mémoire partagée");
        exit(1);
    }

    return shmptr;
}

void create_semaphore(sem_t ** semptr)
{
    #ifdef __APPLE__  // sem_init() n'existe pas sur OSX

        *semptr = sem_open(OSX_SEMNAME, O_CREAT | O_EXCL, 0644, 1);

        if (*semptr == SEM_FAILED)
        {
            if (errno == EEXIST)
            {
                sem_unlink(OSX_SEMNAME);
                create_semaphore(semptr);
            }
            else
            {
                perror("sem_open");
                exit(1);
            }
        }

    #else
    
        if (sem_init(*semptr, O_CREAT | O_EXCL | 0600, 1) < 0)
        {
            perror("sem_init");
            exit(1);
        }

    #endif
}

int main(int argc, char* argv[]) 
{
    /*
        Setup
    */

    //  1. Vérifier arguments

    if (argc != 2) 
    {
        printf("Un paramètre requis : chemin vers l'image de base\n");
        exit(1);
    }

    /*
        Créer les pipes 
    */

    int fd[2][2];  // 2 fd, un pour chaque processus fils

    create_pipes(fd);

    //  3. Créer la mémoire partagée

    int * dist_shm = create_shm(sizeof(int));
    char * path_shm = create_shm(sizeof(char) * MAXPATHLENGTH);

    sem_t * sem_shm = create_shm(sizeof(sem_t));

    //  4. Initialiser le sémaphore

    create_semaphore(&sem_shm);

    /* 
        Créer les processus (fork)
    */

    pid_t pid[2];

    for (int i = 0; i < 2; ++i)  // 2 processus fils
    {
        pid[i] = fork();

        if (pid[i] == 0)  // Processus fils
        {
            int res = 0;

            /*  Init  */

            close(fd[i][WRITE]);

            /*  Fonctionnalité  */

            res |= compareImages(argv[1], fd[i][READ], dist_shm, path_shm, sem_shm);
            printf("res = %d\n", res);

            /*  Cleanup  */

            res |= cleanup_child(fd[i][READ], dist_shm, path_shm);
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

    int res = 0;

    /*  Init  */

    close(fd[0][READ]);
    close(fd[1][READ]);

    /*  Fonctionnalité  */

    res |= feedImagePaths(pid, fd);
    
    int status[2];

    wait(&status[0]);
    wait(&status[1]);

    printf("C");

    printf("\nRESULTS FOR TODAY'S BATTLE : dist = %d, path = \"%s\"", *dist_shm, path_shm);

    /*  Cleanup  */
    
    res |= cleanup_parent(fd, dist_shm, path_shm, sem_shm);
    exit(res);
}
