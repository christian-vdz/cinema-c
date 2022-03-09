
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "shm_const.h"

extern int creer_initialiser_semaphore();
extern int * attacher_segment_memoire();
extern int P();
extern int V();

extern int creer_segment_memoire();

int main(int argc, char *argv[])
{
    pid_t pid_caisse; /* no du processus du processus caisse  */

    int code_retour_fin_caisse;
    int code_retour_fin_sortie;

    int nombre_caisses; /* Pour écriture dans la shm */
    char * nombre_caisses_str; /* Pour conversion du semid (int) en chaine */

    int nom_film; /* Pour écriture dans la shm */
    char * nom_film_str; /* Pour conversion du semid (int) en chaine */

    int nombre_places_cinema; /* Pour écriture dans la shm */
    char * nombre_places_cinema_str; /* Pour conversion du semid (int) en chaine */

    int shmid; /* Id du segment de mémoire partagé */
    int semid; /* Id du sémaphore */

    char shmid_str[20]; /* Pour conversion du shmid (int) en chaine */
    char semid_str[20]; /* Pour conversion du semid (int) en chaine */

    int * mem; /* Adresse du segment de mémoire partagée */

    char param_gnome_terminal[80];

    if ( argc != 4 ) {
        fprintf(stderr, "Usage : %s nombre_caisses nom_film nombre_places\n", argv[0]);
        return(3);
    }

    nombre_caisses_str=argv[1];
    nombre_caisses=atoi(nombre_caisses_str);
    nom_film_str=argv[2];
    nom_film=atoi(nom_film_str);
    nombre_places_cinema_str=argv[3];
    nombre_places_cinema=atoi(nombre_places_cinema_str);

 /* Avant de créer les fils :
    * - on crée le semaphore
    * - on crée le segment de mémoire partagé
    * - on s'attache le segment de mémoire partagé
    * - on initialise le noombre de places de cinema libres dans la shm
    */

    /* Création sémaphore */
    creer_initialiser_semaphore(CLE_SEM, &semid);

    /* Création segment de mémoire partagé */
    creer_segment_memoire(CLE_SHM, &shmid);

/*
printf("DEBUG : cinema : shmid=%d\n", shmid);
*/

    /* Attachement du segment de mémoire partagée */
    mem=attacher_segment_memoire(mem, &shmid);

    /* Pas besoin de sémaphore on est seul :-) */
    *mem=nombre_places_cinema;

    /* Conversion des shmid et semid  (int) en chaine pour appel programme externe */
    sprintf(shmid_str, "%d", shmid);
    sprintf(semid_str, "%d", semid);

    /* création du fils entrée */
    pid_caisse = fork();

    if (pid_caisse == -1) {
        /* Erreur */
        perror("pb fork sur création entrée");
        return(1);
    }

    if (pid_caisse == 0) {
        /*
        printf("Je suis le fils caisse, je vais faire execl dans 10s shmid_str=%s, semid_str=%s\n", shmid_str, semid_str);
        */
        execl("caisse", "caisse", shmid_str, semid_str, NULL);
    }
}
