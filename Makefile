FLAGS=-std=c11 -Wall -Wextra -O2 -Wpedantic

all: imgsearch

imgsearch: main_stepbystep.c
	gcc ${FLAGS} -o image_search main_stepbystep.c