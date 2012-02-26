/*
 * main.c
 *
 * ssshell:
 * Super Simple Shell or Stepan Salenikovich Shell
 *
 * Stepan Salenikovich
 * ID: 260326129
 * Feb 02, 2012
 *
 */
#include "main.h"

// env var from unistd.h
extern char** environ;

char* username;

/*
 * prints the prompt
 */
int prompt() {
	fputs("[", stderr);
	fputs(username, stderr);
	fputs("] > ", stderr);

	return 0;
}

/*
 * gets the input from the user
 * returns 0 if succefull
 * returns 1 if it gets EOF
 * returns -1 if error?
 */
int getinput(char** input_buffer) {
	/*if (feof(stdin) == 1) {
	 getc(stdin);
	 return 1;
	 }*/

	size_t n = 0;
	int return_code;

	return_code = getline(input_buffer, &n, stdin);

	if (return_code > -1) {
		add_history(*input_buffer);
		//did that work!?
		//printf("input string is : %s", *input_buffer);

		return 0;
	} else if (return_code == -1) {
		return 1;
	} else {
		return -1;
	}
}

/*
 * prints env vars
 */
int printenv() {
	char** env;
	for (env = environ; *env != 0; env++) {
		char* thisEnv = *env;
		printf("%s\n", thisEnv);
	}
	return 0;
}

/*
 * prints the current history as maintained by the readline/history lib
 */
int printhist() {
	HIST_ENTRY** history = history_list();
	HIST_ENTRY* entry;
	char* line;
	int line_number;

	for (line_number = 0; line_number < history_length; line_number++) {
		entry = history[line_number];
		line = entry->line;
		printf("%5d  %s", line_number, line);
	}
	return 0;
}

int main(int argc, char** argv) {
	char* input_buffer = NULL;
	int loop = 1;

	//get user
	username = getenv("USER");

	int exit_code;

	using_history(); //initiate history vars

	while (loop) {
		prompt(); //print promt
		exit_code = getinput(&input_buffer); //get input from user
		//if EOF is detected, exit
		if (exit_code == 1) {
			//printf("\n"); <- fails some tests whith this ON
			loop = 0;
		} else {
			command cmd_next;
			// parse input and get command
			parseinput(input_buffer, &cmd_next);

			// execute command
			switch (cmd_next.cmd_type) {
			case echo:
				//TODO
				fputs("echo\n", stderr);
				break;
			case cd:
				//TODO
				fputs("cd\n", stderr);
				break;
			case history:
				exit_code = printhist();
				break;
			case pwd:
				exit_code = printf("%s\n", getenv("PWD"));
				break;
			case set:
				//TODO
				//just acts like env for now
				exit_code = printenv();
				break;
			case env:
				exit_code = printenv();
				break;
			case unset:
				//TODO
				fputs("unset\n", stderr);
				break;
			case pushd:
				//TODO
				fputs("pushd\n", stderr);
				break;
			case popd:
				//TODO
				fputs("popd\n", stderr);
				break;
			case external:
				//TODO execute external command
				fputs("external\n", stderr);
				break;
			case none:
				//do nothing
				break;
			default:
				loop = 0;
				break;
			}
		}

		//set the exit code $?
		char code[3];
		sprintf(code, "%d", exit_code);
		setenv("?", code, 0);

		//free memory
		free(input_buffer);
	}
	return 0;
}

