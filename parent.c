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

// ---- Init ---- //

// Signals

int flag_end_parent = 0;

void handle_signal_parent(int signal)
{
    if (signal == SIGINT || signal == SIGPIPE)
    {
        printf("Terminated by signal %d\n", signal);
        flag_end_parent = 1;
    }
}

void init_parent(int fd[2][2])
{
    close(fd[0][READ]);
    close(fd[1][READ]);

    signal(SIGINT, handle_signal_parent);
    signal(SIGPIPE, handle_signal_parent);
}

// ---- Foncitonnalité ---- //

void terminateChildProcesses(pid_t pid[2]);

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

    while (1)  // Lire jusqu'à EOF ou erreur
    {
        if (flag_end_parent)
        {
            terminateChildProcesses(pid);
            return 0;
        }

        if (fgets(buffer, MAXPATHLENGTH, stdin) == NULL) break;

        // Supprimer le newline éventuel

        buffer[strcspn(buffer, "\n")] = '\0';

        if (write_safe(fd[i % 2][WRITE], buffer, MAXPATHLENGTH) < 0)
        {
            terminateChildProcesses(pid);
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

// ---- Cleanup ---- //

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

    if ((err |= sem_close(sem) < 0)) perror("sem_close");
    if ((err |= sem_unlink(SEMNAME) < 0)) perror("sem_unlink");

    return err;
}
