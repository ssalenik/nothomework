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

/* current number of files */
int num_files;

/* private functions */
int init() {
	int i;
	//calculate number of blocks
	NUM_BLOCKS = MAX_FILES + 3; //free blocks + root dir + fat + free blocks

	//create free list
	free_list = list_create(destroy_free_list);

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
	block_status_t* temp = (block_status_t*) malloc(
			NUM_BLOCKS * sizeof(block_status_t));

	// init all to USED
	for (i = 0; i < NUM_BLOCKS; i++) {
		temp[i] = USED;
	}

	for (i = 0; i < list_length(free_list); i++) {
		block = list_item_int(free_list, i);
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
	int i;
	block_status_t* temp = (block_status_t*) malloc(
			NUM_BLOCKS * sizeof(block_status_t));

	list_empty(free_list);

	if (read_blocks(FREE_INDEX, 1, temp)) {
		for (i = 0; i < NUM_BLOCKS; i++) {
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
	int i;
	init(); //init data structures

	if (fresh) {
		// new FS
		if (init_fresh_disk(NAME, BLOCK_SIZE, NUM_BLOCKS)) {
			num_files = 0;
			//make sure everything is empty
			for(i = 0; i < MAX_FILES; i++){
				directory[i].date_modified = "";
				directory[i].fat_index = 0;
				directory[i].file_size = 0;
				directory[i].filename = "";

				fat[i].block_index = 0;
				fat[i].next_fat_entry = 0;

				file_descriptor[i].fat_index = 0;
				file_descriptor[i].read_block = 0;
				file_descriptor[i].read_offset = 0;
				file_descriptor[i].write_block = 0;
				file_descriptor[i].write_index = 0;
			}
			//all blocks except the first 3 are free
			for (i = 3; i < NUM_BLOCKS; i++)
				list_append_int(free_list, i);

			//write status FS blocks
			write_directory();
			write_fat();
			write_free();

		} else {
			perror("error initializing new file system");
		}
	} else {
		// existing FS
		if (init_disk(NAME, BLOCK_SIZE, NUM_BLOCKS)) {
			//get status FS blocks
			read_directory();
			read_fat();
			read_free();

			//init file descriptor table
			for (i = 0; i < MAX_FILES; i++) {
				if(directory[i].fat_index != 0){
					num_files++;
					file_descriptor[i].fat_index = directory[i].fat_index;
				}
			}
		} else {
			perror("error initializing existing file system");
		}
	}

}
void sfs_ls() {
	int i;
	printf("total files: %d\n\n", num_files);

	for(i = 0; i < MAX_FILES; i++){
		printf("%s\t%d\t%s\n",directory[i].filename, directory[i].file_size, directory[i].date_modified);
	}
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

