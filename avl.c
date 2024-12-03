#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

// Fonction pour calculer la hauteur d'un nœud
int hauteur(NoeudAVL *noeud) {
    return (noeud == NULL) ? 0 : noeud->hauteur;
}

// Fonction pour créer un nouveau nœud
NoeudAVL *nouveau_noeud(const char *cle, int valeur) {
    NoeudAVL *noeud = (NoeudAVL *)malloc(sizeof(NoeudAVL));
    noeud->cle = strdup(cle);
    noeud->valeur = valeur;
    noeud->gauche = noeud->droit = NULL;
    noeud->hauteur = 1; // Hauteur initiale
    return noeud;
}

// Fonction pour insérer un nœud dans l'arbre AVL
NoeudAVL *inserer_avl(NoeudAVL *noeud, const char *cle, int valeur) {
    if (noeud == NULL) return nouveau_noeud(cle, valeur);

    // Comparer les clés pour déterminer la position
    if (strcmp(cle, noeud->cle) < 0)
        noeud->gauche = inserer_avl(noeud->gauche, cle, valeur);
    else if (strcmp(cle, noeud->cle) > 0)
        noeud->droit = inserer_avl(noeud->droit, cle, valeur);
    else {
        noeud->valeur += valeur; // Mise à jour de la valeur si la clé existe déjà
        return noeud;
    }

    // Mettre à jour la hauteur du nœud
    noeud->hauteur = 1 + (hauteur(noeud->gauche) > hauteur(noeud->droit) ? hauteur(noeud->gauche) : hauteur(noeud->droit));

    // (Nous ajouterons les rotations pour équilibrer l'arbre plus tard)
    return noeud;
}

// Fonction pour afficher l'arbre AVL (parcours en ordre)
void afficher_avl(NoeudAVL *racine) {
    if (racine != NULL) {
        afficher_avl(racine->gauche);
        printf("Clé : %s, Valeur : %d\n", racine->cle, racine->valeur);
        afficher_avl(racine->droit);
    }
}


// Libérer la mémoire de l'arbre AVL
void liberer_avl(NoeudAVL *racine) {
    if (racine != NULL) {
        liberer_avl(racine->gauche);
        liberer_avl(racine->droit);
        free(racine->cle);
        free(racine);
    }
}
