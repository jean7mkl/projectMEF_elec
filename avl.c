#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "avl.h"

#define DEBUG 1 // Activez ou désactivez les messages de débogage

static int hauteur(NoeudAVL *N) {
    if (N == NULL) return 0;
    return N->hauteur;
}

static int max(int a, int b) {
    return (a > b) ? a : b;
}


static NoeudAVL *nouveau_noeud(const char *cle, Donnees *valeur) {
    NoeudAVL *noeud = (NoeudAVL *)malloc(sizeof(NoeudAVL));
    if (!noeud) {
        fprintf(stderr, "Erreur : allocation mémoire échouée pour le nœud\n");
        exit(EXIT_FAILURE);
    }
    noeud->cle = strdup(cle);
    noeud->taille = 1;
    noeud->capacite = 2;
    noeud->valeurs = (Donnees **)malloc(noeud->capacite * sizeof(Donnees *));
    if (!noeud->valeurs) {
        fprintf(stderr, "Erreur : allocation mémoire échouée pour les valeurs\n");
        exit(EXIT_FAILURE);
    }
    noeud->valeurs[0] = valeur;
    noeud->hauteur = 1;
    noeud->gauche = NULL;
    noeud->droite = NULL;
    return noeud;
}


static NoeudAVL *rotation_droite(NoeudAVL *y) {
    NoeudAVL *x = y->gauche;
    NoeudAVL *T2 = x->droite;

    x->droite = y;
    y->gauche = T2;

    y->hauteur = max(hauteur(y->gauche), hauteur(y->droite)) + 1;
    x->hauteur = max(hauteur(x->gauche), hauteur(x->droite)) + 1;

    return x;
}

static NoeudAVL *rotation_gauche(NoeudAVL *x) {
    NoeudAVL *y = x->droite;
    NoeudAVL *T2 = y->gauche;

    y->gauche = x;
    x->droite = T2;

    x->hauteur = max(hauteur(x->gauche), hauteur(x->droite)) + 1;
    y->hauteur = max(hauteur(y->gauche), hauteur(y->droite)) + 1;

    return y;
}

static int facteur_equilibre(NoeudAVL *N) {
    if (N == NULL) return 0;
    return hauteur(N->gauche) - hauteur(N->droite);

}


static void ajouter_valeur(NoeudAVL *noeud, Donnees *valeur) {
    if (noeud->taille == noeud->capacite) {
        noeud->capacite *= 2;
        noeud->valeurs = realloc(noeud->valeurs, noeud->capacite * sizeof(Donnees *));
        if (!noeud->valeurs) {
            fprintf(stderr, "Erreur : Réallocation de mémoire échouée\n");
            exit(EXIT_FAILURE);
        }
    }
    noeud->valeurs[noeud->taille++] = valeur;
}




NoeudAVL *inserer_avl(NoeudAVL *noeud, const char *cle, Donnees *valeur) {
    if (DEBUG) printf("DEBUG: Insertion de la clé : %s\n", cle);

    if (noeud == NULL) {
        if (DEBUG) printf("DEBUG: Nouveau nœud créé pour la clé : %s\n", cle);
        return nouveau_noeud(cle, valeur);
    }

    if (strcmp(cle, noeud->cle) < 0) {
        if (DEBUG) printf("DEBUG: Insertion à gauche pour la clé : %s\n", cle);
        noeud->gauche = inserer_avl(noeud->gauche, cle, valeur);
    } else if (strcmp(cle, noeud->cle) > 0) {
        if (DEBUG) printf("DEBUG: Insertion à droite pour la clé : %s\n", cle);
        noeud->droite = inserer_avl(noeud->droite, cle, valeur);
    } else {
        if (DEBUG) printf("DEBUG: Clé dupliquée, ajout de la valeur au tableau pour la clé : %s\n", cle);
        ajouter_valeur(noeud, valeur);
        return noeud;
    }

    noeud->hauteur = 1 + max(hauteur(noeud->gauche), hauteur(noeud->droite));
    int balance = facteur_equilibre(noeud);

    if (balance > 1 && strcmp(cle, noeud->gauche->cle) < 0) {
        if (DEBUG) printf("DEBUG: Rotation droite pour équilibrer.\n");
        return rotation_droite(noeud);
    }
    if (balance < -1 && strcmp(cle, noeud->droite->cle) > 0) {
        if (DEBUG) printf("DEBUG: Rotation gauche pour équilibrer.\n");
        return rotation_gauche(noeud);
    }
    if (balance > 1 && strcmp(cle, noeud->gauche->cle) > 0) {
        if (DEBUG) printf("DEBUG: Double rotation gauche-droite pour équilibrer.\n");
        noeud->gauche = rotation_gauche(noeud->gauche);
        return rotation_droite(noeud);
    }
    if (balance < -1 && strcmp(cle, noeud->droite->cle) < 0) {
        if (DEBUG) printf("DEBUG: Double rotation droite-gauche pour équilibrer.\n");
        noeud->droite = rotation_droite(noeud->droite);
        return rotation_gauche(noeud);
    }

    return noeud;
}

void afficher_avl(NoeudAVL *racine) {
    if (racine != NULL) {
        afficher_avl(racine->gauche);
        printf("Clé : %s, Taille : %d\n", racine->cle, racine->taille);
        for (int i = 0; i < racine->taille; i++) {
            printf("  Donnée %d - Capacity: %ld, Load: %ld\n", i + 1, racine->valeurs[i]->capacity, racine->valeurs[i]->load);
        }
        afficher_avl(racine->droite);
    }
}

long somme_avl(NoeudAVL *racine) {
    if (racine == NULL) return 0;
    long somme = 0;
    for (int i = 0; i < racine->taille; i++) {
        somme += racine->valeurs[i]->capacity;
    }
    return somme + somme_avl(racine->gauche) + somme_avl(racine->droite);
}

void liberer_avl(NoeudAVL *racine) {
    if (racine != NULL) {
        liberer_avl(racine->gauche);
        liberer_avl(racine->droite);
        for (int i = 0; i < racine->taille; i++) {
            free((char *)racine->valeurs[i]->power_plant);
            free((char *)racine->valeurs[i]->hvb_station);
            free((char *)racine->valeurs[i]->hva_station);
            free((char *)racine->valeurs[i]->lv_station);
            free((char *)racine->valeurs[i]->company);
            free((char *)racine->valeurs[i]->individual);
            free(racine->valeurs[i]);
        }
        free(racine->valeurs);
        free((char *)racine->cle);
        free(racine);
    }
}
