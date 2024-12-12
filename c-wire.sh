#!/bin/bash

# Aide utilisateur
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Usage: $0 <chemin_dat> <type_station> <type_consommateur> [<id_centrale>] [OPTIONS]"
    echo "OPTIONS :"
    echo "  --sort             Trier les données filtrées par capacité décroissante."
    echo "  --keep-tmp         Ne pas supprimer les fichiers temporaires."
    echo "  --debug            Activer les messages de débogage."
    exit 0
fi

# Chronométrage global
START_TIME=$(date +%s)

# Vérification des arguments
if [[ $# -lt 3 ]]; then
    echo "Erreur : Arguments insuffisants."
    echo "Usage: $0 <chemin_dat> <type_station> <type_consommateur> [<id_centrale>] [OPTIONS]"
    exit 1
fi

chemin_dat="$1"
type_station="$2"
type_consommateur="$3"
shift 3

id_centrale=""
keep_tmp=false
sort_data=false
debug=false

# Gestion des options
while [[ $# -gt 0 ]]; do
    case "$1" in
        --keep-tmp) keep_tmp=true ;;
        --sort) sort_data=true ;;
        --debug) debug=true ;;
        *) echo "Option inconnue : $1"; exit 1 ;;
    esac
    shift
done

# Vérification du fichier d'entrée
if [[ ! -f "$chemin_dat" ]]; then
    echo "Erreur : Le fichier $chemin_dat n'existe pas."
    exit 1
fi

# Préparation des fichiers temporaires
mkdir -p tmp
fichier_filtre="tmp/filtered.dat"

# Validation et nettoyage du fichier
echo "Validation et nettoyage du fichier d'entrée..."
awk -F';' '
    BEGIN { valid = 1 }
    NF != 8 { valid = 0; print "Erreur : Ligne invalide :", $0 > "/dev/stderr" }
    END { if (!valid) exit 1 }
' "$chemin_dat"

if [[ $? -ne 0 ]]; then
    echo "Erreur : Format invalide dans le fichier $chemin_dat."
    exit 1
fi

# Compléter les colonnes manquantes avec des tirets
awk -F';' '
    BEGIN { OFS = ";" }
    NF < 8 {
        for (i = NF + 1; i <= 8; i++) $i = "-"
    }
    { print $0 }
' "$chemin_dat" > "tmp/cleaned.dat"
chemin_dat="tmp/cleaned.dat"

# Filtrer les données
echo "Filtrage des données..."
awk -F';' -v station="$type_station" -v consumer="$type_consommateur" -v id="$id_centrale" -v debug="$debug" '
    BEGIN { OFS = ";" }
    NR > 1 {
        hvb = ($2 == "-") ? "0" : $2;
        hva = ($3 == "-") ? "0" : $3;
        lv = ($4 == "-") ? "0" : $4;
        company = ($5 == "-") ? "-" : $5;
        individual = ($6 == "-") ? "-" : $6;
        capacity = ($7 == "-") ? "0" : $7;
        load = ($8 == "-") ? "0" : $8;

        # Debugging messages
        if (debug == "true") {
            print "DEBUG: Ligne analysée ->", $0 > "/dev/stderr"
            print "DEBUG: HVB:", hvb, "HVA:", hva, "LV:", lv, "Company:", company, "Individual:", individual, "Capacity:", capacity, "Load:", load > "/dev/stderr"
        }

        # Vérification type_station
        if ((station == "hvb" && hvb == "1") ||
            (station == "hva" && hva == "1") ||
            (station == "lv" && lv == "1") ||
            (station == "all")) {

            # Vérification type_consommateur
            if ((consumer == "Company" && company != "-") ||
                (consumer == "Individual" && individual != "-") ||
                (consumer == "All")) {

                # Vérifier id_centrale si présent
                if (id == "" || $1 == id) {
                    print $1, hvb, hva, lv, company, individual, capacity, load > "tmp/filtered.dat"
                }
            }
        }
    }
' "$chemin_dat"


if [[ ! -s "$fichier_filtre" ]]; then
    echo "Erreur : Aucun résultat après le filtrage."
    exit 1
fi

# Option de tri
if [[ "$sort_data" == true ]]; then
    echo "Tri des données par capacité décroissante..."
    sort -t';' -k7,7nr "$fichier_filtre" -o "$fichier_filtre"
fi

# Compilation des fichiers C
if [[ ! -f main ]]; then
    echo "Compilation du programme C..."
    gcc -c avl.c -o avl.o
    gcc -c io.c -o io.o
    gcc -c main.c -o main.o
    gcc -o main avl.o io.o main.o
    if [[ $? -ne 0 ]]; then
        echo "Erreur : Compilation échouée."
        exit 1
    fi
fi

# Exécuter le programme
echo "Exécution du programme avec les données filtrées..."
./main "$fichier_filtre"

# Gestion du fichier de sortie
if [[ -f output.dat ]]; then
    mkdir -p output
    mv output.dat "output/output_${type_station}_${type_consommateur}.dat"
    echo "Résultats enregistrés dans output/output_${type_station}_${type_consommateur}.dat"
else
    echo "Erreur : Pas de fichier de sortie généré."
    exit 1
fi

# Nettoyage si demandé
if [[ "$keep_tmp" == false ]]; then
    rm -rf tmp
fi

# Temps total d'exécution
END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))
echo "Temps total : $ELAPSED secondes."
