#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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

int main(int argc, char* argv[]) 
{
    /*
        Verifier arguments 
    */

    if (argc != 2) 
    {
        printf("Un paramètre requis : chemin vers l'image de base\n");
        exit(1);
    }

    /*
        Créer les pipes 
    */

    int fd[2][2];  // 2 fd, un pour chaque processus fils

    if (pipe(fd[0]) < 0 || pipe(fd[1]) < 0) 
    {
        perror("Création des pipes");
        exit(1);
    }

    /* 
        Créer la mémoire partagée
    */

    int key = 1729;
    int shm_id = shmget(key, SHMSIZE, IPC_CREAT | IPC_EXCL | 0600);

    if (shm_id < 0) 
    {
        perror("shmget");
        exit(1);
    }

    int* shm_addr = shmat(shm_id, NULL, 0);  // Attacher le shm pour tout processus

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
            compareImages(argv[1], fd[i][READ]);

            /*  Cleanup  */

            close(fd[i][READ]);  // À la fin, ferme tous les pipes

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

    /*  Cleanup  */

    cleanup_parent(fd, shm_addr, shm_id);
}
