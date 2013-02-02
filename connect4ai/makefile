CC=gcc

all: ./connect4.c ./connect4.h ./ai.c ./ai.h
	$(CC) -Wall connect4.c ai.c -o connect4

optimize: ./connect4.c ./connect4.h ./ai.c ./ai.h
	$(CC) -O3 -Wall connect4.c ai.c -o connect4

clean:
	rm connect4
