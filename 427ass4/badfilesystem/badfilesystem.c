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
	//calculate number of blocks
	NUM_BLOCKS = MAX_FILES + 3; //free blocks + root dir + fat + free blocks

	//create free list
	free_list = list_create(destroy_free_list);

	//create other stuff
	directory = (directory_entry_t*) malloc(
			MAX_FILES * sizeof(directory_entry_t));
	fat = (fat_entry_t*) malloc(NUM_BLOCKS * sizeof(fat_entry_t));
	file_descriptor = (file_descriptor_t*) malloc(
			MAX_FILES * sizeof(file_descriptor_t));

	//set fat for first 3
	fat[1].block_index = 1;
	fat[2].block_index = 2;

	//determine block size
	BLOCK_SIZE = MAX_FILES * sizeof(directory_entry_t);
	if (MAX_FILES * sizeof(file_descriptor_t) > BLOCK_SIZE)
		BLOCK_SIZE = MAX_FILES * sizeof(file_descriptor_t);
	if (NUM_BLOCKS * sizeof(fat_entry_t) > BLOCK_SIZE)
		BLOCK_SIZE = NUM_BLOCKS * sizeof(fat_entry_t);
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
	int i = 0;
	int block;

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
	int i = 0;
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

/**
 * creates and opens a file
 *
 * returns file index in descriptor table on success; otherwise -1
 */
int create_file(char *name) {
	int file_index = 0;
	//check if there is space left
	if (list_length(free_list) == 0) {
		perror("no space left in file system!");
		return -1;
	} else {
		num_files++;
		//find empty slot in directory
		file_index = 0;
		while (directory[file_index].fat_index != 0)
			file_index++;

		//create file entry
		strcpy(directory[file_index].filename, name);
		directory[file_index].fat_index = list_shift_int(free_list);
		fat[file_index].block_index = directory[file_index].fat_index;
		fat[file_index].next_fat_entry = 0;

		//create file descritor entry
		file_descriptor[file_index].fat_index = directory[file_index].fat_index;

		write_directory();
		write_fat();
		write_free();

		return file_index;
	}
}

/***************************************/
/*         public functions            */
/***************************************/
void mksfs(int fresh) {
	int i = 0;
	init(); //init data structures

	if (fresh) {
		// new FS
		if (init_fresh_disk(NAME, BLOCK_SIZE, NUM_BLOCKS) == 0) {
			num_files = 0;
			//make sure everything is empty
			for (i = 0; i < MAX_FILES; i++) {
				strcpy(directory[i].date_modified, "");
				directory[i].fat_index = 0;
				directory[i].file_size = 0;
				strcpy(directory[i].filename, "");

				fat[i].block_index = 0;
				fat[i].next_fat_entry = 0;

				file_descriptor[i].fat_index = 0;
				file_descriptor[i].read_index = 0;
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
				if (directory[i].fat_index != 0) {
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
	int i = 0;
	printf("total files: %d\n\n", num_files);

	for (i = 0; i < MAX_FILES; i++) {
		if (directory[i].fat_index != 0) {
			printf("%d\t%s\t%d\t%s\n", i, directory[i].filename,
					directory[i].file_size, directory[i].date_modified);
		}
	}
}

/**
 * opens/creates a file
 *
 * returns file index in descriptor table on success; otherwise -1
 */
int sfs_fopen(char *name) {
	int file_index = 0;

	//check if the file exists
	if (num_files != 0) {
		// look for file in directory
		while (strcmp(directory[file_index].filename, name)
				&& ++file_index < MAX_FILES)
			;

		if (file_index < MAX_FILES) {
			//file already exists, set to append mode
			file_descriptor[file_index].fat_index =
					directory[file_index].fat_index;
			file_descriptor[file_index].write_index =
					directory[file_index].file_size;

			return file_index;
		} else {
			// no such file
			return create_file(name);
		}
	} else {
		// no files at all, so create one
		return create_file(name);
	}

}
/**
 * closes file with specified file ID
 *
 * checks if file ID is out of bounds
 */
void sfs_fclose(int fileID) {
	if (fileID >= MAX_FILES || fileID < 0) {
		perror("file index out of bounds");
	} else
		file_descriptor[fileID].fat_index = 0;
}
void sfs_fwrite(int fileID, char *buf, int length) {
	char *buf_new;
	int block_current = 0;
	int blocks = 1; // the number of blocks the files takes up
	int offset_current = 0;
	int i;
	int size_new;
	int bytes_write = 0;
	int bytes_written = 0; // number of bytes written; should = length at end of operation
	int status = 0; // used to store return value of operations

	//check that file exists and is open
	if (file_descriptor[fileID].fat_index == 0) {
		errno = EINVAL;
		perror("specified files is not open for writing!");
	} else {
		//calculate new size of file
		size_new = directory[fileID].file_size;
		if (file_descriptor[fileID].write_index + length > size_new)
			size_new = file_descriptor[fileID].write_index + length;

		//figure out which block to start writing in
		//as well as the offset
		block_current = file_descriptor[fileID].fat_index;
		offset_current = file_descriptor[fileID].write_index;
		while (offset_current > BLOCK_SIZE) {
			block_current = fat[block_current].next_fat_entry;
			blocks++;
			offset_current -= BLOCK_SIZE;
		}

		//now start writing!
		while (bytes_written != length) {

			// first read the current block
			buf_new = (char*) malloc(BLOCK_SIZE * sizeof(char));
			read_blocks(block_current, 1, buf_new);
			//check how much data to write
			bytes_write = (length - bytes_written > BLOCK_SIZE - offset_current)? BLOCK_SIZE - offset_current : length - bytes_written;
			// overwrite with the new data
			memcpy(buf_new + offset_current, buf + bytes_written,
					bytes_write);
			// write to the block
			write_blocks(block_current, 1, buf_new);

			bytes_written += bytes_write;
			file_descriptor[fileID].write_index += bytes_written;
			//update files size if increased
			if(file_descriptor[fileID].write_index > directory[fileID].file_size)
				directory[fileID].file_size = file_descriptor[fileID].write_index;

			//offset for next block is now 0
			offset_current = 0;


			//check if we need to get another block
			if(bytes_written != length){
				//check if we need to allocate another block
				if(fat[block_current].next_fat_entry == 0){
					//check if there are free blocks left
					if( list_length(free_list) != 0){
						fat[block_current].next_fat_entry = list_shift_int(free_list);
						block_current = fat[block_current].next_fat_entry;
					} else {
						// no more free blocks!
						directory[fileID].file_size = file_descriptor[fileID].write_index;
						errno = ENOMEM;
						perror("cannot finish writing buffer");
					}
				} else {
					// else continue writing in next block
					block_current = fat[block_current].next_fat_entry;
				}

			}

		}

	}
}
void sfs_fread(int fileID, char *buf, int length) {

}
void sfs_fseek(int fileID, int loc) {

}
int sfs_remove(char *file) {

	return 0;
}

