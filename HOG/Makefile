CC=g++
CFLAGS=-Wall -std=c++11 -I/usr/include/opencv/ -O3 -I./include/
LDFLAGS=-lm -lopencv_core -lopencv_highgui

.PHONY: clean 

all: lib/HOG.o bin/HOG_BB bin/HOG_sliding


lib/HOG.o: src/HOG.cpp
	$(CC) $(CFLAGS) src/HOG.cpp  $(LDFLAGS) -c -o lib/HOG.o

bin/HOG_BB: lib/HOG.o src/HOG_BB.cpp
	$(CC) $(CFLAGS) src/HOG_BB.cpp lib/HOG.o $(LDFLAGS) -o $@

bin/HOG_sliding: lib/HOG.o src/HOG_sliding.cpp
	$(CC) $(CFLAGS) src/HOG_sliding.cpp lib/HOG.o $(LDFLAGS) -o $@

clean:
	rm -f lib/*
	rm -f bin/*
	
