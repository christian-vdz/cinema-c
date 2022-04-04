#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

extern int * attacher_segment_memoire();

/******************************************************************************/
/*
 * Programme principal
*/
/******************************************************************************/
int main(int argc, char *argv[]) {
  unsigned int delais=3;

  int shmid=atoi(argv[1]);
  int semid=atoi(argv[2]);
  char* nom_film=argv[3];

  int *mem;

  /* Attachement du segment de mémoire partagée */
  mem=attacher_segment_memoire(mem, &shmid);
 
  /* Tant qu'il reste des places */
  while (*mem > 0) {
    /* Afficher un message puis attendre 5 secondes */
    printf("Il reste %d places pour le film %s\n", *mem, nom_film);
    sleep (5);
  }

  /* Si le while s'est terminé c'est qu'il n'y a plus de places, on affiche un message et on supprime le sémaphore */
  printf("---- Il n'y a plus de places pour le film %s ----\n", nom_film);
  semctl(semid, 0, IPC_RMID);

  /* Détachement et destruction du segment mémoire */
  shmdt(mem);
  shmctl(shmid, IPC_RMID, NULL);

  return(0);
}
