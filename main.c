#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "io.h"

#define DEBUG 1 // Activez ou désactivez les messages de débogage

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <fichier_entree.dat> <fichier_sortie.dat> <has_header>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *fichier_entree = argv[1];
    const char *fichier_sortie = argv[2];
    int has_header = atoi(argv[3]); // Convertit le troisième argument en entier

    if (DEBUG) printf("DEBUG: Lecture du fichier d'entrée : %s\n", fichier_entree);

    // Charger les données dans l'arbre AVL
    NoeudAVL *arbre = charger_dat_dans_avl(fichier_entree, has_header);

    if (!arbre) {
        fprintf(stderr, "Erreur : Arbre AVL non généré à partir de %s\n", fichier_entree);
        return EXIT_FAILURE;
    }

    if (DEBUG) printf("DEBUG: Arbre AVL généré avec succès.\n");
    afficher_avl(arbre);

    // Calculer la somme des capacités
    long somme = somme_avl(arbre);
    printf("Consommation totale (Capacity) : %ld\n", somme);

    // Générer le fichier de sortie
    if (!generer_fichier_sortie(fichier_sortie, arbre)) {
        fprintf(stderr, "Erreur : Impossible de générer le fichier de sortie %s\n", fichier_sortie);
        liberer_avl(arbre);
        return EXIT_FAILURE;
    }

    if (DEBUG) printf("DEBUG: Fichier de sortie généré avec succès : %s\n", fichier_sortie);
    liberer_avl(arbre);

    printf("Traitement terminé. Résultats enregistrés dans %s\n", fichier_sortie);
    return EXIT_SUCCESS;
}
