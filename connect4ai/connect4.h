/**
 * Stepan Salenikovich
 * ECSE 526
 * Assignment 1
 *
 * connect4.h
 */
#ifndef CONNECT4_H_
#define CONNECT4_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sys/time.h>

#include "ai.h"
#include "printstate.h"

#endif /* CONNECT4_H_ */

/* prints the help */
int printhelp();

/* parse input file and save state into bitboards */
int parsefile(char *ifile);