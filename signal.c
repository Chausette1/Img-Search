#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "signal.h"

void handler_sigint(int signal_recu) {
    if(signal_recu == SIGINT){
        while (1) { //boucle infini avec un sleep car si on est dans un états critique on ne peut pas SIGTERM
            if (kill(0, SIGTERM) == 0) {
                wait(NULL);
                wait(NULL);
                printf("SIGINT terminer proprement.\n");
                exit(0);
            } else {
                sleep(1);
            }
        }
    }
}

void hide_sigterm() {
    signal(SIGTERM, SIG_IGN);
}

void unhide_sigterm() {
    signal(SIGTERM, SIGTERM);
}

void handler_sigterm(int signal_recu) {
    if (signal_recu == SIGTERM) {  // Is always SIGTERM lol
        close(fd[0][READ]);
        close(fd[1][READ]);
    }
}