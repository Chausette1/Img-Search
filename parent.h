#ifndef __PARENT_H
#define __PARENT_H

#include <sys/types.h>

void cleanup_parent(int fd[2][2], int * dist, char * path, sem_t* sem);

int feedImagePaths(pid_t pid[2], int fd[2][2]);

#endif
