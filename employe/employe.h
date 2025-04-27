//
// Created by Adam Hassani on 26/04/2025.
//

#ifndef EMPLOYE_H
#define EMPLOYE_H
#include <sys/_pthread/_pthread_t.h>
#include "../client/client.h"
#include "../caisse/caisse.h"

typedef enum TypeMission {
    AUCUNE,
    ENCAISSER_CLIENT,
    REMPLIR_RAYON,
} TypeMission;

typedef struct Employe {
    int id;
    pthread_t thread;
    TypeMission mission;
} Employe;

void init_employes(Employe employes[]);

void* routine_employe(void* arg);

#endif //EMPLOYE_H
