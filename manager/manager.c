//
// Created by Adam Hassani on 27/04/2025.
//

#include "manager.h"

#include <stdio.h>
#include <unistd.h>

#include "../queue/queue.h"
#include "../employe/employe.h"

extern FileAttente file_attente;
extern Employe employes[5];
extern int NB_RAYONS_DEFAULT;
extern Rayon *rayons_global;

extern int places_libres;
extern pthread_mutex_t mutex_places;

pthread_t thread_manager;

void init_manager() {
    pthread_create(&thread_manager, NULL, routine_manager, NULL);
    printf("[MANAGER] Initialisation du manager\n");
}

void *routine_manager(void *arg) {
    (void) arg;

    while (1) {
        int action_effectuee = 0;

        // vérifier le remplissage des rayons
        for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
            pthread_mutex_lock(&rayons_global[i].mutex);

            if (rayons_global[i].stock == 0) {
                pthread_mutex_unlock(&rayons_global[i].mutex);

                for (int j = 0; j < 5; j++) {
                    if (employes[j].mission == AUCUNE) {
                        employes[j].mission = REMPLIR_RAYON;
                        printf("[MANAGER] Envoie de l'employé %d remplir %s\n", employes[j].id, rayons_global[i].nom);
                        action_effectuee = 1;
                        break;
                    }
                }
                if (action_effectuee) break;
            } else {
                pthread_mutex_unlock(&rayons_global[i].mutex);
            }
        }

        // vérifier l'encaissement des clients
        if (!action_effectuee && !file_est_vide(&file_attente)) {
            pthread_mutex_lock(&mutex_places);

            if (places_libres > 0) {
                places_libres--; // on "réserve" une caisse
                pthread_mutex_unlock(&mutex_places);

                for (int i = 0; i < 5; i++) {
                    if (employes[i].mission == AUCUNE) {
                        employes[i].mission = ENCAISSER_CLIENT;
                        printf("[MANAGER] Assigne Employé %d pour encaisser un client (ouvre une caisse)\n",
                               employes[i].id);
                        break;
                    }
                }
            } else {
                pthread_mutex_unlock(&mutex_places);
            }
        }
        usleep(100000);
    }
}
