#include "ai.h"

/* checks if end-game has been reached
 * returns:
 * 0 - if not endgame
 * 1 - if white won
 * 2 - if black won
 */
int check_endgame(uint64_t board) {
	int winner = 0;

	uint64_t copy = board & (board >> 1); // horizontal check


	return 0;
}