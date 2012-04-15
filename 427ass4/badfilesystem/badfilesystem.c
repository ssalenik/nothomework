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

int initialized = 0; //flag to test whether or not mem stucts have been initialized yet

/***********************************************/
/*           private functions                 */
/***********************************************/

/**
 * initializes the needed data structures in memory
 */
int init() {
	//calculate number of blocks
	NUM_BLOCKS = MAX_FILES + 3; //free blocks + root dir + fat + free blocks

	//create free list
	free_list = list_create(destroy_free_list);

	//create other stuff

	//determine block size based on the largest needed data structure
	BLOCK_SIZE = MAX_FILES * sizeof(directory_entry_t);
	if (MAX_FILES * sizeof(file_descriptor_t) > BLOCK_SIZE)
		BLOCK_SIZE = MAX_FILES * sizeof(file_descriptor_t);
	if (NUM_BLOCKS * sizeof(fat_entry_t) > BLOCK_SIZE)
		BLOCK_SIZE = NUM_BLOCKS * sizeof(fat_entry_t);
	if (NUM_BLOCKS * sizeof(int) > BLOCK_SIZE)
		BLOCK_SIZE = NUM_BLOCKS * sizeof(int); // should never happen

	/* init these arrays to be the BLOCK SIZE in order memcpy from the FS properly */
	directory = (directory_entry_t*) malloc(BLOCK_SIZE);
	fat = (fat_entry_t*) malloc(BLOCK_SIZE);
	file_descriptor = (file_descriptor_t*) malloc(
			MAX_FILES * sizeof(file_descriptor_t));

	//set fat for first 3
	fat[1].block_index = 1;
	fat[2].block_index = 2;

	printf("block size: %d\nnumber of blocks: %d\n\n", BLOCK_SIZE, NUM_BLOCKS);

	return 0;
}

/**
 * writes the file directory to the proper FS block
 */
int write_directory() {
	if (write_blocks(DIRECTORY_INDEX, 1, directory))
		return 0;
	else
		return -1;
}
/**
 * reads the file directory from the proper FS block
 */
int read_directory() {
	if (read_blocks(DIRECTORY_INDEX, 1, directory))
		return 0;
	else
		return -1;
}
/**
 * writes the FAT to the proper FS block
 */
int write_fat() {
	if (write_blocks(FAT_INDEX, 1, fat))
		return 0;
	else
		return -1;
	return 0;
}
/**
 * reads the FAT from the proper FS block
 */
int read_fat() {
	if (read_blocks(FAT_INDEX, 1, fat))
		return 0;
	else
		return -1;
	return 0;
}
/**
 * writes the free block list to the proper FS block
 */
int write_free() {
	int i = 0;
	int block;
	block_status_t* temp = (block_status_t*) malloc(BLOCK_SIZE);

	// init all to USED
	for (i = 0; i < NUM_BLOCKS; i++) {
		temp[i] = USED;
	}

	// check which are free and mark them accordingly
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

/**
 * reads the free block list from the proper FS block
 */
int read_free() {
	int i = 0;

	block_status_t* temp = (block_status_t*) malloc(BLOCK_SIZE);

	list_empty(free_list);

	//using linked-list, so only append the free blocks
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
		//find empty slot in directory
		file_index = 0;
		while (directory[file_index].fat_index != 0)
			file_index++;

		//create file entry
		strcpy(directory[file_index].filename, name);
		directory[file_index].fat_index = list_shift_int(free_list);
		fat[file_index].block_index = directory[file_index].fat_index;
		fat[file_index].next_fat_entry = 0;

		//create file descriptor entry
		file_descriptor[file_index].fat_index = directory[file_index].fat_index;
		file_descriptor[file_index].read_index = 0;
		file_descriptor[file_index].write_index = 0;

		write_directory();
		write_fat();
		write_free();

		return file_index;
	}
}

