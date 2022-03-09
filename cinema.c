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

int main(int argc, char *argv[]) {
    pid_t pid_caisse; /* PID du processus caisse  */
    pid_t pid_afficheur; /* PID du processus afficheur */

    int nombre_caisses; /* Pour créer les processus caisse */
    char * nombre_caisses_str; /* Pour conversion du nombre_caisses (int) en chaine */

    int nom_film; /* Pour afficher par le processus affichage */
    char * nom_film_str; /* Pour conversion du nom_film (int) en chaine */

    int nombre_places_cinema; /* Pour écriture dans la shm */
    char * nombre_places_cinema_str; /* Pour conversion du nombre_places_cinema (int) en chaine */

    int shmid; /* ID du segment de mémoire partagé */
    char shmid_str[20]; /* Pour conversion du shmid (int) en chaine */

    int semid; /* ID du sémaphore */
    char semid_str[20]; /* Pour conversion du semid (int) en chaine */

    int i; /* Compteur pour la création des caisses */
    char i_str[10]; /* Pour conversion du i (int) en chaine */    

    int * mem; /* Adresse du segment de mémoire partagée */

    int wstatus;
    
    char param_gnome_terminal[80];

    if (argc != 4) {
        fprintf(stderr, "Usage : %s nombre_caisses nom_film nombre_places\n", argv[0]);
        return(3);
    }

    nombre_caisses_str=argv[1];
    nombre_caisses=atoi(nombre_caisses_str);
    nom_film_str=argv[2];
    nom_film=atoi(nom_film_str);
    nombre_places_cinema_str=argv[3];
    nombre_places_cinema=atoi(nombre_places_cinema_str);

    /* Création sémaphore */
    creer_initialiser_semaphore(CLE_SEM, &semid);

    /* Création segment de mémoire partagé */
    creer_segment_memoire(CLE_SHM, &shmid);

    /* Attachement du segment de mémoire partagée */
    mem=attacher_segment_memoire(mem, &shmid);

    /* Pas besoin de sémaphore on est seul :-) */
    *mem=nombre_places_cinema;

    /* Conversion des shmid et semid  (int) en chaine pour appel programme externe */
    sprintf(shmid_str, "%d", shmid);
    sprintf(semid_str, "%d", semid);

    /* création des fils caisse */
    for (i = 0; i < nombre_caisses; i++) {
        pid_caisse = fork();

        if (pid_caisse == -1) {
            perror("pb fork sur création caisse");
            return(1);
        }
        
        if (pid_caisse == 0) {
            sprintf(i_str, "%d", i);
            execl("caisse", "caisse" , shmid_str, semid_str, i_str, NULL);
        }
    }
    
    /* création du fils afficheur */
    pid_afficheur = fork();
    
    if (pid_afficheur == -1) {
        perror("pb fork sur création afficheur");
        return(1);
    }

    if (pid_afficheur == 0) {     
        execl("afficheur", "afficheur" , shmid_str, semid_str, nom_film_str, NULL);
    }

    if (waitpid(pid_afficheur, &wstatus, WUNTRACED | WCONTINUED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
