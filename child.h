#ifndef __CHILD_H
#define __CHILD_H

#include "common.h"

int compareImages(char* base, int fd_read, int * dist, char * path, sem_t* sem);

void cleanup_child(int fd_read, int * dist, char * path);

#endif
