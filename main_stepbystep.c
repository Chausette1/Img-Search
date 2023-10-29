

int main(int argc, char* argv[]) {
    /*
        Verifier arguments 
    */

    // ...

    /*
        Créer les pipes 
    */

    // ...

    /* 
        Créer la mémoire partagée
    */

    // ...

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

    // ...

    /* 
        Ecrire dans les pipes (en alternant peut-être) les chemins d'images un par un (for loop ?)
    */
}
