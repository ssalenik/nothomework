/*
 * badfilesystem.h
 *
 *  Created on: Apr 14, 2012
 *      Author: Stepan Salenikovich
 */

#ifndef BADFILESYSTEM_H_
#define BADFILESYSTEM_H_

#include "disk_emu.h"
#include <slack/std.h>
#include <slack/list.h>

//file system properties
#define MAX_FILES 1024	// corresponds to the number of blocks - 3
#define MAX_FILE_NAME 32

// block size calculated at run time
static int BLOCK_SIZE;
static int NUM_BLOCKS;

//entry in the root directory
typedef struct {
	char* file_name;
	int file_size;
	int fat_index;
	int write_block;
	int write_index;	// offset in bytes from the start of the write block
	int read_block;
	int read_offset;	// offset in bytes from the start of the read block
}directory_entry_t;

typedef struct {
	int block_index;
	int next_fat_entry;	// points to the next fat entry for this file; 0 if its the last entry
}fat_entry_t;

// free entries list
List *free_blocks;
list_release_t *destroy_free_blocks;

directory_entry_t directory[MAX_FILES ];
fat_entry_t fat[MAX_FILES];

/*private function prototypes */
void init();

/* public function prototypes */
void mksfs(int fresh);
void sfs_ls();
int sfs_fopen(char *name);
void sfs_fclose(int fileID);
void sfs_fwrite(int fileID, char *buf, int length);
void sfs_fread(int fileID, char *buf, int length);
void sfs_fseek(int fileID, int loc);
int sfs_remove(char *file);


#endif /* BADFILESYSTEM_H_ */
