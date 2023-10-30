#ifndef PROJOS_PARENT
#define PROJOS_PARENT

#include <sys/types.h>

void cleanup_parent(int fd[2][2], const char* shm_addr, int shm_id);

int feedImagePaths(pid_t pid[2], int fd[2][2]);

#endif
