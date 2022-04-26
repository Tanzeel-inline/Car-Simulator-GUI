CC=gcc
CFLAGS=-c -std=gnu11 -O
LIBS= -lSDL_bgi -l SDL2 -pthread -lm

all: cp

cp: CarPark.o Queue.o CPSimulator.o Car.h Queue.h CarPark.h
	$(CC) CarPark.o Queue.o CPSimulator.o -o cp $(LIBS)

Queue.o: Queue.c Queue.h
	$(CC) $(CFLAGS) Queue.c

CPSimulator.o: CPSimulator.c
	$(CC) $(CFLAGS) CPSimulator.c

clean:
	rm cp Queue.o CPSimulator.o

