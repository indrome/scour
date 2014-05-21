CC=g++
CFLAGS=-Wall -std=c++11 -I/usr/include/opencv/ -g -I./include/ -I/home/tobias/src/libsvm/
LDFLAGS=-lm -lopencv_core -lopencv_highgui -lopencv_imgproc /home/tobias/src/libsvm/svm.o

.PHONY: clean 

all: lib/HOG.o bin/HOG_BB bin/HOG_sliding


lib/HOG.o: src/HOG.cpp include/HOG.hpp
	$(CC) $(CFLAGS) src/HOG.cpp  $(LDFLAGS) -c -o lib/HOG.o

bin/HOG_BB: lib/HOG.o src/HOG_BB.cpp include/HOG.hpp
	$(CC) $(CFLAGS) src/HOG_BB.cpp lib/HOG.o $(LDFLAGS) -o $@

bin/HOG_sliding: lib/HOG.o src/HOG_sliding.cpp include/HOG.hpp
	$(CC) $(CFLAGS) src/HOG_sliding.cpp lib/HOG.o $(LDFLAGS) -o $@

clean:
	rm -f lib/*
	rm -f bin/*
	
