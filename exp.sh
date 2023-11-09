#!/bin/bash

prefix="YourPrefix: "

tmpdist=/Users/andreescario/Documents/school/unif2/os/projet1/base-projet1/img-dist/
PATH="$PATH:${tmpdist}"



while IFS= read -r line; do
    echo "$prefix$line" 
done