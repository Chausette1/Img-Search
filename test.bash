tmpdist=/Users/andreescario/Documents/school/unif2/os/projet1/base-projet1/img-dist/
imgspath=/Users/andreescario/Documents/school/unif2/os/projet1/base-projet1/img

if ! echo "$PATH" | grep -Eq "(^|:)$1($|:)" ; then
    PATH="$PATH:${tmpdist}"
fi

make; ./img-search ${imgspath}/1.bmp