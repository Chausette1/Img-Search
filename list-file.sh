#!/bin/bash

if [ -z $1 ]
then
	echo "Missing directory name."
	exit 1
fi

if [ ! -d $1 ]
then
	echo "The specified path is not a directory."
	exit 2
fi

for file in "$1"/*; do
	if [ -f "$file" ]; then
	 echo "$(basename "$file")"
	fi
done