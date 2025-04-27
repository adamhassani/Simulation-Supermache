//
// Created by Adam Hassani on 25/04/2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <pthread.h>
#include "../rayon/rayon.h"

typedef enum Priorite {
    ELEVE = 3,
    MOYEN = 2,
    BAS = 1
} Priorite;

typedef struct Client {
    int id;
    Priorite priorite;
    int poids_caddie;
    int nb_articles_lourds;
    int nb_articles_legers;
    pthread_t thread;
} Client;

// struct relatif a chaque client pour le shopping
typedef struct ShoppingArgs {
    Client* client;
    Rayon* rayons;
    int nb_rayons;
} ShoppingArgs;

void* routine_arrivee_client(void*);
Client* routine_creation_client(void);
void* routine_shopping_client(void* arg); // comportement du client lors de son parcours des rayons

#endif //CLIENT_H
