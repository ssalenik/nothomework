CC=gcc

all: ./sem-ex.c ./sillythreads.c ./sillythreads.h
	$(CC) -Wall sem-ex.c sillythreads.c -DHAVE_PTHREAD_RWLOCK=1 -lm -lslack -o sillythreadtest 

clean:
	rm sillythreadtest testoutput.txt

test: all
	./sillythreadtest > testoutput.txt
