#ifndef __CHILD_H
#define __CHILD_H

#include <semaphore.h>
#include <sys/types.h>

#include "common.h"

int compareImages(char* base, int fd_read, int * dist, char * path, sem_t * sem);

int cleanup_child(int fd_read, int * dist, char * path);

#endif
