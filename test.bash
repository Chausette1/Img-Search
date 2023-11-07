tmpdist=/Users/andreescario/Documents/school/unif2/os/projet1/base-projet1/img-dist/
imgspath=/Users/andreescario/Documents/school/unif2/os/projet1/base-projet1/img

if ! echo "$PATH" | grep -Eq "(^|:)$1($|:)" ; then
    PATH="$PATH:${tmpdist}"
fi

ipcrm -M 1729; make; ./image-search ${imgspath}/1.bmp