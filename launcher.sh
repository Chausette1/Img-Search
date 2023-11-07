#!/bin/bash

if [[ -z "$1" ]]; then
	echo "veuillez entrer un mode (-a / -automatic ou -i / -interactive)"
	exit
fi

make

if [[ "$1" == '-a' || "$1" == 'automatic' ]]; then
	if [[ -z  "$2" || ! -e "$2" ]]; then
		echo "veuillez entrer le chemin vers le dossier contenant les images"
		exit
	fi
	
	./img-search "$1" "$2"


elif [[ "$1" == '-i' || "$1" == 'interactive' ]]; then
	if [[ -e "$2" ]]; then
		echo "Chemin invalide"
		exit
	fi
	
	if [[ -z "$2" ]]; then
		./img-search "$1" ./img/
	else
		./img-search "$1" "$2"
	fi

else
	echo "Mode non valide."
	exit
fi
