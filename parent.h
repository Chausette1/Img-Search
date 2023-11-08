#ifndef __PARENT_H
#define __PARENT_H

#include <semaphore.h>
#include <sys/types.h>

void init_parent(int fd[2][2]);

int feedImagePaths(pid_t pid[2], int fd[2][2]);

int cleanup_parent(int fd[2][2], int * dist, char * path, sem_t * sem);

#endif
