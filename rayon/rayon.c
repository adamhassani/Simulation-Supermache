//
// Created by Adam Hassani on 25/04/2025.
//

#include "rayon.h"

#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>

void init_rayons(Rayon rayons[], int nb_rayons_leger, int nb_rayons_lourd) {
    for (int i = 0; i < nb_rayons_leger; i++) {
        sprintf(rayons[i].nom, "Rayon leger %d", i+1);
        rayons[i].type = LEGER;
        rayons[i].capacite = 10;

        // nommage de la semaphore. obligatoire sur MacOs
        sprintf(rayons[i].nom_semaphore, "/Rayon_leger_%d", i+1);

        sem_unlink(rayons[i].nom_semaphore); // securite
        rayons[i].semaphore_stock = sem_open(rayons[i].nom_semaphore, O_CREAT | O_EXCL, 0644, rayons[i].capacite);
        rayons[i].stock = rayons[i].capacite;
        pthread_mutex_init(&rayons[i].mutex, NULL);
    }
    for (int i = 0; i < nb_rayons_lourd; i++) {
        sprintf(rayons[i+nb_rayons_leger].nom, "Rayon lourd %d", i+1);
        rayons[i+nb_rayons_leger].type = LOURD;
        rayons[i+nb_rayons_leger].capacite = 5;

        // nom de la semaphore
        sprintf(rayons[i+nb_rayons_leger].nom_semaphore, "/Rayon_lourd_%d", i+1);

        // pas de sem_init. nommage obligatoire sous macos
        sem_unlink(rayons[i+nb_rayons_leger].nom_semaphore); // securite
        rayons[i+nb_rayons_leger].semaphore_stock = sem_open(rayons[i+nb_rayons_leger].nom_semaphore, O_CREAT | O_EXCL, 0644, rayons[i+nb_rayons_leger].capacite);
        rayons[i+nb_rayons_leger].stock = rayons[i+nb_rayons_leger].capacite;
        pthread_mutex_init(&rayons[i+nb_rayons_leger].mutex, NULL);
    }
}

int get_stock_rayon(Rayon *r) {
    pthread_mutex_lock(&r->mutex); // acces concurrent partage

    int count = 0;
    while (sem_trywait(r->semaphore_stock) == 0) {
        count++;
    }
    for (int i = 0; i < count; i++) {
        sem_post(r->semaphore_stock);
    }

    pthread_mutex_unlock(&r->mutex);
    return count;
}

