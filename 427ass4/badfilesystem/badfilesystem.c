/*
 * badfilesystem.c
 *
 *  Created on: Apr 14, 2012
 *      Author: Stepan Salenikovich
 *      ID: 260326129
 *
 *      API for the badfilesystem.
 *
 *      See README for usage details.
 */

#include"badfilesystem.h"

/* private functions */
int init() {
	//calculate number of blocks
	NUM_BLOCKS = MAX_FILES + 3; //free blocks + root dir + fat + free blocks

	//create free list
	free_list = list_create(destroy_free_list);

	for (int i = 3; i < NUM_BLOCKS; i++)
		list_append_int(free_list, i); //all the blocks except the first 3 should be free

	//determine block size
	BLOCK_SIZE = MAX_FILES * sizeof(directory_entry_t);
	if (MAX_FILES * sizeof(file_descriptor_t) > BLOCK_SIZE)
		BLOCK_SIZE = MAX_FILES * sizeof(file_descriptor_t);
	if (MAX_FILES * sizeof(fat_entry_t) > BLOCK_SIZE)
		BLOCK_SIZE = MAX_FILES * sizeof(fat_entry_t);
	if (NUM_BLOCKS * sizeof(int) > BLOCK_SIZE)
		BLOCK_SIZE = NUM_BLOCKS * sizeof(block_status_t); // should never happen

	return 0;
}
int write_directory() {
	if (write_blocks(DIRECTORY_INDEX, 1, directory))
		return 0;
	else
		return -1;
}
int read_directory() {
	if (read_blocks(DIRECTORY_INDEX, 1, directory))
		return 0;
	else
		return -1;
}
int write_fat() {
	if (write_blocks(FAT_INDEX, 1, fat))
		return 0;
	else
		return -1;
	return 0;
}
int read_fat() {
	if (read_blocks(FAT_INDEX, 1, fat))
		return 0;
	else
		return -1;
	return 0;
}
int write_free() {
	int i, block;
	block_status_t temp[MAX_FILES];

	// init all to USED
	for (i = 0; i < NUM_BLOCKS; i++) {
		temp[i] = USED;
	}

	for (i = 0; i < list_length(free_list); i++) {
		block = list_item_int(free_list);
		temp[block] = FREE;
	}

	if (write_blocks(FREE_INDEX, 1, temp)) {
		free(temp);
		return 0;
	} else {
		free(temp);
		return -1;
	}

}
int read_free() {
	block_status_t temp[MAX_FILES];

	list_empty(free_list);

	if (read_blocks(FREE_INDEX, 1, temp)) {
		for (int i = 0; i < MAX_FILES; i++) {
			if (temp[i] == FREE) {
				list_append_int(free_list, i);
			}
		}
		free(temp);
		return 0;
	} else {
		free(temp);
		return -1;
	}
}

/* public functions */
void mksfs(int fresh) {

}
void sfs_ls() {

}
int sfs_fopen(char *name) {

	return 0;
}
void sfs_fclose(int fileID) {

}
void sfs_fwrite(int fileID, char *buf, int length) {

}
void sfs_fread(int fileID, char *buf, int length) {

}
void sfs_fseek(int fileID, int loc) {

}
int sfs_remove(char *file) {

	return 0;
}

