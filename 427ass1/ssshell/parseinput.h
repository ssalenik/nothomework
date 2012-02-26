/*
 * parseinput.h
 *
 * ssshell:
 * Super Simple Shell or Stepan Salenikovich Shell
 *
 * Stepan Salenikovich
 * ID: 260326129
 * Feb 02, 2012
 *
 */

#ifndef PARSEINPUT_H_
#define PARSEINPUT_H_

#include <string.h>
#include <stdio.h>

#endif /* PARSEINPUT_H_ */

/* internal commands */
typedef enum {
	echo, cd, pwd, history, set, env, unset, pushd, popd, exitshell, external, none
} type;

typedef struct {
	type cmd_type;
	int argc;
	char** argv;
}command;

int parseinput(char* inputbuffer, command* command);
type typecheck(char* cmd_name);
