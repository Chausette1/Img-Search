#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>

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

void cleanup_parent(int fd[2][2], int* shm_addr, int shm_id)
{
    close(fd[0][WRITE]);
    close(fd[1][WRITE]);

    if (shmdt(shm_addr) < 0)  // Détacher le shm
    {
        perror("shmdt for some ungodly reason");
        exit(1);
    }

    if (shmctl(shm_id, IPC_RMID, NULL) < 0)  // Supprimer le shm
    {
        perror("shmctl");
        exit(1);
    }
}

void compareImages(char* base, int fd_read)
{
    
}

void feedImagePaths(const int fd_writes[2])
{
    int i = 0;
    char buffer[MAXPATHLENGTH];

    // while (fgets(buffer, MAXPATHLENGTH, stdin) != NULL)  // Lire jusqu'à EOF ou erreur
    // {
        
    // }
}

void create_pipes(int fd[2][2])
{
    if (pipe(fd[0]) < 0 || pipe(fd[1]) < 0) 
    {
        perror("Création des pipes");
        exit(1);
    }
}

void * create_shm(size_t size)
{
    void * shmptr = mmap(
        NULL, 
        size, 
        PROT_READ | PROT_WRITE, 
        MAP_SHARED | MAP_ANON, 
        -1, 0
    );

    if (shmptr == MAP_FAILED)
    {
        perror("Création de la mémoire partagée");
        exit(1);
    }

    return shmptr;
}

int create_semaphore(sem_t ** semptr, const char * sem_name)
{
    #ifdef __APPLE__  // sem_init() n'existe pas sur OSX

        *semptr = sem_open(sem_name, O_CREAT | O_EXCL | 0600, 1);

        if (*semptr == SEM_FAILED)
        {
            perror("sem_open");
            return 1;
        }

    #else
    
        if (sem_init(*semptr, O_CREAT | O_EXCL | 0600, 1) < 0)
        {
            perror("sem_init");
            return 1;
        }

    #endif

    return 0;
}

int main(int argc, char* argv[]) 
{
    /*
        Setup
    */

    //  1. Vérifier arguments

    if (argc != 2) 
    {
        printf("Un paramètre requis : chemin vers l'image de base\n");
        exit(1);
    }

<<<<<<< Updated upstream
    /*
        Créer les pipes 
    */
=======
    setbuf(stdout, NULL);  // debug, uncomment to output printfs during deadlocks

    //  2. Créer les pipes 
>>>>>>> Stashed changes

    int fd[2][2];  // 2 fd, un pour chaque processus fils

    create_pipes(fd);

    //  3. Créer la mémoire partagée

    int * dist_shm = create_shm(sizeof(int));
    char * path_shm = create_shm(sizeof(char) * MAXPATHLENGTH);

    sem_t * sem_shm = create_shm(sizeof(sem_t));

<<<<<<< Updated upstream
    int* shm_addr = shmat(shm_id, NULL, 0);  // Attacher le shm pour tout processus
=======
    //  4. Initialiser le sémaphore

    const char * sem_name = "/imgcomp_sem_1729374";  // Nom du sémaphore dans le cas OSX

    create_semaphore(&sem_shm, sem_name);

    // sem_t sem;
    // if (sem_init(&sem, 0, 1) < 0)
    // {
    //     perror("sem_init");
    //     if (shmctl(shm_id, IPC_RMID, NULL) < 0) perror("shmctl");
    //     exit(1);
    // }
>>>>>>> Stashed changes

    /* 
        Créer les processus (fork)

            * Un pipe par processus (fonction dont on passe le fd par param ?)
            * Si parent, close(fd[READ])
            * Si enfant:
                - Ignorer SIG_INT (pas sûr que c'est bien ça qu'il faut faire ?)
                - close(fd[WRITE])
                - Peut-être une fonction comparerImages() qui attend systématiquement
                  des écritures de path d'image dans fd venant du parent (avec while loop
                  et pause() ?)
            * Gestion d'erreur
    */

    pid_t pid[2];

    for (int i = 0; i < 2; ++i)  // Gérer la création de 2 processus fils
    {
        pid[i] = fork();

        if (pid[i] == 0)  // Processus fils
        {
            /*  Init  */

            close(fd[i][WRITE]);
<<<<<<< Updated upstream
            compareImages(argv[1], fd[i][READ]);

            /*  Cleanup  */

            close(fd[i][READ]);  // À la fin, ferme tous les pipes
=======

            /*  Fonctionnalité  */

            int res = compareImages(argv[1], fd[i][READ], dist_shm, path_shm, sem_shm);
            printf("res = %d\n", res);

            /*  Cleanup  */

            cleanup_child(fd[i][READ], dist_shm, path_shm);
            exit(res);
        }
        else if (pid[i] < 0)
        {
            perror("main -> fork");
            exit(1);
        }
>>>>>>> Stashed changes

            if (shmdt(shm_addr) < 0)  // Détacher le shm
            {
                perror("shmdt for some ungodly reason");
                exit(1);
            }
            
            exit(0);
        }
    }

    // Processus parent

    /* 
        Ecrire dans les pipes les chemins d'images un par un
    */

   /*  Init  */

    close(fd[0][READ]);
    close(fd[1][READ]);

    const int fd_writes[2] = { fd[0][WRITE], fd[1][WRITE] };
    feedImagePaths(fd_writes);

<<<<<<< Updated upstream
    /*  Cleanup  */

    cleanup_parent(fd, shm_addr, shm_id);
=======
    printf("B");

    wait(&status[0]);
    wait(&status[1]);

    printf("C");

    /*  Cleanup  */
    
    cleanup_parent(fd, dist_shm, path_shm, sem_shm);
    exit(res);
>>>>>>> Stashed changes
}
