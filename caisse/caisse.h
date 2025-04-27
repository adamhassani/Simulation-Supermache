//
// Created by Adam Hassani on 27/04/2025.
//

#ifndef CAISSE_H
#define CAISSE_H

#include <pthread.h>
#include "../client/client.h"

#define NB_CAISSES 3
#define PLACES_MAX 4


typedef struct Caisse {
    Client* clients[PLACES_MAX];
    int places_utilisees;
    pthread_mutex_t mutex;
} Caisse;


void init_caisse(Caisse caisses[]);

void afficher_caisse(Caisse caisse);

int peut_accueillir_client(Client* client);
int placer_client(Client* client);

#endif //CAISSE_H


