#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include "../client/client.h"

typedef struct NodeClient {
    Client* client;
    struct NodeClient* suivant;
} NodeClient;

typedef struct FileAttente{
    NodeClient* debut;
    NodeClient* fin;
    pthread_mutex_t mutex;
} FileAttente;

void init_file(FileAttente* file);
void ajouter_client(FileAttente* file, Client* client);
Client* retirer_client(FileAttente* file);
int file_est_vide(FileAttente* file);
Client* premier_client(FileAttente* file);

#endif // QUEUE_H
