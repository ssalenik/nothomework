/*
 * README.txt
 *
 *  Created on: Apr 14, 2012
 *      Author: Stepan Salenikovich
 *		ID: 260326129
 *
 *		Submission and usage details for ECSE 427 - assignment 4 and the "badfilesystem"
 */
 
 According to the provided test program - everything works!
 
 Usage Details:
 The max number of files can be specified in badfilesystem.h
 This corresponds to the number of blocks which will be allocated, minus 3.  As 3 more blocks will be needed to store the file directory, the FAT, and the free blocks array.
 
 The block size is calculated based on the max number of files specified.  This is done because the directory, the FAT, and the free block array are each stored in only one block.  Thus whichever is the biggest one (and the size of each of these data structures depends on the max number of files) determines the block size.  The resulting number of blocks and the block size are printed out when initializing the file system.
 
 Like in *nix, extensions are not independently interpreted and are thus considered as simply part of the file name.  Thus the max file length includes the extension length in it.
 
 
 Things of Note:
 In memory, the free block list is implemented as a linked list, for quick searching. However, when written to the disk (file) it is written as an array.
 
 The submission contains the files system file, "badsfs", created when running the test program.  It can be deleted if you wish to test creating the file from scratch.