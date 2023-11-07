#ifndef PROJOS_PARENT
#define PROJOS_PARENT

#include <sys/types.h>

void cleanup_parent(int fd[2][2]);
void terminateChildProcesses(pid_t pid[2]);
int feedImagePaths(pid_t pid[2], int fd[2][2]);

#endif
