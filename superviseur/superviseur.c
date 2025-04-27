//
// Created by Adam Hassani on 27/04/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "superviseur.h"

#include <unistd.h>

#include "../queue/queue.h"
#include "../client/client.h"
#include "../caisse/caisse.h"


extern FileAttente file_attente;
extern Caisse caisses[NB_CAISSES];

pthread_t thread_superviseur;

void init_superviseur() {
    pthread_create(&thread_superviseur, NULL, routine_superviseur, NULL);
    printf("[SUPERVISEUR] Initialisation du superviseur\n");
}

void* routine_superviseur(void* arg) {
    (void)arg;

    while (1) {
        if (!file_est_vide(&file_attente)) {
            Client* client = premier_client(&file_attente); // 1. Regarder premier client

            if (client != NULL && peut_accueillir_client(client)) { // 2. Vérifier si on peut l'accueillir
                Client* client_retire = retirer_client(&file_attente); // 3. Retirer seulement si OK

                if (client_retire != NULL) {
                    placer_client(client_retire); // 4. Placer dans une caisse ouverte
                }
            }
        }

        usleep(100000); // Pause pour ne pas saturer le CPU
    }
}
