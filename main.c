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
#include <string.h>

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
    *semptr = sem_open(SEMNAME, O_CREAT | O_EXCL, 0644, 1);

    if (*semptr == SEM_FAILED)
    {
        if (errno == EEXIST)
        {
            sem_unlink(SEMNAME);
            create_semaphore(semptr);
        }
        else
        {
            perror("sem_open");
            exit(1);
        }
    }
}

int main(int argc, char* argv[]) 
{
    /*
        Setup
    */

    //  1. Vérifier arguments

    if (argc != 2) 
    {
        printf("Usage : ./img-search image\n");
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

            init_child(fd[i][WRITE]);

            /*  Fonctionnalité  */

            res |= compareImages(argv[1], fd[i][READ], dist_shm, path_shm, sem_shm);

            /*  Cleanup  */

            res |= cleanup_child(fd[i][READ], dist_shm, path_shm, sem_shm);
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

    init_parent(fd);

    /*  Fonctionnalité  */

    res |= feedImagePaths(pid, fd);

    wait(NULL);
    wait(NULL);

    if (strlen(path_shm) > 0) 
    {
        printf("Most similar image found: '%s' with a distance of %d.", path_shm, *dist_shm);
    }
    else 
    {
        printf("No similar image found (no comparison could be performed successfully).");
    }

    /*  Cleanup  */
    
    res |= cleanup_parent(fd, dist_shm, path_shm, sem_shm);
    exit(res);
}
