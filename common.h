#ifndef __COMMON_H
#define __COMMON_H

#define READ 0
#define WRITE 1

#define MAXPATHLENGTH 1000  // 999 chars + '\n'

// Nom du sémaphore utilisé en OSX (pas de sémaphores non-nommés)
#define OSX_SEMNAME "/img-search"

// #define flagerr(res, cond) (res |= cond)  // `res == 1` si `cond` est vrai

#endif
