//
// Created by Adam Hassani on 25/04/2025.
//

#ifndef RAYON_H
#define RAYON_H
#include <pthread.h>
#include <sys/semaphore.h>

typedef enum TypeRayon { // association de type d'article et poids
    LEGER = 2,
    LOURD = 8,
} TypeRayon;

typedef struct Rayon {
    char nom[50];
    char nom_semaphore[64]; // obligatoire sur macos
    TypeRayon type;
    int capacite;
    int stock;
    pthread_mutex_t mutex; // acces protege aux rayons
    sem_t* semaphore_stock;
} Rayon;

void init_rayons(Rayon rayons[], int nb_rayons_leger, int nb_rayons_lourd);
int get_stock_rayon(Rayon *rayon);


#endif //RAYON_H
