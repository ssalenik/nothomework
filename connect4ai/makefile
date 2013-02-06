CC=gcc

all: ./connect4.c ./connect4.h ./ai.c ./ai.h ./khash.h ./printstate.h ./printstate.c
	$(CC) -Wall connect4.c ai.c printstate.c -o connect4

optimize: ./connect4.c ./connect4.h ./ai.c ./ai.h ./khash.h ./printstate.h ./printstate.c
	$(CC) -O3 -Wall connect4.c ai.c printstate.c -o connect4

debug: ./connect4.c ./connect4.h ./ai.c ./ai.h ./khash.h ./printstate.h ./printstate.c
	$(CC) -Wall -g connect4.c ai.c printstate.c -o connect4

clean:
	rm connect4
