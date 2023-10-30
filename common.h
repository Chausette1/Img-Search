#ifndef PROJOS_COMMON
#define PROJOS_COMMON

#define READ 0
#define WRITE 1

#define MAXPATHLENGTH 1000  // 999 chars + '\n'

typedef struct 
{
    int dist;
    char path[MAXPATHLENGTH];
} 
shm_data_t;

#define SHMSIZE sizeof(shm_data_t)  // == 1004 : 1000 pour le chemin, 4 pour la distance (int)

#endif
