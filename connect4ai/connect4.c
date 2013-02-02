#import "connect4.h"

/* game state */
char o = 'o';	//white
char x = 'x';	//black
char e = '.';	//empty

char game_state[8][8] = {	{' ', '1', '2', '3', '4', '5', '6', '7'}, \
							{'1', 'o', '.', '.', '.', '.', '.', 'x'}, \
							{'2', 'x', '.', '.', '.', '.', '.', 'o'}, \
							{'3', 'o', '.', '.', '.', '.', '.', 'x'}, \
							{'4', 'x', '.', '.', '.', '.', '.', 'o'}, \
							{'5', 'o', '.', '.', '.', '.', '.', 'x'}, \
							{'6', 'x', '.', '.', '.', '.', '.', 'o'}, \
							{'7', 'o', '.', '.', '.', '.', '.', 'x'}};

/*
 * prints the game state
 */
int showstate() {
	int i, j;
	for(i = 0; i < 8; i++) {
		for(j = 0; j < 8; j++) {
			printf("%c ", game_state[i][j]);
		}
		printf("\n");
	}

	return 0;
}

/* prints the help */
int printhelp() {
	printf("usage: connect 4 [<args>] [<input file>]\n\n"
			"args:\n"
			"\tw\tuser plays as white\n"
			"\tb\tuser plays as black\n\n"
			"in-game commands:\n"
			"\tmove:\t<x,y,direction>; eg: 12W - will move the piece at <1,2> to the right\n"
			"\thelp:\ttype \'help\'\n"
			"\texit:\ttype \'exit\'\n\n"
			"input file:\n"
			"\tThe input file should contain comma sperated starting locations for each piece.\n"
			"\tThe first line contains the positions for all of the white pieces, and then\n"
			"\tthe second line contains the position for all the black pieces.\n"
			"\texample for indicating the default staring position:\n"
			"\t\t11,13,15,17,72,74,76\n"
			"\t\t12,14,16,71,73,75,77\n");

	return 0;
}

/* gets input from user */
int getinput() {

	return 0;
}
/* validates entered move */
int validatemove() {
	return 0;
}
/* changes game state to reflect move*/
int executemove() {

	return 0;
}

int main(int argc, char** argv) {
	showstate();
	printhelp();

	return 0;
}