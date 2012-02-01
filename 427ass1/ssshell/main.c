//simple shell
#include "main.h"

// env var from unistd.h
extern char** environ;

char* username;
char* machine;
char* dir;

/*
 * prints the prompt
 */
int prompt() {
	printf("blah");
	return 0;
}

/*
 * gets the input from the user
 * returns 0 if succefull
 * returns 1 if it gets EOF
 * returns -1 if error?
 */
int getinput(char* input_buffer) {
	/*if (feof(stdin) == 1) {
	 getc(stdin);
	 return 1;
	 }*/

	size_t n = 0;
	int return_code;

	return_code = getline(&input_buffer, &n, stdin);

	if (return_code > -1) {
		add_history(input_buffer);
		//did that work!?
		printf("input string is : %s", input_buffer);

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
void printenv() {
	char** env;
	for (env = environ; *env != 0; env++) {
		char* thisEnv = *env;
		printf("%s\n", thisEnv);
	}
}

/*
 * prints the current history as maintained by the readline/history lib
 */
void printhist() {
	HIST_ENTRY** history = history_list();
	HIST_ENTRY* entry;
	char* line;
	int line_number;

	for (line_number = 0; line_number <= history_length; line_number++) {
		entry = history[line_number];
		line = entry->line;
		printf("%5d  %s", line_number, line);
	}
	free(history);
	free(entry);
	free(line);
}

int main(int argc, char** argv) {
	char* input_buffer = NULL;

	int exit_code;

	using_history(); //initiate history vars

	/*printenv();
	 setenv("FOO","test", 0);
	 unsetenv("FOO");*/
	int i = 0;

	for (i = 0; i < 11; i++) {
		exit_code = getinput(input_buffer);
		if (exit_code == 1) {
			printf("exiting\n");
			return 1;
		}

		free(input_buffer);
	}

	printhist();

	return 0;
}

