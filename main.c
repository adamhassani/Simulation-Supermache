// created by adam hassani on 27/04/2025

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "employe/employe.h"
#include "producteur/producteur.h"
#include "queue/queue.h"
#include "rayon/rayon.h"
#include "caisse/caisse.h"
#include "manager/manager.h"
#include "superviseur/superviseur.h"

// declaration des configs par defaut
int NB_RAYONS_DEFAULT = 5;
int NB_RAYONS_LEGER_DEFAULT = 3;
int NB_RAYONS_LOURD_DEFAULT = 2;

// globals partages
Rayon *rayons_global = NULL;
FileAttente file_attente;
Employe employes[5];
int places_libres = 3;
pthread_mutex_t mutex_places = PTHREAD_MUTEX_INITIALIZER;
Caisse caisses[NB_CAISSES];

// thread qui affiche regulierement l'etat des caisses
void *routine_afficheur(void *arg) {
    (void) arg;
    while (1) {
        printf("\n========= ETAT DES CAISSES =========\n");
        for (int i = 0; i < NB_CAISSES; i++) {
            // lock pour lire sans conflit
            pthread_mutex_lock(&caisses[i].mutex);
            printf("Caisse %d : %d places utilisees / %d\n",
                   i + 1, caisses[i].places_utilisees, PLACES_MAX);
            pthread_mutex_unlock(&caisses[i].mutex);
        }
        printf("=====================================\n\n");
        sleep(5); // pause 5s
    }
    return NULL;
}

int main(void) {
    // 1. init generale
    srandom(time(NULL)); // base aleatoire
    printf("[MAIN] demarrage du programme...\n");

    // 2. init file, caisses, employes
    init_file(&file_attente); // liste d'attente vide
    init_caisse(caisses); // caisses a zero
    init_employes(employes); // threads employes

    // 3. init rayons
    Rayon rayons[NB_RAYONS_DEFAULT];
    init_rayons(rayons,NB_RAYONS_LEGER_DEFAULT,NB_RAYONS_LOURD_DEFAULT);
    rayons_global = rayons; // pointeur global

    // 4. demarrage manager et superviseur
    init_manager(); // thread manager lance
    init_superviseur(); // thread superviseur lance

    // 5. demarrage du producteur et de l'afficheur
    pthread_t thread_producteur, thread_afficheur;
    pthread_create(&thread_producteur,NULL,routine_producteur,NULL);
    pthread_create(&thread_afficheur,NULL,routine_afficheur,NULL);

    // 6. on attend la fin du producteur (jamais car boucle infinie)
    pthread_join(thread_producteur, NULL);

    // 7. nettoyage (jamais atteint normalement)
    for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
        sem_close(rayons[i].semaphore_stock);
        sem_unlink(rayons[i].nom_semaphore);
        pthread_mutex_destroy(&rayons[i].mutex);
    }

    return 0;
}
