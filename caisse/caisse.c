//
// Created by Adam Hassani on 27/04/2025.
//

#include "caisse.h"
#include <stdio.h>


extern Caisse caisses[NB_CAISSES];

void init_caisse(Caisse caisses[]) {
    for (int i = 0; i < NB_CAISSES; i++) {
        for (int j = 0; j < PLACES_MAX; j++) {
            caisses[i].clients[j] = NULL; // initialise toutes les places a NULL
        }
        caisses[i].places_utilisees = 0;
        pthread_mutex_init(&caisses[i].mutex, NULL);
        printf("[CAISSE] Initialisation de la caisse %d\n", i + 1);
    }
}

void afficher_caisses(Caisse caisses[]) {
    for (int i = 0; i < NB_CAISSES; i++) {
    }
}

int peut_accueillir_client(Client *client) {
    int places_necessaires = client->priorite;

    for (int i = 0; i < NB_CAISSES; i++) {
        pthread_mutex_lock(&caisses[i].mutex);

        int places_restantes = PLACES_MAX - caisses[i].places_utilisees;

        pthread_mutex_unlock(&caisses[i].mutex);

        if (places_restantes >= places_necessaires) {
            return 1; // il y a assez de place pour assigner le client a une caisse
        }
    }
    return 0;
}

int placer_client(Client *client) {
    int places_necessaires = client->priorite;

    for (int i = 0; i < NB_CAISSES; i++) {
        pthread_mutex_lock(&caisses[i].mutex);

        int places_restantes = PLACES_MAX - caisses[i].places_utilisees;
        if (places_restantes >= places_necessaires) {
            for (int j = 0; j < places_necessaires; j++) {
                caisses[i].clients[caisses[i].places_utilisees + j] = client;
            }

            caisses[i].places_utilisees += places_necessaires;
            pthread_mutex_unlock(&caisses[i].mutex);


            return 1; //client place avec succes
        }
        pthread_mutex_unlock(&caisses[i].mutex);
    }
    return 0; //client n'a pas pu etre place
}
