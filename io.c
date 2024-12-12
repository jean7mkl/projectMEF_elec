#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "avl.h"

#define DEBUG 1 // Activez ou désactivez les messages de débogage



NoeudAVL *charger_dat_dans_avl(const char *nom_fichier, int has_header) {
    if (DEBUG) printf("DEBUG: Lecture du fichier d'entrée : %s\n", nom_fichier);

    FILE *fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        perror("Erreur : Impossible d'ouvrir le fichier");
        return NULL;
    }
    if (DEBUG) printf("DEBUG: Fichier ouvert avec succès : %s\n", nom_fichier);

    char ligne[1024]; // Augmenté la taille pour gérer des lignes plus longues
    NoeudAVL *racine = NULL;

    // Ignore la première ligne si has_header est vrai
    if (has_header) {
        if (fgets(ligne, sizeof(ligne), fichier) == NULL) {
            fprintf(stderr, "Erreur : Fichier vide ou lecture échouée\n");
            fclose(fichier);
            return NULL;
        }
        if (DEBUG) printf("DEBUG: Première ligne ignorée : %s\n", ligne);
    }

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (DEBUG) printf("DEBUG: Ligne brute lue : %s\n", ligne);

        // Extraction des champs avec strtok
        char *power_plant = strtok(ligne, ";");
        char *hvb_station = strtok(NULL, ";");
        char *hva_station = strtok(NULL, ";");
        char *lv_station = strtok(NULL, ";");
        char *company = strtok(NULL, ";");
        char *individual = strtok(NULL, ";");
        char *capacity_str = strtok(NULL, ";");
        char *load_str = strtok(NULL, "\n");

        // Validation des champs
        if (!power_plant || !hvb_station || !hva_station || !lv_station ||
            !company || !individual || !capacity_str || !load_str) {
            fprintf(stderr, "DEBUG: Ligne ignorée - Champs manquants ou invalides : %s\n", ligne);
            continue;
        }

        // Conversion des champs numériques
        if (strcmp(capacity_str, "-") == 0) capacity_str = "0";
        if (strcmp(load_str, "-") == 0) load_str = "0";

        long capacity = atol(capacity_str);
        long load = atol(load_str);

        if (capacity < 0 || load < 0) {
            fprintf(stderr, "DEBUG: Ligne ignorée - Valeurs numériques invalides\n");
            continue;
        }

        // Allocation mémoire pour les données
        Donnees *donnees = (Donnees *)malloc(sizeof(Donnees));
        donnees->power_plant = my_strdup(power_plant);
        donnees->hvb_station = my_strdup(hvb_station);
        donnees->hva_station = my_strdup(hva_station);
        donnees->lv_station = my_strdup(lv_station);
        donnees->company = my_strdup(company);
        donnees->individual = my_strdup(individual);
        donnees->capacity = capacity;
        donnees->load = load;

        if (DEBUG) printf("DEBUG: Insertion dans l'arbre AVL pour la clé : %s\n", power_plant);
        racine = inserer_avl(racine, power_plant, donnees);
    }

    fclose(fichier);

    if (!racine) {
        fprintf(stderr, "Erreur : L'arbre AVL est vide après le traitement des données.\n");
    }

    return racine;
}


void ecrire_avl(NoeudAVL *racine, FILE *fichier) {
    if (racine != NULL) {
        ecrire_avl(racine->gauche, fichier);
        for (int i = 0; i < racine->taille; i++) {
            fprintf(fichier, "%s;%s;%s;%s;%s;%s;%ld;%ld\n",
                    racine->cle,
                    racine->valeurs[i]->hvb_station,
                    racine->valeurs[i]->hva_station,
                    racine->valeurs[i]->lv_station,
                    racine->valeurs[i]->company,
                    racine->valeurs[i]->individual,
                    racine->valeurs[i]->capacity,
                    racine->valeurs[i]->load);
        }
        ecrire_avl(racine->droite, fichier);
    }
}

int generer_fichier_sortie(const char *nom_fichier, NoeudAVL *racine) {
    if (DEBUG) printf("DEBUG: Ouverture du fichier de sortie : %s\n", nom_fichier);
    FILE *fichier = fopen(nom_fichier, "w");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }

    fprintf(fichier, "Power plant;HV-B Station;HV-A Station;LV Station;Company;Individual;Capacity;Load\n");
    if (DEBUG) printf("DEBUG: En-tête écrit dans le fichier de sortie.\n");

    ecrire_avl(racine, fichier);

    fclose(fichier);
    if (DEBUG) printf("DEBUG: Fichier de sortie fermé.\n");
    return 1;
}
