#import "connect4.h"

/* game state
 * 0 = empty
 * 1 = o = white
 * 2 = x = black
 */
/*int game_state[7][7] = {{ 1, 2, 0, 0, 0, 0, 2}, \
						{ 2, 0, 0, 0, 0, 0, 1}, \
						{ 1, 0, 0, 0, 0, 0, 2}, \
						{ 2, 0, 0, 0, 0, 0, 1}, \
						{ 1, 0, 0, 0, 0, 0, 2}, \
						{ 2, 0, 0, 0, 0, 0, 1}, \
						{ 1, 0, 0, 0, 0, 0, 2}}; */

char game_state[8][8] = {	{' ', '1', '2', '3', '4', '5', '6', '7'}, \
							{'1', 'o', '.', '.', '.', '.', '.', 'x'}, \
							{'2', 'x', '.', '.', '.', '.', '.', 'o'}, \
							{'3', 'o', '.', '.', '.', '.', '.', 'x'}, \
							{'4', 'x', '.', '.', '.', '.', '.', 'o'}, \
							{'5', 'o', '.', '.', '.', '.', '.', 'x'}, \
							{'6', 'x', '.', '.', '.', '.', '.', 'o'}, \
							{'7', 'o', '.', '.', '.', '.', '.', 'x'}};

/* output */
char w = 'o';	//white
char b = 'x';	//black
char e = '.';	//empty

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

int playermove() {
	
}

int main(int argc, char** argv) {
	showstate();

	return 0;
}