/***********************************************/
/*             public functions                */
/***********************************************/
void mksfs(int fresh) {
	int i = 0;
	//first check if data structs have been initialized yet or not
	if (initialized) {
		//free them first
		free(directory);
		free(fat);
		free(file_descriptor);
		list_destroy(&free_list);
		free(free_list);
	} else {
		initialized = 1;
	}
	init(); //init data structures

	if (fresh) {
		printf("creating new FS: %s\n\n", NAME);
		// new FS
		if (init_fresh_disk(NAME, BLOCK_SIZE, NUM_BLOCKS) == 0) {
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
			errno = EIO;
			perror("error initializing new file system");
		}
	} else {
		printf("recovering existing FS: %s\n\n", NAME);
		// existing FS
		if (init_disk(NAME, BLOCK_SIZE, NUM_BLOCKS) == 0) {
			//get status FS blocks
			read_directory();
			read_fat();
			read_free();

			//init file descriptor table
			for (i = 0; i < MAX_FILES; i++) {
				if (directory[i].fat_index != 0) {
					file_descriptor[i].fat_index = directory[i].fat_index;
				}
			}
		} else {
			errno = EIO;
			perror("error initializing existing file system");
		}
	}

}

/**
 * lists all of the files currently in the directory
 *
 * prints out the file ID, file name, and file size (and file modified date, if implemented)
 */
