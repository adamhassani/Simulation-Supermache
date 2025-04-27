#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "employe/employe.h"
#include "producteur/producteur.h"
#include "queue/queue.h"
#include "rayon/rayon.h"
#include "caisse/caisse.h"
#include "manager/manager.h"
#include "superviseur/superviseur.h"

// Déclaration globale
int NB_RAYONS_DEFAULT = 5;
int NB_RAYONS_LEGER_DEFAULT = 3;
int NB_RAYONS_LOURD_DEFAULT = 2;

Rayon* rayons_global = NULL;
FileAttente file_attente;
Employe employes[5];
int places_libres = 3;
pthread_mutex_t mutex_places = PTHREAD_MUTEX_INITIALIZER;
Caisse caisses[NB_CAISSES];

void print_dashboard(void) {
    // 1) Efface l’écran et replace le curseur
    printf("\033[2J\033[H");

    // 2) En-tête
    time_t t = time(NULL);
    printf("=== Supermarché — %s", ctime(&t));

    // 3) Rayons
    printf("\nRayons :\n");
    for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
        int stock = get_stock_rayon(&rayons_global[i]);
        printf("  %-15s : %2d articles\n",
               rayons_global[i].nom, stock);
    }

    // 4) File d’attente
    int qlen = file_taille(&file_attente);
    printf("\nFile d’attente : %d client(s)\n", qlen);

    // 5) Caisses
    printf("\nCaisses :\n");
    for (int i = 0; i < NB_CAISSES; i++) {
        pthread_mutex_lock(&caisses[i].mutex);
        printf("  Caisse %d : %d/%d places utilisées\n",
               i+1, caisses[i].places_utilisees, PLACES_MAX);
        pthread_mutex_unlock(&caisses[i].mutex);
    }

    // 6) Employés
    printf("\nEmployés :\n");
    for (int i = 0; i < 5; i++) {
        printf("  Employé %d : %s\n",
               employes[i].id,
               mission_str(employes[i].mission));
    }

    fflush(stdout);
}

void* routine_afficheur(void* arg) {
    (void)arg;
    while (1) {
        print_dashboard();
        sleep(1);
    }
}

int main(void) {
    // 1. Initialisation générale
    srandom(time(NULL));
    printf("[MAIN] Démarrage du programme...\n");

    // 2. Initialisation des ressources
    init_file(&file_attente);
    init_caisse(caisses);
    init_employes(employes);

    // 3. Initialisation des rayons
    Rayon rayons[NB_RAYONS_DEFAULT];
    init_rayons(rayons, NB_RAYONS_LEGER_DEFAULT, NB_RAYONS_LOURD_DEFAULT);
    rayons_global = rayons;

    // 4. Démarrage des rôles de gestion
    init_manager();
    init_superviseur();

    // 5. Démarrage des producteurs et afficheur
    pthread_t thread_producteur, thread_afficheur;
    pthread_create(&thread_producteur, NULL, routine_producteur, NULL);
    pthread_create(&thread_afficheur, NULL, routine_afficheur, NULL);

    // 6. Synchronisation principale
    pthread_join(thread_producteur, NULL);

    // 7. Nettoyage final (jamais atteint ici car boucle infinie mais bon pour la forme)
    for (int i = 0; i < NB_RAYONS_DEFAULT; i++) {
        sem_close(rayons[i].semaphore_stock);
        sem_unlink(rayons[i].nom_semaphore);
        pthread_mutex_destroy(&rayons[i].mutex);
    }

    return 0;
}
