FLAGS=-std=c11 -Wall -Wextra -O2 -Wpedantic

all: imgsearch

imgsearch: main_stepbystep.c parent.c child.c common.c
	gcc ${FLAGS} -c parent.c
	gcc ${FLAGS} -c child.c
	gcc ${FLAGS} -c common.c
	gcc ${FLAGS} -o img-search main_stepbystep.c parent.o child.o common.o
	rm -f *.o