void sfs_ls() {
	int i = 0;

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

	// look for file in directory
	while (strcmp(directory[file_index].filename, name)
			&& ++file_index < MAX_FILES)
		;

	if (file_index < MAX_FILES) {
		//file already exists, set to append mode
		file_descriptor[file_index].fat_index = directory[file_index].fat_index;
		file_descriptor[file_index].write_index =
				directory[file_index].file_size;
		file_descriptor[file_index].read_index = 0;

		return file_index;
	} else {
		// no such file
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

/**
 * writes (the given length) of the given buffer to the given file
 * starting from the current location of the write pointer
 *
 * returns with error message if file is not open
 *
 * will write to blocks until the FS runs out of space
 * ie: it does not check if there is enough space before hand
 */
void sfs_fwrite(int fileID, char *buf, int length) {
	char *buf_new;
	int block_current = 0;
	int offset_current = 0;
	int bytes_write = 0;
	int bytes_written = 0; // number of bytes written; should = length at end of operation

	//check that file exists and is open
	if (file_descriptor[fileID].fat_index == 0) {
		errno = EINVAL;
		perror("specified file is not open for writing!");
	} else {
		//figure out which block to start writing in
		//as well as the offset
		block_current = file_descriptor[fileID].fat_index;
		offset_current = file_descriptor[fileID].write_index;
		while (offset_current > BLOCK_SIZE) {
			block_current = fat[block_current].next_fat_entry;
			offset_current -= BLOCK_SIZE;
		}

		//now start writing!
		while (bytes_written != length) {

			// first read the current block
			buf_new = (char*) malloc(BLOCK_SIZE * sizeof(char));
			read_blocks(block_current, 1, buf_new);
			//check how much data to write
			bytes_write =
					(length - bytes_written > BLOCK_SIZE - offset_current) ?
							BLOCK_SIZE - offset_current :
							length - bytes_written;
			// overwrite with the new data
			memcpy(buf_new + offset_current, buf + bytes_written, bytes_write);
			// write to the block
			write_blocks(block_current, 1, buf_new);

			free(buf_new);

			bytes_written += bytes_write;
			file_descriptor[fileID].write_index += bytes_written;
			//update files size if increased
			if (file_descriptor[fileID].write_index
					> directory[fileID].file_size)
				directory[fileID].file_size =
						file_descriptor[fileID].write_index;

			//offset for next block is now 0
			offset_current = 0;

			//check if we need to get another block
			if (bytes_written != length) {
				//check if we need to allocate another block
				if (fat[block_current].next_fat_entry == 0) {
					//check if there are free blocks left
					if (list_length(free_list) != 0) {
						fat[block_current].next_fat_entry = list_shift_int(
								free_list);
						block_current = fat[block_current].next_fat_entry;
					} else {
						// no more free blocks!
						directory[fileID].file_size =
								file_descriptor[fileID].write_index;
						errno = ENOMEM;
						perror("cannot finish writing buffer");
					}
				} else {
					// else continue writing in next block
					block_current = fat[block_current].next_fat_entry;
				}
			}
		}
		//write changes to the status blocks
		write_directory();
		write_fat();
		write_free();
	}
}

/**
 * reads from the given file the given length into the given buffer
 * starting from the current read pointer
 *
 * returns with error message if the file is not open
 *
 * will read into the buffer until the end of the file
 * ie: it does not check if the desired length goes out of the file bounds
 * ahead of time
 */
void sfs_fread(int fileID, char *buf, int length) {
	char *buf_new;
	int block_current = 0;
	int offset_current = 0;
	int bytes_to_read;
	int bytes_read = 0;

	//check that file exists and is open
	if (file_descriptor[fileID].fat_index == 0) {
		errno = EINVAL;
		perror("specified files is not open for reading!");
	} else {

		//figure out which block to start reading in
		//as well as the offset
		block_current = file_descriptor[fileID].fat_index;
		offset_current = file_descriptor[fileID].read_index;
		while (offset_current > BLOCK_SIZE) {
			block_current = fat[block_current].next_fat_entry;
			offset_current -= BLOCK_SIZE;
		}

		//now start reading!
		while (bytes_read != length) {

			// first read the current block
			buf_new = (char*) malloc(BLOCK_SIZE * sizeof(char));
			read_blocks(block_current, 1, buf_new);
			//check how much data to
			bytes_to_read =
					(length - bytes_read > BLOCK_SIZE - offset_current) ?
							BLOCK_SIZE - offset_current : length - bytes_read;
			// overwrite with the new data
			memcpy(buf + bytes_read, buf_new + offset_current, bytes_to_read);

			free(buf_new);

			bytes_read += bytes_to_read;
			file_descriptor[fileID].read_index += bytes_read;

			//offset for next block is now 0
			offset_current = 0;

			//check if we need to get another block
			if (bytes_read != length) {
				// else continue writing in next block
				block_current = fat[block_current].next_fat_entry;
				if (block_current == 0) {
					errno = ESPIPE;
					perror("trying to read beyond last block of file");
				}
			}

		}
	}
}

/**
 * moves the write and read pointers of the give file to the given location
 * (from the beginning of the file)
 *
 * returns with error message if the files is not open, or if the give location
 * is outside fo the file boundaries
 */
void sfs_fseek(int fileID, int loc) {
	//check that file exists and is open
	if (file_descriptor[fileID].fat_index == 0) {
		errno = EINVAL;
		perror("specified files is not open");
	}
	//check that loc is not out of bounds of file
	else if (directory[fileID].file_size < loc) {
		errno = EFAULT;
		perror("desired location greater than file size");
	} else {
		file_descriptor[fileID].read_index = loc;
		file_descriptor[fileID].write_index = loc;
	}

}

/**
 * deletes the given file from the directory
 *
 * returns 0 on success; -1 on failure, if the given file is not found
 *
 * note: it does not reset the actual contents of the FS, just marks the appropriate entries as empty
 */
int sfs_remove(char *file) {
	int file_index = 0;
	int block_current = 0;

	// look for file in directory
	while (strcmp(directory[file_index].filename, file)
			&& ++file_index < MAX_FILES)
		;

	if (file_index < MAX_FILES) {
		//file exists, close and remove
		file_descriptor[file_index].fat_index = 0;

		//traverse FAT and free blocks
		block_current = directory[file_index].fat_index;
		while (block_current != 0) {
			//add to free list
			list_append_int(free_list, block_current);
			block_current = fat[file_index].next_fat_entry;
			fat[file_index].next_fat_entry = 0;
		}

		//remove from directory
		strcpy(directory[file_index].filename, "");
		directory[file_index].fat_index = 0;
		directory[file_index].file_size = 0;

		return 0;
	} else {
		// no such file
		return -1;
	}
}

