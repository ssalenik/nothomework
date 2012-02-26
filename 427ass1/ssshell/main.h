/*
 * main.h
 *
 * ssshell:
 * Super Simple Shell or Stepan Salenikovich Shell
 *
 * Stepan Salenikovich
 * ID: 260326129
 * Feb 02, 2012
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include <readline/readline.h>
#include <readline/history.h>

#include "parseinput.h"

#endif /* MAIN_H_ */

/* prototypes */
int prompt();
int getinput(char** input_buffer);
int printenv();
int printhist();
