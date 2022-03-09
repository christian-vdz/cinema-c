#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

extern int * attacher_segment_memoire();
extern int P();
extern int V();

/******************************************************************************/
/*
 * Fonctions
*/
/******************************************************************************/
bool prendre_places(int *mem, int semid, int nombre_places_demandees, int id_caisse) {
  bool prise_places=false;

  if (nombre_places_demandees <= *mem) {
    /* On protège l'accès à la shm */
    P(semid);

    /* On écrit dans la shm */
    *mem=(*mem - nombre_places_demandees);

    /* On protège l'accès à la shm */
    V(semid);

    /* Les places demandées sont disponibles, on affiche le message et on écrit true dans notre var de retour */
    // printf("%d places sont prises par la caisse %d\n", nombre_places_demandees, id_caisse);
    prise_places = true;
  }

  return prise_places;
}

/******************************************************************************/
/*
 * Programme principal
*/
/******************************************************************************/
int main(int argc, char *argv[]) {
  unsigned int delais=3;

  int shmid=atoi(argv[1]);
  int semid=atoi(argv[2]);
  int id_caisse=atoi(argv[3]);

  int *mem;

  printf("---- On lance la caisse %d ----\n", id_caisse+1);
  
  /* Attachement du segment de mémoire partagée */
  mem=attacher_segment_memoire(mem, &shmid);

  /* Tant qu'il reste des places */
  while(*mem > 0) {
    int nb_places_demandees;

    /* On prend un nombre aléatoire entre 0 et 6, qu'on incrémente de 1 */
    /* Cela tant que le nombre aléatoire est supérieur aux nombres de places disponibles */
    do {
      /* Initialisation du désordre */
      srandom(time(NULL));
      
      nb_places_demandees=(random() % 7)+1;
    }
    while (nb_places_demandees > *mem);
  
    /* On essaie de prendre les places, si le retour de la fonction prendre_places est false on affiche l'erreur */
    if (!prendre_places(mem, semid, nb_places_demandees, id_caisse+1)) {
      perror("pb sur la prise de places");
      return (1);
    }

    srandom(time(NULL) % 3);
    sleep((random() % 2)+1);
  }

  return(0);
}
