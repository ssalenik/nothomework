CC=gcc

all: ./badfilesystem.c ./badfilesystem.h ./disk_emu.c ./disk_emu.h ./sfs_test.c
	$(CC) -Wall badfilesystem.c disk_emu.c sfs_test.c -DHAVE_PTHREAD_RWLOCK=1 -lslack -o badfilesystem_test 

clean:
	rm badfilesystem_test badsfs testoutput.txt

test: all
	./badfilesystem_test > testoutput.txt
