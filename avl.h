#ifndef AVL_H
#define AVL_H

#include <stdlib.h>

typedef struct Donnees {
    const char *power_plant;
    const char *hvb_station;
    const char *hva_station;
    const char *lv_station;
    const char *company;
    const char *individual;
    long capacity;
    long load;
} Donnees;

typedef struct NoeudAVL {
    const char *cle;
    Donnees **valeurs;
    int taille;
    int capacite;
    int hauteur;
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
} NoeudAVL;

NoeudAVL *inserer_avl(NoeudAVL *racine, const char *cle, Donnees *valeur);
void afficher_avl(NoeudAVL *racine);
void liberer_avl(NoeudAVL *racine);
long somme_avl(NoeudAVL *racine);

#endif
