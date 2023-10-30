#ifndef PROJOS_CHILD
#define PROJOS_CHILD

int compareImages(char* base, int fd_read);

void cleanup_child(int fd_read, const char* shm_addr);

#endif
