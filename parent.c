#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"

void cleanup_parent(int fd[2][2], const char* shm_addr, int shm_id)
{
    close(fd[0][WRITE]);
    close(fd[1][WRITE]);

    if (shmdt(shm_addr) < 0)  // Détacher le shm
    {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shm_id, IPC_RMID, NULL) < 0)  // Supprimer le shm
    {
        perror("shmctl");
        exit(1);
    }
}

void feedImagePaths(const int fd_writes[2])
{
    int i = 0;
    char buffer[MAXPATHLENGTH];

    // while (fgets(buffer, MAXPATHLENGTH, stdin) != NULL)  // Lire jusqu'à EOF ou erreur
    // {
        
    // }
}
