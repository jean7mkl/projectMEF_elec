#ifndef IO_H
#define IO_H

#include "avl.h"

// Charger un fichier CSV dans un arbre AVL
NoeudAVL*charger_csv_dans_avl(const char *nom_fichier);

// Générer un fichier CSV à partir d'un arbre AVL
int generer_fichier_sortie(const char *nom_fichier, NoeudAVL *racine);

#endif
