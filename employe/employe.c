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
extern Rayon *rayons_global;
extern int places_libres;
extern pthread_mutex_t mutex_places;


void *routine_employe(void *arg) {
    Employe *employe = arg;

    while (1) {
        if (employe->mission == AUCUNE) {
            usleep(200000); // petite pause pour éviter de tourner pour rien
            continue;
        }

        if (employe->mission == ENCAISSER_CLIENT) {
            int client_trouve = 0;
            int start = employe->id - 1; // chaque employe demarre à son propre ID
            for (int d = 0; d < NB_CAISSES && !client_trouve; d++) {
                int i = (start + d) % NB_CAISSES; // on balaye les caisses en cycle
                pthread_mutex_lock(&caisses[i].mutex);

                if (caisses[i].places_utilisees > 0) {
                    // 1) trouver la premiere case non-NULL
                    int idx = -1;
                    for (int p = 0; p < PLACES_MAX; p++) {
                        if (caisses[i].clients[p] != NULL) {
                            idx = p;
                            break;
                        }
                    }

                    if (idx >= 0) {
                        Client *client = caisses[i].clients[idx];
                        int places_occupees = client->priorite;

                        sleep(2); // simulation de l'encaissement
                        printf("[EMPLOYE %d] Client %d encaissé à la caisse %d\n",
                               employe->id, client->id, i + 1);

                        // 2) liberer les cases et compacter
                        // on décale tout vers l'avant de places_occupees
                        int tail = caisses[i].places_utilisees;
                        for (int k = idx; k + places_occupees < tail; k++) {
                            caisses[i].clients[k] = caisses[i].clients[k + places_occupees];
                        }
                        // remplir les dernieres cases à NULL
                        for (int k = tail - places_occupees; k < tail; k++) {
                            caisses[i].clients[k] = NULL;
                        }
                        caisses[i].places_utilisees -= places_occupees;

                        // mettre à jour places_libres
                        pthread_mutex_lock(&mutex_places);
                        if (places_libres < NB_CAISSES) places_libres++;
                        pthread_mutex_unlock(&mutex_places);

                        free(client);
                        client_trouve = 1;
                    }
                }

                pthread_mutex_unlock(&caisses[i].mutex);
            }

            if (!client_trouve) {
                // pas de client trouve, on reessaie plus tard
                usleep(500000);
            }
            employe->mission = AUCUNE;
        }

        if (employe->mission == REMPLIR_RAYON) {
            printf("[EMPLOYE %d] Remplissage d'un rayon\n", employe->id);
            sleep(1); // simuler le temps pour remplir

            // remplissage rayon
            for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
                pthread_mutex_lock(&rayons_global[i].mutex); // bloque l'acces au rayon pour verifier les stock

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

            employe->mission = AUCUNE; // mission terminee
        }
    }
}

void init_employes(Employe employes[]) {  // initialisaiton de l'employe
    for (int i = 0; i < 5; i++) {
        employes[i].id = i + 1;
        employes[i].mission = AUCUNE;
        pthread_create(&employes[i].thread, NULL, routine_employe, &employes[i]);
        printf("[EMPLOYE] Initialisation de l'employé %d\n", employes[i].id);
    }
}
