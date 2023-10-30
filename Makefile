FLAGS=-std=c11 -Wall -Wextra -O2 -Wpedantic

all: imgsearch

imgsearch: main_stepbystep.c parent.c child.c
	gcc ${FLAGS} -c parent.c
	gcc ${FLAGS} -c child.c
	gcc ${FLAGS} -o image_search main_stepbystep.c parent.o child.o
	rm -f *.o