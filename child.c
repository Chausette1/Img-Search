#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"

void compareImages(char* base, int fd_read)
{
    
}

void cleanup_child(int fd_read, const char* shm_addr)
{
    close(fd_read);  // À la fin, ferme tous les pipes

    if (shmdt(shm_addr) < 0)  // Détacher le shm
    {
        perror("shmdt for some ungodly reason");
        exit(1);
    }
}
