/*
 * badfilesystem.h
 *
 *  Created on: Apr 14, 2012
 *      Author: Stepan Salenikovich
 *      ID: 260326129
 *
 *      API for the badfilesystem.
 *
 *      See README for usage details.
 */

#ifndef BADFILESYSTEM_H_
#define BADFILESYSTEM_H_

#include "disk_emu.h"
#include <slack/std.h>
#include <slack/list.h>
#include <string.h>

//the name which will be given to the file which store the FS on disk
#define NAME "badsfs"

//file system properties
#define MAX_FILES 100	// corresponds to the number of blocks - 3
#define MAX_FILE_NAME 32
#define DATE_SIZE 32

// location of dir, fat, free block list
#define DIRECTORY_INDEX 0
#define FAT_INDEX 1
#define FREE_INDEX 2
#define DESCRIPTOR_OFFSET 3

// used to mark the blocks in the free block array written to the disk
typedef enum {USED, FREE}block_status_t;

// block size calculated at run time
int BLOCK_SIZE;
int NUM_BLOCKS;

//entry in the root directory
typedef struct {
	char filename[MAX_FILE_NAME];
	int file_size;
	char date_modified[DATE_SIZE];	// date created or modified (not implemented currently)
	int fat_index;
}directory_entry_t;

// entry in the file descriptor table
typedef struct {
	int fat_index;
	int write_index;	// index in bytes from the start
	int read_index;		// index in bytes from the start
}file_descriptor_t;

// entry in the FAT
typedef struct {
	int block_index;
	int next_fat_entry;	// points to the next fat entry for this file; 0 if its the last entry
}fat_entry_t;

// free entries list
List *free_list;
list_release_t *destroy_free_list;

// directory and FAT
directory_entry_t* directory;
fat_entry_t* fat;

// file descriptor table
file_descriptor_t* file_descriptor;

/*private function prototypes */
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
