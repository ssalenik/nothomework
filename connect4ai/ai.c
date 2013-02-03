#include "ai.h"

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

KHASH_MAP_INIT_INT64(64, list_t*);
khash_t(64) *h;

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
		printf("new key\n");
		list_t *new_list;
		new_list = malloc(sizeof(list_t));
		new_list->b_white = bitboard_white;
		new_list->b_black = bitboard_black;
		new_list->next = NULL;
		kh_value(h, k) = new_list;

		printf("stored w: %llu\nstored b: %llu\n", new_list->b_white, new_list->b_black);
		return 1;
	} else {
		// key already exists, check the state
		printf("old key\n");
		list_t *old_list;
		list_t *next_state;
		old_list = kh_value(h, k);
		next_state = old_list;

		while(next_state != NULL) {
			printf("w: %llu\nb: %llu\n", old_list->b_white, old_list->b_black);
			if( old_list->b_white == bitboard_white && old_list->b_black == bitboard_black) {
				// same state
				printf("same state\n");
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
		list_t *new_state;
		new_state = malloc(sizeof(list_t));
		new_state->b_white = bitboard_white;
		new_state->b_black = bitboard_black;
		new_state->next = NULL;
		old_list->next = new_state;
		return 1;
	}


	return 0;
}

int minimax(turn_t turn,
			uint64_t bitboard_white,
			uint64_t bitboard_black,
			uint64_t white_bits[],
			uint64_t black_bits[],
			int curr_depth) {

	// test
	int result;

	printf("storing:\nw: %llu\nw: %llu\n", bitboard_white, bitboard_black);
	result = has_visited_state(bitboard_white, bitboard_black);
	printf("result: %i\n", result);
	printf("storing:\nw: %llu\nw: %llu\n", bitboard_white, bitboard_black);
	result = has_visited_state(bitboard_white, bitboard_black);
	printf("result: %i\n", result);
	printf("storing:\nw: %llu\nw: %llu\n", bitboard_white, bitboard_black + 1);
	result = has_visited_state(bitboard_white, bitboard_black + 1);
	printf("result: %i\n", result);
	
	if(turn == white) {

	} else {

	}



	return 0;
}