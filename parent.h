#ifndef PROJOS_PARENT
#define PROJOS_PARENT

void cleanup_parent(int fd[2][2], const char* shm_addr, int shm_id);

void feedImagePaths(const int fd_writes[2]);

#endif
