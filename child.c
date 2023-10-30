#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"

int compareImages(char* base, int fd_read)
{
    char buffer[MAXPATHLENGTH];
    ssize_t bytes;

    bytes = read(fd_read, buffer, MAXPATHLENGTH);

    while (buffer[0] != '\0')  // Si '\0', alors fin des chemins à comparer
    {
        if (bytes < 0)
        {
            perror("read");

            return 1;
        }

        printf("%d, %s\n", getpid(), buffer);

        bytes = read(fd_read, buffer, MAXPATHLENGTH);
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
