//
// Created by Adam Hassani on 25/04/2025.
//

#include "producteur.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "../client/client.h"

static int phase_soutenur = 1; // 1 pour soutenu, 0 sinon

void* routine_producteur(void* arg) {
    (void)arg;

    int secondes = 0;

    while (1){
        // changement de phase toutes les 30s
        if (secondes >= 30) {
            secondes = 0;
            phase_soutenur = !phase_soutenur;
            printf("\n==== CHANGEMENT DE PHASE ====\n---- Phase %s ----\n\n", phase_soutenur ? "Soutenu" : "Detendue");
        }

        if (phase_soutenur) {
            // 2 clients toutes les 6 secondes
            Client* client1 = routine_creation_client();
            pthread_create(&client1->thread, NULL, routine_arrivee_client, client1);
            Client* client2 = routine_creation_client();
            pthread_create(&client2->thread, NULL, routine_arrivee_client, client2);
            sleep(6);
            secondes+=6;
        } else {
            // 1 client toutes les 10s
            Client* client = routine_creation_client();
            pthread_create(&client->thread, NULL, routine_arrivee_client, client);
            sleep(10);
            secondes+=10;
        }
    }
}


