#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "io.h"

NoeudAVL *charger_csv_dans_avl(const char *nom_fichier) {
    printf("Chargement des données depuis %s...\n", nom_fichier);

    FILE *fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        return NULL;
    }

    NoeudAVL *arbre = NULL;
    char ligne[256];
    char id_station[128], type_station[128], type_consommateur[128], id_centrale[128];
    int capacity, load;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Supprimer le saut de ligne final, si présent
        ligne[strcspn(ligne, "\n")] = '\0';

        // Ignorer les en-têtes si présents
        if (strncmp(ligne, "id_station", 10) == 0) {
            continue;
        }

        // Extraire les données
        if (sscanf(ligne, "%127[^,],%127[^,],%127[^,],%127[^,],%d,%d",
                   id_station, type_station, type_consommateur, id_centrale, &capacity, &load) == 6) {
            // Construire une clé unique
            char cle_unique[256];
            snprintf(cle_unique, sizeof(cle_unique), "%s_%s_%s", type_station, type_consommateur, id_centrale);

            // Afficher les données pour débogage
            printf("Ligne analysée : %s\n", ligne);
            printf("Clé : %s, Capacité : %d\n", cle_unique, capacity);

            // Insérer dans l'arbre AVL
            arbre = inserer_avl(arbre, cle_unique, capacity);
        } else {
            fprintf(stderr, "Format de ligne invalide : %s\n", ligne);
        }
    }

    fclose(fichier);
    return arbre;
}



void ecrire_avl(FILE *fichier, NoeudAVL *racine) {
    if (racine != NULL) {
        ecrire_avl(fichier, racine->gauche);
        fprintf(fichier, "%s,%d\n", racine->cle, racine->valeur);
        ecrire_avl(fichier, racine->droit);
    }
}



int generer_fichier_sortie(const char *nom_fichier, NoeudAVL *racine) {
    printf("Génération du fichier de sortie %s...\n", nom_fichier);

    FILE *fichier = fopen(nom_fichier, "w");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }

    // Écrire les données dans le fichier
    ecrire_avl(fichier, racine);

    fclose(fichier);
    return 1;
}
