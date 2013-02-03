#include "ai.h"

#define DEBUG 1

// borders
#define BORDER_W 18085043209519168
#define BORDER_E 282578800148737
#define BORDER_N 127
#define BORDER_S 35747322042253312 

KHASH_MAP_INIT_INT64(64, list_t*);
khash_t(64) *h;

/* checks if end-game has been reached
 * returns:
 * 0 - if no win
 * 1 - if win (horizontal)
 * 2 - if win (vertical)
 * 3 - if win (diagonal \ )
 * 4 - if win (diagonal / )
 *
 * algo adapted from 	
 */
int check_endgame(uint64_t board) {
	// horizontal check
	uint64_t copy = board & (board >> 1);
	if (copy & (copy >> 2))
    	return 1;
    // vertical check	
    copy = board & (board >> 8);
    if (copy & (copy >> 2*8))
    	return 2;
    // diagonal / check
    copy = board & (board >> 7);
    if (copy & (copy >> 2*7))
    	return 3;
    // diagonal \ check
    copy = board & (board >> 9);
    if (copy & (copy >> 2*9))
    	return 4;

    return 0;
}

uint64_t generate_key(uint64_t w, uint64_t b) {
	return w ^ (b << 15);
}

int ai_turn(turn_t turn,
			uint64_t bitboard_white,
			uint64_t bitboard_black,
			uint64_t white_bits[],
			uint64_t black_bits[],
			int d_cutoff,
			int* states_visited) {
	h = kh_init(64);  // allocate a hash table
	has_visited_state(bitboard_white, bitboard_black); // save the initial state

	minimax(turn, bitboard_white, bitboard_black, white_bits, black_bits, 0);

	return 0;
}

/* function which checks if the state given has already been visited
 * does this by checking the hash table of states
 * if state is new, stores it in the hash table
 *
 * @return - 1 if new, 0 if visited already
 */
int has_visited_state(uint64_t bitboard_white, uint64_t bitboard_black) {
	int ret;
	khiter_t k;

	uint64_t key = generate_key(bitboard_white, bitboard_black);

	k = kh_put(64, h, key, &ret);

	// if ret != 0, its a new key
	if(ret) {
		// new key, store state
		// create linked list node to do so
		#if DEBUG == 1
		printf("new key\n");
		#endif

		list_t *new_list;
		new_list = malloc(sizeof(list_t));
		new_list->b_white = bitboard_white;
		new_list->b_black = bitboard_black;
		new_list->next = NULL;
		kh_value(h, k) = new_list;
		return 1;

	} else {
		// key already exists, check the state
		#if DEBUG == 1
		printf("old key, listing visited states:\n");
		#endif

		list_t *old_list;
		list_t *next_state;
		old_list = kh_value(h, k);
		next_state = old_list;

		while(next_state != NULL) {
			#if DEBUG == 1
			printf("w: %llu\tb: %llu\n", old_list->b_white, old_list->b_black);
			#endif

			if( old_list->b_white == bitboard_white && old_list->b_black == bitboard_black) {
				// same state
				#if DEBUG == 1
				printf("found same state\n");
				#endif

				return 0;
			} else {
				next_state = old_list->next;
				if(next_state != NULL) {
					old_list = next_state;
				}
			}
		}
		// state was not found, add it to list
		// create linked list node to do so

		#if DEBUG == 1
		printf("did not find same state, adding new state to list\n");
		#endif

		list_t *new_state;
		new_state = malloc(sizeof(list_t));
		new_state->b_white = bitboard_white;
		new_state->b_black = bitboard_black;
		new_state->next = NULL;
		old_list->next = new_state;
		return 1;
	}

}

/* performs recursive minimax algo
 * returns as soon as a winning move is found, ie:
 *		the algo won't continue if a guaranteed ai move is found
 *
 * @return: 1 if player who starts (turn) wins
 *			0 if its a draw/no winning state is found before cutoff is reached
 *			-1 if player who starts (turn) loses
 */
int minimax(turn_t turn,
			uint64_t bitboard_white,
			uint64_t bitboard_black,
			uint64_t white_bits[],
			uint64_t black_bits[],
			int curr_ply) {
	int piece_idx, dir;
	
	// first check if this move resulted in an end-game
	if(turn == white) {
		// if current turn is white, previous move was black
		if(check_endgame(bitboard_black))
			return -1;
	}

	// white is MAX, black is MIN
	for(piece_idx = 0; piece_idx < 7; piece_idx++) {
		// try possible move for each piece
		for(dir = 0; dir < 4; dir++) {
			// try the 4 possible directions


		}
	}

	return 0;
}

/* attempts to move the given piece north one spot
 * returns 0 if successful, -1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int north(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
	uint64_t piece_tmp = *piece;
	// first check if its trying to move off the board
	if (piece_tmp & BORDER_N)
		return 1;
	// move up
	piece_tmp = piece_tmp >> 8;
	// now check if it conflicts with its own or the oponent's pieces
	if (piece_tmp & *bb_own || piece_tmp & bb_oponent)
		return 1;
	// move is successful, alter the piece and its board
	*bb_own = *bb_own - *piece;
	*piece = piece_tmp;
	return 0;
}

/* attempts to move the given piece south one spot
 * returns 0 if successful, -1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int south(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
	uint64_t piece_tmp = *piece;
	// first check if its trying to move off the board
	if (piece_tmp & BORDER_S)
		return 1;
	// move down
	piece_tmp = piece_tmp << 8;
	// now check if it conflicts with its own or the oponent's pieces
	if (piece_tmp & *bb_own || piece_tmp & bb_oponent)
		return 1;
	// move is successful, alter the piece and its board
	*bb_own = *bb_own - *piece;
	*piece = piece_tmp;
	return 0;
}

/* attempts to move the given piece west one spot
 * returns 0 if successful, -1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int west(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
	uint64_t piece_tmp = *piece;
	// first check if its trying to move off the board
	if (piece_tmp & BORDER_E)
		return 1;
	// move left
	piece_tmp = piece_tmp >> 1;
	// now check if it conflicts with its own or the oponent's pieces
	if (piece_tmp & *bb_own || piece_tmp & bb_oponent)
		return 1;
	// move is successful, alter the piece and its board
	*bb_own = *bb_own - *piece;
	*piece = piece_tmp;
	return 0;
}

/* attempts to move the given piece east one spot
 * returns 0 if successful, 1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int east(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
	uint64_t piece_tmp = *piece;
	// first check if its trying to move off the board
	if (piece_tmp & BORDER_W)
		return 1;
	// move right
	piece_tmp = piece_tmp << 1;
	// now check if it conflicts with its own or the oponent's pieces
	if (piece_tmp & *bb_own || piece_tmp & bb_oponent)
		return 1;
	// move is successful, alter the piece and its board
	*bb_own = *bb_own - *piece;
	*piece = piece_tmp;
	return 0;
}