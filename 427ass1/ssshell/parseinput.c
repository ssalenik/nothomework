/*
 * parseinput.c
 *
 * ssshell:
 * Super Simple Shell or Stepan Salenikovich Shell
 *
 * Stepan Salenikovich
 * ID: 260326129
 * Feb 02, 2012
 *
 */

#include "parseinput.h"

char* commands[10] = { "echo", "cd", "pwd", "history", "set", "env", "unset", "pushd",
		"popd", "exit" };

/*
 * parses string into command(s) and arguments
 */
int parseinput(char* inputbuffer, command* command) {
	int argc = 0;
	char* argv[32];

	// spaces turn into nulls
	// cmd_array[0] points to first non null char
	argv[0] = strtok(inputbuffer, " \n\t");

	if (argv[0] != NULL) {
		argc++;

		//TODO: parse args, piping, redirection

		command->argv = argv;
		command->argc = argc;
		command->cmd_type = typecheck(argv[0]);
	} else {
		// no command
		command->cmd_type = none;
		command->argc = 0;
		command->argv = NULL;
	}

	return 0;
}

/*
 * checks and returns type of command
 */
type typecheck(char* cmd_name) {
	type cmd = none;

	if(0 == strcmp(cmd_name, commands[0])){
		cmd = echo;
	} else if(0 == strcmp(cmd_name, commands[1])){
		cmd = cd;
	} else if(0 == strcmp(cmd_name, commands[2])){
		cmd = pwd;
	} else if(0 == strcmp(cmd_name, commands[3])){
		cmd = history;
	} else if(0 == strcmp(cmd_name, commands[4])){
		cmd = set;
	} else if(0 == strcmp(cmd_name, commands[5])){
		cmd = env;
	} else if(0 == strcmp(cmd_name, commands[6])){
		cmd = unset;
	} else if(0 == strcmp(cmd_name, commands[7])){
		cmd = pushd;
	} else if(0 == strcmp(cmd_name, commands[8])){
		cmd = popd;
	} else if(0 == strcmp(cmd_name, commands[9])){
		cmd = exitshell;
	} else {
		cmd = external;
	}

	return cmd;
}
