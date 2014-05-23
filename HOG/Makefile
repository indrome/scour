CC=g++
CFLAGS=-Wall -std=c++11 -I/usr/include/opencv/ -O3 -I./include/ -I/home/tobias/src/libsvm/
LDFLAGS=-lm -lopencv_core -lopencv_highgui -lopencv_imgproc 

.PHONY: clean 

all: lib/HOG.o bin/detect_bb bin/detect_sliding


lib/HOG.o: src/HOG.cpp include/HOG.hpp
	$(CC) $(CFLAGS) src/HOG.cpp  $(LDFLAGS) -c -o lib/HOG.o

bin/detect_bb: lib/HOG.o src/detect_bb.cpp include/HOG.hpp
	$(CC) $(CFLAGS) src/detect_bb.cpp lib/HOG.o $(LDFLAGS) -o $@

bin/detect_sliding: lib/HOG.o src/detect_sliding.cpp include/HOG.hpp
	$(CC) $(CFLAGS) src/detect_sliding.cpp lib/HOG.o $(LDFLAGS) -o $@

clean:
	rm -f lib/*
	rm -f bin/*
	
