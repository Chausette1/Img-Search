#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#include "common.h"

int compareImages(char* base, int fd_read)
{
    char buffer[MAXPATHLENGTH];
    ssize_t bytes;

    while (1)  // Dahl loop my beloved
    {
        // Lire le chemin suivant du pipe

        bytes = read(fd_read, buffer, MAXPATHLENGTH);

        if (bytes < 0)
        {
            perror("read");
            return 1;
        }

        if (buffer[0] == (char) 255) break;  // Détecter le end flag

        // Effectuer la comparaison

        pid_t pid = fork();

        if (pid == 0)
        {
            // Supposé que img-dist est dans PATH
            printf("current proc: %d\n", getpid());
            printf("buffer = \"%s\"; length = %lu\n", buffer, strlen(buffer));

            // img-dist : 0 -> 64 = ok, 65 -> = erreur

            printf("args = %s %s\n", base, buffer);

            execlp("img-dist", "img-dist", base, buffer, (char*) NULL);

            perror("execlp");
            exit(65);
        }
        else if (pid > 0)
        {
            int status;
            
            if (wait(&status) < 0 ) 
            {
                perror("wait");
                return 1;
            }
            else// if (WEXITSTATUS(status) >= 0 && WEXITSTATUS(status) <= 64)
            {
                // Comparer et enregistrer

                printf("img-dist exit status = %d\n", WEXITSTATUS(status));
            }
        }
        else
        {
            printf("pid=%d ", getpid());
            perror("compareImages -> fork");
            return 1;
        }
    } 

    return 0;
}

/*
    Fermer les pipes et détacher le shm. Cette fonction doit être la dernière à
    être appelée.
*/
void cleanup_child(int fd_read, const char* shm_addr)
{
    close(fd_read);  // À la fin, ferme tous les pipes

    if (shmdt(shm_addr) < 0)  // Détacher le shm
    {
        perror("shmdt");
        exit(1);
    }
}
