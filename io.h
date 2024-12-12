#ifndef IO_H
#define IO_H

#include <stdio.h> // Ajout pour la définition de FILE
#include "avl.h"

NoeudAVL *charger_dat_dans_avl(const char *nom_fichier, int has_header);
int generer_fichier_sortie(const char *nom_fichier, NoeudAVL *racine);
void ecrire_avl(NoeudAVL *racine, FILE *fichier);

#endif
