#include "printstate.h"

/* game state */
char w = 'o';	//white
char b = 'x';	//black
char e = '.';	//empty

char empty_state[8][8] = {	{' ', '1', '2', '3', '4', '5', '6', '7'}, \
							{'1', '.', '.', '.', '.', '.', '.', '.'}, \
							{'2', '.', '.', '.', '.', '.', '.', '.'}, \
							{'3', '.', '.', '.', '.', '.', '.', '.'}, \
							{'4', '.', '.', '.', '.', '.', '.', '.'}, \
							{'5', '.', '.', '.', '.', '.', '.', '.'}, \
							{'6', '.', '.', '.', '.', '.', '.', '.'}, \
							{'7', '.', '.', '.', '.', '.', '.', '.'}};

int bitboard_lookup[56][2] = { \
{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1},{-1,-1},
{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2},{-1,-1},
{1,3},{2,3},{3,3},{4,3},{5,3},{6,3},{7,3},{-1,-1},
{1,4},{2,4},{3,4},{4,4},{5,4},{6,4},{7,4},{-1,-1},
{1,5},{2,5},{3,5},{4,5},{5,5},{6,5},{7,5},{-1,-1},
{1,6},{2,6},{3,6},{4,6},{5,6},{6,6},{7,6},{-1,-1},
{1,7},{2,7},{3,7},{4,7},{5,7},{6,7},{7,7},{-1,-1}};

/*
 * prints the game state from the bitboard representation
 */
int showstate(uint64_t white_bits[], uint64_t black_bits[]) {
	int i, j;
	char state_tmp[8][8];

	// copy empty state
	memcpy(state_tmp, empty_state, 64*sizeof(char));

	// get all the pieces
	uint64_t white_tmp;
	uint64_t black_tmp;
	int white_idx = 0;
	int black_idx = 0;
	for(i = 0; i < 7; i++) {
		white_tmp = white_bits[i];
		black_tmp = black_bits[i];
		white_idx = 0;
		black_idx = 0;
		// get the bit position of the white piece
		while(white_tmp != 1) {
			white_tmp = white_tmp >> 1;
			white_idx++;
		}
		// get the bit position of the black piece
		while(black_tmp != 1) {
			black_tmp = black_tmp >> 1;
			black_idx++;
		}
		// mark the board
		state_tmp[bitboard_lookup[white_idx][1]][bitboard_lookup[white_idx][0]] = w;
		state_tmp[bitboard_lookup[black_idx][1]][bitboard_lookup[black_idx][0]] = b;
	}

	// now print the board
	printf("\n");
	for(i = 0; i < 8; i++) {
		for(j = 0; j < 8; j++) {
			printf("%c ", state_tmp[i][j]);
		}
		printf("\n");
	}

	return 0;
}