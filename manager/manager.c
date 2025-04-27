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
    // thread du manager
    pthread_create(&thread_manager, NULL, routine_manager, NULL);
    printf("[MANAGER] Initialisation du manager\n");
}

void *routine_manager(void *arg) {
    (void) arg;

    while (1) {
        // boucle infini pour surveiller le magasin
        int action_effectuee = 0;

        // d'abord on regarde si un rayon est vide, car prioritaire
        for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
            pthread_mutex_lock(&rayons_global[i].mutex); // acces protege pour recuperer un stock juste

            if (rayons_global[i].stock == 0) {
                // on marque stock a reremplir
                rayons_global[i].stock = -rayons_global[i].capacite;
                pthread_mutex_unlock(&rayons_global[i].mutex);

                // recherche d'un employe pour reremplir le rayon
                for (int j = 0; j < 5; j++) {
                    if (employes[j].mission == AUCUNE) {
                        employes[j].mission = REMPLIR_RAYON;
                        printf("[MANAGER] Envoie de l'employé %d remplir %s\n", employes[j].id, rayons_global[i].nom);
                        action_effectuee = 1;
                        break;
                    }
                }
                // si on a reussi a assigner, on quitte la boucle
                if (action_effectuee) break;
            } else {
                // si rayon pas vide, on passe au suivant en debloquant le mutex
                pthread_mutex_unlock(&rayons_global[i].mutex);
            }
        }

        // passer a la verification de clients a encaisser
        if (!action_effectuee && !file_est_vide(&file_attente)) {
            // on reserve une place en caisse
            pthread_mutex_lock(&mutex_places);

            if (places_libres > 0) {
                places_libres--;
                pthread_mutex_unlock(&mutex_places);

                // on assigne un employe pour encaisser un client
                for (int i = 0; i < 5; i++) {
                    if (employes[i].mission == AUCUNE) {
                        employes[i].mission = ENCAISSER_CLIENT;
                        printf("[MANAGER] Assigne Employé %d pour encaisser un client (ouvre une caisse)\n",
                               employes[i].id);
                        break;
                    }
                }
            } else {
                // si plus de place, on debloque
                pthread_mutex_unlock(&mutex_places);
            }
        }
        usleep(500000); // petite pause
    }
}
