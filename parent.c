#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <string.h>
#include <semaphore.h>

#include "common.h"

#define WRITE_ATTEMPTS 3  // Si write() échoue 3 fois sans SIGPIPE, terminer

/*
    Envoyer un signal SIGTERM à chaque processus fils.
*/
void terminateChildProcesses(pid_t pid[2])
{
    for (int i = 0; i < 2; ++i)
    {
        if (kill(pid[i], SIGTERM) < 0) perror("Error terminating child process");
    }
}

/*
    Fermer les pipes et supprimer le shm. Cette fonction doit être la dernière à
    être appelée.
*/
int cleanup_parent(int fd[2][2], int * dist, char * path, sem_t * sem)
{
    int err = 0;

    close(fd[0][WRITE]);
    close(fd[1][WRITE]);

    if ((err |= munmap(dist, sizeof(int)) < 0)) perror("munmap");
    if ((err |= munmap(path, sizeof(char) * MAXPATHLENGTH) < 0)) perror("munmap");

    #ifdef __APPLE__

        if ((err |= sem_close(sem) < 0)) perror("sem_close");
        if ((err |= sem_unlink(OSX_SEMNAME) < 0)) perror("sem_unlink");

    #else

        if ((err |= sem_destroy(sem) < 0)) perror("sem_destroy");

        if ((err |= munmap(sem, sizeof(sem_t)) < 0)) perror("munmap");

    #endif

    return err;
}

/*
    Alimenter progressivement les pipes avec les chemins vers les images à comparer.

    Retourne :
        - 0 en cas de succès
        - 1 en cas d'erreur
*/
int feedImagePaths(pid_t pid[2], int fd[2][2])
{
    unsigned int i = 0;
    char buffer[MAXPATHLENGTH];

    while (fgets(buffer, MAXPATHLENGTH, stdin) != NULL)  // Lire jusqu'à EOF ou erreur
    {
        // Supprimer le newline éventuel

        buffer[strcspn(buffer, "\n")] = '\0';

        // Si write() échoue 3 fois sans SIGPIPE, abandonner et terminer
        // EX : si le parent est interrompu par un signal mais résume après

        unsigned char att = 0;
        ssize_t bytes;

        do 
        {
            bytes = write(fd[i % 2][WRITE], buffer, MAXPATHLENGTH);

            

            att++;
        } 
        while (bytes < 0 && att < WRITE_ATTEMPTS);

        if (bytes < 0)  // Write échec
        {
            terminateChildProcesses(pid);

            perror("write");
            return 1;
        }

        i++;
    }

    if (ferror(stdin)) 
    {
        terminateChildProcesses(pid);

        perror("fgets");
        return 1;
    }

    buffer[0] = (char) -1;  // End flag
    write(fd[0][WRITE], buffer, MAXPATHLENGTH);
    write(fd[1][WRITE], buffer, MAXPATHLENGTH);

    return 0;
}
