all: main.c huff.c huff.h bitfile.c bitfile.h
	gcc -Wall -g -o huff huff.c main.c bitfile.c -I. -lm

clean:
	rm huff
