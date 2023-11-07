#ifndef PROJOS_COMMON
#define PROJOS_COMMON

#define READ 0
#define WRITE 1

#define MAXPATHLENGTH 1000  // 999 chars + '\n'

typedef struct SHMData
{
    int dist;
    char path[MAXPATHLENGTH];
} 
SHMData;

#define SHMSIZE sizeof(SHMData)  // == 1004 : 1000 pour le chemin, 4 pour la distance (int)

#endif
