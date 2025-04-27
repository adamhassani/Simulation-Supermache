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

// Déclaration globale
int NB_RAYONS_DEFAULT = 5;
int NB_RAYONS_LEGER_DEFAULT = 3;
int NB_RAYONS_LOURD_DEFAULT = 2;

Rayon* rayons_global = NULL;
FileAttente file_attente;
Employe employes[5];
int places_libres = 3;
pthread_mutex_t mutex_places = PTHREAD_MUTEX_INITIALIZER;
Caisse caisses[NB_CAISSES];

// Routine d'affichage automatique de l'état des caisses
void* routine_afficheur(void* arg) {
    (void)arg;
    while (1) {
        printf("\n========= ÉTAT DES CAISSES =========\n");
        for (int i = 0; i < NB_CAISSES; i++) {
            pthread_mutex_lock(&caisses[i].mutex);
            printf("Caisse %d : %d places utilisées / %d\n", i + 1, caisses[i].places_utilisees, PLACES_MAX);
            pthread_mutex_unlock(&caisses[i].mutex);
        }
        printf("=====================================\n\n");
        sleep(5); // Toutes les 5 secondes
    }
}

int main(void) {
    // 1. Initialisation générale
    srandom(time(NULL));
    printf("[MAIN] Démarrage du programme...\n");

    // 2. Initialisation des ressources
    init_file(&file_attente);
    init_caisse(caisses);
    init_employes(employes);

    // 3. Initialisation des rayons
    Rayon rayons[NB_RAYONS_DEFAULT];
    init_rayons(rayons, NB_RAYONS_LEGER_DEFAULT, NB_RAYONS_LOURD_DEFAULT);
    rayons_global = rayons;

    // 4. Démarrage des rôles de gestion
    init_manager();
    init_superviseur();

    // 5. Démarrage des producteurs et afficheur
    pthread_t thread_producteur, thread_afficheur;
    pthread_create(&thread_producteur, NULL, routine_producteur, NULL);
    pthread_create(&thread_afficheur, NULL, routine_afficheur, NULL);

    // 6. Synchronisation principale
    pthread_join(thread_producteur, NULL);

    // 7. Nettoyage final (jamais atteint ici car boucle infinie mais bon pour la forme)
    for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
        sem_close(rayons[i].semaphore_stock);
        sem_unlink(rayons[i].nom_semaphore);
        pthread_mutex_destroy(&rayons[i].mutex);
    }

    return 0;
}
