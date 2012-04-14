/*
 * badfilesystem.h
 *
 *  Created on: Apr 14, 2012
 *      Author: Stepan Salenikovich
 *      ID: 260326129
 */

#ifndef BADFILESYSTEM_H_
#define BADFILESYSTEM_H_

#include "disk_emu.h"
#include <slack/std.h>
#include <slack/list.h>

#define NAME "badfilesystem"

//file system properties
#define MAX_FILES 1024	// corresponds to the number of blocks - 3
#define MAX_FILE_NAME 32
#define DATE_SIZE 32

// location of dir, fat, free block list
#define DIRECTORY_INDEX 0
#define FAT_INDEX 1
#define FREE_INDEX 2

typedef enum {USED, FREE}block_status_t;

// block size calculated at run time
static int BLOCK_SIZE;
static int NUM_BLOCKS;

//entry in the root directory
typedef struct {
	char filename[MAX_FILE_NAME];
	int file_size;
	char date_modified[DATE_SIZE];	// date created or modified
	int fat_index;
}directory_entry_t;

typedef struct {
	int fat_index;
	int write_block;
	int write_index;	// offset in bytes from the start of the write block
	int read_block;
	int read_offset;	// offset in bytes from the start of the read block
}file_descriptor_t;

typedef struct {
	int block_index;
	int next_fat_entry;	// points to the next fat entry for this file; 0 if its the last entry
}fat_entry_t;

// free entries list
List *free_list;
list_release_t *destroy_free_list;

directory_entry_t directory[MAX_FILES];
fat_entry_t fat[MAX_FILES];

file_descriptor_t file_descriptor[MAX_FILES];

/*private function prototypes */

/**
 * initializes everything needed for the FS
 *
 * returns 0 if successful
 */
int init();
int write_directory();
int read_directory();
int write_fat();
int read_fat();
int write_free();
int read_free();

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
