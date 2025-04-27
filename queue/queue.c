#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

void init_file(FileAttente *file) {
    // liste chainee vide
    file->debut = NULL;
    file->fin = NULL;
    pthread_mutex_init(&file->mutex, NULL);
    printf("[FILE] Initialisation de la file d'attente\n");
}

void ajouter_client(FileAttente *file, Client *client) {
    NodeClient *nouveau = malloc(sizeof(NodeClient));
    nouveau->client = client;
    nouveau->suivant = NULL;

    pthread_mutex_lock(&file->mutex); // bloque l'acces a la queue pour eviter que 2 clients entrent en meme temps

    if (file->fin == NULL) {
        // si file vide
        file->debut = nouveau;
        file->fin = nouveau;
    } else {
        // sinon ajout apres dernier element
        file->fin->suivant = nouveau;
        file->fin = nouveau;
    }

    pthread_mutex_unlock(&file->mutex);

    printf("[FILE] Client %d ajouté à la file d'attente (Priorité %d - %d kg)\n",
           client->id, client->priorite, client->poids_caddie);
}

Client *retirer_client(FileAttente *file) {
    pthread_mutex_lock(&file->mutex); // protection de la donnee partager

    Client *client = NULL;
    if (file->debut != NULL) {
        // verifier qu'il y a bien un client dans la queue
        NodeClient *temp = file->debut;
        client = temp->client;
        file->debut = temp->suivant;

        if (file->debut == NULL) {
            file->fin = NULL; // la file devient vide
        }

        free(temp);

    }

    pthread_mutex_unlock(&file->mutex);
    return client;
}

int file_est_vide(FileAttente *file) {
    pthread_mutex_lock(&file->mutex);
    int vide = (file->debut == NULL);
    pthread_mutex_unlock(&file->mutex);
    return vide;
}

Client *premier_client(FileAttente *file) {
    pthread_mutex_lock(&file->mutex);
    Client *client = NULL;
    if (file->debut != NULL) {
        client = file->debut->client;
    }
    pthread_mutex_unlock(&file->mutex);
    return client;
}
