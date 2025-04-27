//
// Created by Adam Hassani on 25/04/2025.
//

#include "client.h"

#include <stdio.h>
#include <stdlib.h>

#include "../caisse/caisse.h"
#include "../queue/queue.h"



static int client_id = 0;
extern Rayon *rayons_global;
extern int NB_RAYONS_DEFAULT;
extern FileAttente file_attente;


// Creation du client lors de son entree en magasin
Client *routine_creation_client(void) {
    Client *client = malloc(sizeof(Client));
    client->id = ++client_id;
    client->priorite = (Priorite) ((random() % 3) + 1); // attribution d'une priorite de maniere aleatoire
    client->poids_caddie = 0;
    client->nb_articles_legers = 0;
    client->nb_articles_lourds = 0;
    return client;
}

// Parcours du client
void *routine_arrivee_client(void *arg) {
    Client *client = arg;

    ShoppingArgs *args = malloc(sizeof(ShoppingArgs));
    args->client = client;
    args->rayons = rayons_global;
    args->nb_rayons = NB_RAYONS_DEFAULT;


    // demarrer le shopping
    routine_shopping_client(args);

    pthread_exit(NULL);
}

void *routine_shopping_client(void *arg) {
    ShoppingArgs *args = arg;
    Client *c = args->client;
    int N = args->nb_rayons;

    // 1) Init client
    c->poids_caddie = 0;
    c->nb_articles_lourds = 0;
    c->nb_articles_legers = 0;

    // 2) Prendre 1 lourd au début
    while (1) {
        int i = random() % N;
        if (args->rayons[i].type == LOURD) {
            sem_wait(args->rayons[i].semaphore_stock);
            pthread_mutex_lock(&args->rayons[i].mutex);
            args->rayons[i].stock--;
            pthread_mutex_unlock(&args->rayons[i].mutex);
            c->poids_caddie += LOURD;
            c->nb_articles_lourds++;
            break;
        }
    }

    // 3) Remplir aléatoirement jusqu'à ≥20kg sans dépasser 22kg
    while (c->poids_caddie < 20) {
        // a) Déterminer les types possibles
        TypeRayon possibles[2];
        int count = 0;
        if (c->poids_caddie + LOURD <= 22)
            possibles[count++] = LOURD;
        if (c->poids_caddie + LEGER <= 22)
            possibles[count++] = LEGER;

        // b) Choisir un type au hasard
        TypeRayon choix = possibles[random() % count];

        // c) Choisir un rayon de ce type au hasard
        int idx;
        while (1) {
            idx = random() % N;
            if (args->rayons[idx].type == choix) {
                sem_wait(args->rayons[idx].semaphore_stock);
                pthread_mutex_lock(&args->rayons[idx].mutex);
                args->rayons[idx].stock--;
                pthread_mutex_unlock(&args->rayons[idx].mutex);
                break;
            }
        }

        // d) Mettre à jour le caddie
        c->poids_caddie += choix;
        if (choix == LOURD) c->nb_articles_lourds++;
        else c->nb_articles_legers++;
    }


    ajouter_client(&file_attente, c); // ajouter le client dans la file d'attente

    return NULL;
}


