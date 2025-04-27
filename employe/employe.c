//
// Created by Adam Hassani on 26/04/2025.
//

#include "employe.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>



extern Caisse caisses[NB_CAISSES];
extern int NB_RAYONS_DEFAULT;
extern Rayon* rayons_global;
extern int places_libres;
extern pthread_mutex_t mutex_places;


void *routine_employe(void *arg) {
    Employe *employe = (Employe *) arg;

    while (1) {
        if (employe->mission == AUCUNE) {
            usleep(100000); // petite pause pour éviter de tourner pour rien
            continue;
        }

        if (employe->mission == ENCAISSER_CLIENT) {
            printf("[EMPLOYE %d] Commence à encaisser un client\n", employe->id);

            int client_trouve = 0;

            // Cherche un client dans les caisses
            for (int i = 0; i < NB_CAISSES; i++) {
                pthread_mutex_lock(&caisses[i].mutex);

                if (caisses[i].places_utilisees > 0 && caisses[i].clients[0] != NULL) {
                    Client *client = caisses[i].clients[0];
                    int places_occupees = client->priorite;

                    // Simuler encaissement
                    sleep(2);

                    printf("[EMPLOYE %d] Client %d encaissé à la caisse %d\n", employe->id, client->id, i + 1);

                    // Libérer les places du client
                    for (int p = 0; p < PLACES_MAX; p++) {
                        if (caisses[i].clients[p] == client) {
                            caisses[i].clients[p] = NULL;
                        }
                    }
                    caisses[i].places_utilisees -= places_occupees;

                    pthread_mutex_lock(&mutex_places);
                    if (places_libres < NB_CAISSES) {
                        places_libres++;
                    }
                    pthread_mutex_unlock(&mutex_places);

                    free(client); // IMPORTANT : libérer la mémoire du client
                    client_trouve = 1;

                    pthread_mutex_unlock(&caisses[i].mutex);
                    break;
                }

                pthread_mutex_unlock(&caisses[i].mutex);
            }

            if (!client_trouve) {
                // Si pas trouvé, attendre un peu
                printf("[EMPLOYE %d] Aucun client à encaisser pour l'instant\n", employe->id);
                usleep(500000); // attendre 0,5s
            }

            employe->mission = AUCUNE; // mission terminée
        }

        if (employe->mission == REMPLIR_RAYON) {
            printf("[EMPLOYE %d] Remplissage d'un rayon\n", employe->id);
            sleep(1); // simuler le temps pour remplir

            // Remettre du stock dans un rayon
            for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
                pthread_mutex_lock(&rayons_global[i].mutex);

                if (rayons_global[i].stock <= 0) {
                    if (rayons_global[i].type == LEGER) {
                        rayons_global[i].stock = 10;
                    } else {
                        rayons_global[i].stock = 5;
                    }
                    int cap = rayons_global[i].capacite;
                    for (int j = 0; j < cap; j++) {
                        sem_post(rayons_global[i].semaphore_stock);
                    }
                    printf("[EMPLOYE %d] Rayon '%s' re-rempli à %d articles.\n", employe->id, rayons_global[i].nom,
                           rayons_global[i].stock);
                    pthread_mutex_unlock(&rayons_global[i].mutex);
                    break;
                }

                pthread_mutex_unlock(&rayons_global[i].mutex);
            }

            employe->mission = AUCUNE; // mission terminée
        }
    }
}

void init_employes(Employe employes[]) {
    for (int i = 0; i < 5; i++) {
        employes[i].id = i + 1;
        employes[i].mission = AUCUNE;
        pthread_create(&employes[i].thread, NULL, routine_employe, &employes[i]);
        printf("[EMPLOYE] Initialisation de l'employé %d\n", employes[i].id);
    }
}

const char* mission_str(TypeMission m) {
    switch (m) {
        case AUCUNE:
            return "aucune";
        case ENCAISSER_CLIENT:
            return "CAISSE";
        case REMPLIR_RAYON:
            return "RAYON";
    }
}
