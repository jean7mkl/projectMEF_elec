#!/bin/bash

# Vérification des arguments
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Usage: $0 <chemin_csv> <type_station> <type_consommateur> [<id_centrale>] [OPTIONS]"
    echo "OPTIONS :"
    echo "  <chemin_csv>       Chemin du fichier CSV d'entrée."
    echo "  <type_station>     Type de station : hvb, hva ou lv."
    echo "  <type_consommateur> Type de consommateur : comp, indiv, all."
    echo "  [<id_centrale>]    (Optionnel) Identifiant de la centrale à filtrer."
    echo "  --sort             Trier les données filtrées par capacité décroissante."
    echo "  --keep-tmp         Ne pas supprimer les fichiers temporaires."
    echo "  -h, --help         Affiche cette aide."
    exit 0
fi

# Chronométrage global
START_TIME=$(date +%s)

# Vérification des dépendances
for cmd in grep wc sort mingw32-make; do
    if ! command -v $cmd &> /dev/null; then
        echo "Erreur : La commande '$cmd' n'est pas installée ou introuvable."
        exit 1
    fi
done

# Initialisation des variables
id_centrale=""
keep_tmp=false
sort_data=false

# Vérification du nombre minimal d'arguments
if [[ $# -lt 3 ]]; then
    echo "Erreur : Arguments insuffisants."
    echo "Usage: $0 <chemin_csv> <type_station> <type_consommateur> [<id_centrale>] [OPTIONS]"
    exit 1
fi

# Extraction des arguments principaux
chemin_csv="$1"
type_station="$2"
type_consommateur="$3"
shift 3

# Vérification si id_centrale est présent
if [[ $# -gt 0 && ! "$1" =~ ^-- ]]; then
    id_centrale="$1"
    shift
fi

# Gestion des options supplémentaires
while [[ $# -gt 0 ]]; do
    case "$1" in
        --keep-tmp) keep_tmp=true ;;
        --sort) sort_data=true ;;
        -h|--help)
            echo "Usage: $0 <chemin_csv> <type_station> <type_consommateur> [<id_centrale>] [OPTIONS]"
            echo "OPTIONS :"
            echo "  --sort             Trier les données filtrées par capacité décroissante."
            echo "  --keep-tmp         Ne pas supprimer les fichiers temporaires."
            exit 0
            ;;
        *)
            echo "Option inconnue : $1"
            exit 1
            ;;
    esac
    shift
done

# Vérification des fichiers et des valeurs
if [[ ! -f "$chemin_csv" ]]; then
    echo "Erreur : Le fichier $chemin_csv n'existe pas."
    exit 1
fi

if [[ "$type_station" != "hvb" && "$type_station" != "hva" && "$type_station" != "lv" ]]; then
    echo "Erreur : Type de station invalide ($type_station). Utiliser hvb, hva ou lv."
    exit 1
fi

if [[ "$type_consommateur" != "comp" && "$type_consommateur" != "indiv" && "$type_consommateur" != "all" ]]; then
    echo "Erreur : Type de consommateur invalide ($type_consommateur). Utiliser comp, indiv ou all."
    exit 1
fi

# Préparer le répertoire temporaire
mkdir -p tmp
fichier_filtre="tmp/filtered.csv"

# Filtrer les données
echo "Filtrage des données pour type_station=$type_station, type_consommateur=$type_consommateur, id_centrale=$id_centrale..."
grep "$type_station" "$chemin_csv" | grep "$type_consommateur" > "$fichier_filtre"

if [[ ! -s "$fichier_filtre" ]]; then
    echo "Erreur : Aucun résultat après le filtrage pour type_station=$type_station et type_consommateur=$type_consommateur."
    exit 1
fi

# Filtrage supplémentaire par id_centrale
if [[ -n "$id_centrale" ]]; then
    grep "$id_centrale" "$fichier_filtre" > "tmp/filtered_with_id.csv"
    fichier_filtre="tmp/filtered_with_id.csv"

    if [[ ! -s "$fichier_filtre" ]]; then
        echo "Erreur : Aucun résultat après le filtrage par id_centrale ($id_centrale)."
        exit 1
    fi
fi

# Tri des données si demandé
if [[ "$sort_data" == true ]]; then
    echo "Tri des données filtrées par capacité décroissante..."
    sort -t, -k5,5nr "$fichier_filtre" > "tmp/sorted.csv"
    fichier_filtre="tmp/sorted.csv"
fi

echo "Données filtrées disponibles dans $fichier_filtre"

# Vérifier et compiler le programme C
if [[ ! -f codeC/main ]]; then
    echo "Compilation du programme C..."
    cd codeC || exit 1
    mingw32-make
    if [[ $? -ne 0 ]]; then
        echo "Erreur : Compilation échouée."
        exit 1
    fi
    cd ..
fi

# Exécuter le programme C
echo "Exécution du programme C avec $fichier_filtre..."
./codeC/main "$fichier_filtre"
if [[ $? -ne 0 ]]; then
    echo "Erreur : Le programme C a rencontré un problème."
    exit 1
fi

# Déplacer le fichier de sortie
if [[ -f output.csv ]]; then
    mkdir -p output
    mv output.csv "output/output_${type_station}_${type_consommateur}.csv"
    echo "Résultats enregistrés dans output/output_${type_station}_${type_consommateur}.csv"
else
    echo "Erreur : Le programme C n'a pas généré de fichier de sortie."
    exit 1
fi

# Nettoyage des fichiers temporaires
if [[ "$keep_tmp" == false ]]; then
    echo "Nettoyage des fichiers temporaires..."
    rm -rf tmp
fi

# Résumé
END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))
echo "Temps total d'exécution : $ELAPSED secondes"
