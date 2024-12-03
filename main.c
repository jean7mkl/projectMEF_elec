#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "io.h"

int main(int argc, char *argv[]) {
    // Vérifier le nombre d'arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier_entree.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Charger le fichier CSV en entrée
    const char *fichier_entree = argv[1];
    NoeudAVL *arbre = charger_csv_dans_avl(fichier_entree);

    if (!arbre) {
        fprintf(stderr, "Erreur: Impossible de charger les données depuis %s\n", fichier_entree);
        return EXIT_FAILURE;
    }

    // Afficher l'arbre AVL dans la console
    printf("Données dans l'arbre AVL :\n");
    afficher_avl(arbre);

    // Générer le fichier CSV de sortie
    if (!generer_fichier_sortie("output.csv", arbre)) {
        fprintf(stderr, "Erreur: Impossible de générer le fichier de sortie\n");
        return EXIT_FAILURE;
    }

    // Libérer la mémoire de l'arbre AVL
    liberer_avl(arbre);

    printf("Traitement terminé. Résultats enregistrés dans output.csv\n");
    return EXIT_SUCCESS;
}
