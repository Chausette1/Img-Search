#ifndef __COMMON_H
#define __COMMON_H

#define READ 0
#define WRITE 1

#define MAXPATHLENGTH 1000  // 999 chars + '\n'

#define SEMNAME "/img-search"

// #define flagerr(res, cond) (res |= cond)  // `res == 1` si `cond` est vrai

int read_safe(int fd, char * buffer, size_t size);

int write_safe(int fd, char * buffer, size_t size);

#endif
