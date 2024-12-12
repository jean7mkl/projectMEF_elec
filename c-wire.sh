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


if [[ ! -f "$chemin_dat" ]]; then
    echo "Erreur : Le fichier $chemin_dat n'existe pas."
    exit 1
fi
arg_fus="$type_station $type_consommateur"

case "$arg_fus" in
	"hvb comp")
	if [ -z "$id_centrale" ]; then
		station=$(grep  -P "^(\d+);(\d+);-;-;-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^(\d+);(\d+);-;-;(\d+);-;-;(\d+)" $chemin_dat)
	else
		station=$(grep  -P "^($id_centrale);(\d+);-;-;-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^($id_centrale);(\d+);-;-;(\d+);-;-;(\d+)" $chemin_dat)
	fi
	;;
	"hva comp")
	if [ -z "$id_centrale" ]; then
		station=$(grep  -P "^(\d+);(\d+);(\d+);-;-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^(\d+);-;(\d+);-;(\d+);-;-;(\d+)" $chemin_dat)
	else
		station=$(grep  -P "^($id_centrale);(\d+);(\d+);-;-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^($id_centrale);-;(\d+);-;(\d+);-;-;(\d+)" $chemin_dat)
	fi
	;;
	"lv comp")
	if [ -z "$id_centrale" ]; then
		station=$(grep  -P "^(\d+);-;(\d+);(\d+);-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^(\d+);-;-;(\d+);(\d+);-;-;(\d+)" $chemin_dat)
	else
		station=$(grep  -P "^($id_centrale);-;(\d+);(\d+);-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^($id_centrale);-;-;(\d+);(\d+);-;-;(\d+)" $chemin_dat)
	fi
	;;
	"lv indiv")
	if [ -z "$id_centrale" ]; then
		station=$(grep  -P "^(\d+);-;(\d+);(\d+);-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^(\d+);-;-;(\d+);-;(\d+);-;(\d+)" $chemin_dat)
	else 
		station=$(grep  -P "^($id_centrale);-;(\d+);(\d+);-;-;(\d+);-" $chemin_dat)
		usagers=$(grep  -P "^($id_centrale);-;-;(\d+);-;(\d+);-;(\d+)" $chemin_dat)
	fi
	;;
	"lv all")
	if [ -z "$id_centrale" ]; then
		station=$(grep  -P "^(\d+);-;(\d+);(\d+);-;-;(\d+);-" $chemin_dat)
		usagers_1=$(grep  -P "^(\d+);-;-;(\d+);(\d+);-;-;(\d+)" $chemin_dat)
		usagers_2=$(grep  -P "^(\d+);-;-;(\d+);-;(\d+);-;(\d+)" $chemin_dat)
	else
		station=$(grep  -P "^($id_centrale);-;(\d+);(\d+);-;-;(\d+);-" $chemin_dat)
		usagers_1=$(grep  -P "^($id_centrale);-;-;(\d+);(\d+);-;-;(\d+)" $chemin_dat)
		usagers_2=$(grep  -P "^($id_centrale);-;-;(\d+);-;(\d+);-;(\d+)" $chemin_dat)
	fi
	usagers="$usagers_1$'\n'$usagers_2"
	;;
	*)
	echo "Erreur : mode $arg_fus non pris en charge."
	exit -1
esac
    echo -e "$station\n$usagers" > tmp/filtered.dat
   	

# Vérification si le fichier filtré est vide
if [[ ! -s "tmp/filtered.dat" ]]; then
    echo "Erreur : Aucun résultat après le filtrage."
    exit 1
fi

echo "Filtrage terminé avec succès. Les résultats sont dans tmp/filtered.dat."

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
./main "$fichier_filtre output.dat 0"

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
