#include "ai.h"

#define DEBUG 0

// borders
#define BORDER_W 18085043209519168
#define BORDER_E 282578800148737
#define BORDER_N 127
#define BORDER_S 35747322042253312 

KHASH_MAP_INIT_INT64(64, list_t*);
khash_t(64) *h;

int ply_cutoff; // 2 * depth_cutoff
uint64_t states_visited = 0;
int collisions = 0;

int piece_to_move;
int dir_to_move;

/* checks if end-game has been reached
 * returns:
 * 0 - if no win
 * 1 - if win (horizontal)
 * 2 - if win (vertical)
 * 3 - if win (diagonal \ )
 * 4 - if win (diagonal / )
 *
 * algo adapted from:
 * http://stackoverflow.com/questions/7033165/algorithm-to-check-a-connect-four-field
 * which an algo by John Tromp from http://homepages.cwi.nl/~tromp/c4/fhour.html	
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
	return w ^ (b << 8);
}

int ai_turn(uint64_t bb_1,
			uint64_t bb_2,
			uint64_t bits_1[],
			uint64_t bits_2[],
			int d_cutoff,
			int* states) {
	h = kh_init(64);  // allocate a hash table
	ply_cutoff = 2*d_cutoff;
	has_visited_state(bb_1, bb_2); // save the initial state

	int util = minimax(1, bb_1, bb_2, bits_1, bits_2, 0);

	printf("util: %i\nstates: %llu\ncollisions: %i\npiece to move: %i\ndir to move: %i\n", util, states_visited, collisions, piece_to_move, dir_to_move);

	*states = states_visited;
	return util;
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

		states_visited++;	//incremente states visited count
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

		collisions++;	// key was the same, but it was a different state
		states_visited++;	//incremente states visited count
		return 1;
	}

}

/* performs recursive minimax algo
 * assumes player 1 is max player
 *
 * @return: 1 if player who starts (turn) wins
 *			0 if its a draw/no winning state is found before cutoff is reached
 *			-1 if player who starts (turn) loses
 */
int minimax(int turn,
			uint64_t bb_1,
			uint64_t bb_2,
			uint64_t bits_1[],
			uint64_t bits_2[],
			int curr_ply) {
	if(turn == 1) {
		// previous turn was of player 2
		// check if it was a winning move
		if(check_endgame(bb_2)) {
			// player 2 is min
			//printf("lost\n");
			return -1;
		}
	} else {
		// previous turn was of player 1
		int end = check_endgame(bb_1);
		if(end) {
			//player 1 is max
			//printf("won: %i\n", end);
			//printf("bb1: %llu\n", bb_1);
			//showstate(bits_1, bits_2);
			return 1;
		}
	}
	// check if we have reached the depth cutoff
	
	if (curr_ply == ply_cutoff) {
		// depth cutoff, and no winner
		return 0;
	}
	curr_ply++;

	int utility; // the result
	int utility_piece[7] = {0, 0, 0, 0, 0, 0, 0}; // utility of each piece
	int utility_dir[7]; // direction of the utility picked
	int piece_idx;
	if(turn == 1){
		// player 1 moves
		// player 1 is MAX, player 1 wants the max possible utility

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			// try the 4 possible directions
			// north
			uint64_t north = bits_1[piece_idx];
			uint64_t bb_north = bb_1;
			int utility_tmp[4] = {-2, -2, -2, -2};
			if (trynorth(&north, &bb_north, bb_2) == 0) {
				// possible to move north
				utility_tmp[N] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_north, bb_2) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = north;
					// perform minimax on this move
					utility_tmp[N] = minimax(2, bb_north, bb_2, bits_tmp, bits_2, curr_ply);
				}
			}
			// east
			uint64_t east = bits_1[piece_idx];
			uint64_t bb_east = bb_1;
			if (tryeast(&east, &bb_east, bb_2) == 0) {
				// possible to move east
				utility_tmp[E] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_east, bb_2) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = east;
					// perform minimax on this move
					utility_tmp[E] = minimax(2, bb_east, bb_2, bits_tmp, bits_2, curr_ply);
				}
			}
			// west
			uint64_t west = bits_1[piece_idx];
			uint64_t bb_west = bb_1;
			if (trywest(&west, &bb_west, bb_2) == 0) {
				// possible to move west
				utility_tmp[W] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_west, bb_2) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = west;
					// perform minimax on this move
					utility_tmp[W] = minimax(2, bb_west, bb_2, bits_tmp, bits_2, curr_ply);
				}
			}
			// south
			uint64_t south = bits_1[piece_idx];
			uint64_t bb_south = bb_1;
			if (trysouth(&south, &bb_south, bb_2) == 0) {
				// possible to move south
				utility_tmp[S] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_south, bb_2) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = south;
					// perform minimax on this move
					utility_tmp[S] = minimax(2, bb_south, bb_2, bits_tmp, bits_2, curr_ply);
				}
			}

			// 0 - north
			// 1 - east
			// 2 - west
			// 3 - south
			// now check in preferred order which one to take
			int dir_order[4] = {E, W, N, S};
			int dir_idx;
			utility_piece[piece_idx] = utility_tmp[dir_order[0]];
			utility_dir[piece_idx] = dir_order[0];
			for(dir_idx = 1; dir_idx < 4; dir_idx++) {
				if(utility_piece[piece_idx] < utility_tmp[dir_order[dir_idx]]) {
					utility_piece[piece_idx] = utility_tmp[dir_order[dir_idx]];
					utility_dir[piece_idx] = dir_order[dir_idx];
				}
			}
		}

		// 0 - 6 piece
		// now check in preferred order which one to take
		int piece_order[7] = {0, 1, 2, 3, 4, 5, 6};
		int order_idx;
		utility = utility_piece[piece_order[0]];
		piece_idx = piece_order[0];
		for(order_idx = 1; order_idx < 7; order_idx++) {
			if(utility < utility_piece[piece_order[order_idx]]) {
				utility = utility_piece[piece_order[order_idx]];
				piece_idx = piece_order[order_idx];
			}
		}

		piece_to_move = piece_idx;
		dir_to_move = utility_dir[piece_to_move];
		// if(utility  == 1)
		// 	printf("uitl= 1 - %i, %i, %i, %i, %i, %i, %i\n", utility_piece[0], utility_piece[1], utility_piece[2], utility_piece[3], utility_piece[4], utility_piece[5], utility_piece[6]);
		return utility;
		
	} else {
		// player 2 moves
		// player 2 is MIN, player 2 wants the min possible utility

		// try the 7 possible pieces
		int states_tried = 0;
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			// try the 4 possible directions
			// north
			uint64_t north = bits_2[piece_idx];
			uint64_t bb_north = bb_2;
			int utility_tmp[4] = {2, 2, 2, 2};
			if (trynorth(&north, &bb_north, bb_1) == 0) {
				// possible to move north
				utility_tmp[N] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_1, bb_north) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = north;
					// perform minimax on this move
					utility_tmp[N] = minimax(1, bb_1, bb_north, bits_1, bits_tmp, curr_ply);
					states_tried++;
				}
			}
			// east
			uint64_t east = bits_2[piece_idx];
			uint64_t bb_east = bb_2;
			if (tryeast(&east, &bb_east, bb_1) == 0) {
				// possible to move east
				utility_tmp[E] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_1, bb_east) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = east;
					// perform minimax on this move
					utility_tmp[E] = minimax(1, bb_1, bb_east, bits_1, bits_tmp, curr_ply);
					states_tried++;
				}
			}
			// west
			uint64_t west = bits_2[piece_idx];
			uint64_t bb_west = bb_2;
			if (trywest(&west, &bb_west, bb_1) == 0) {
				// possible to move west
				utility_tmp[W] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_1, bb_west) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = west;
					// perform minimax on this move
					utility_tmp[W] = minimax(1, bb_1, bb_west, bits_1, bits_tmp, curr_ply);
					states_tried++;
				}
			}
			// south
			uint64_t south = bits_2[piece_idx];
			uint64_t bb_south = bb_2;
			if (trysouth(&south, &bb_south, bb_1) == 0) {
				// possible to move south
				utility_tmp[S] = 0; // set to 0, as it is a possible state
				// check if state has been visited
				if(has_visited_state(bb_1, bb_south) == 1) {
					// new state!
					// need a copy of the modified board for each try
					uint64_t bits_tmp[7];
					memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
					bits_tmp[piece_idx] = south;
					// perform minimax on this move
					utility_tmp[S] = minimax(1, bb_1, bb_south, bits_1, bits_tmp, curr_ply);
					states_tried++;
				}
			}

			// 0 - north
			// 1 - east
			// 2 - west
			// 3 - south
			// now check in preferred order which one to take
			int dir_order[4] = {E, W, N, S};
			int dir_idx;
			utility_piece[piece_idx] = utility_tmp[dir_order[0]];
			utility_dir[piece_idx] = dir_order[0];
			for(dir_idx = 1; dir_idx < 4; dir_idx++) {
				if(utility_piece[piece_idx] > utility_tmp[dir_order[dir_idx]]) {
					utility_piece[piece_idx] = utility_tmp[dir_order[dir_idx]];
					utility_dir[piece_idx] = dir_order[dir_idx];
				}
			}
		}

		// 0 - 6 piece
		// now check in preferred order which one to take
		int piece_order[7] = {0, 1, 2, 3, 4, 5, 6};
		int order_idx;
		utility = utility_piece[piece_order[0]];
		piece_idx = piece_order[0];
		for(order_idx = 1; order_idx < 7; order_idx++) {
			if(utility > utility_piece[piece_order[order_idx]]) {
				utility = utility_piece[piece_order[order_idx]];
				piece_idx = piece_order[order_idx];
			}
		}

		piece_to_move = piece_idx;
		dir_to_move = utility_dir[piece_to_move];
		// if(utility > 1 || utility < -1) {
		// 	printf("uitl > 1, wut? - %i, %i, %i, %i, %i, %i, %i\n", utility_piece[0], utility_piece[1], utility_piece[2], utility_piece[3], utility_piece[4], utility_piece[5], utility_piece[6]);
		// 	printf("states tried: %i\n", states_tried);
		// 	showstate(bits_1, bits_2);
		// }
		return utility;
	}
}

/* attempts to move the given piece north one spot
 * returns 0 if successful, 1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int trynorth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
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
	*bb_own = *bb_own | piece_tmp;
	*piece = piece_tmp;
	return 0;
}

/* attempts to move the given piece south one spot
 * returns 0 if successful, 1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int trysouth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
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
	*bb_own = *bb_own | piece_tmp;
	*piece = piece_tmp;
	return 0;
}

/* attempts to move the given piece west one spot
 * returns 0 if successful, 1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int trywest(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
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
	*bb_own = *bb_own | piece_tmp;
	*piece = piece_tmp;
	return 0;
}

/* attempts to move the given piece east one spot
 * returns 0 if successful, 1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int tryeast(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
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
	*bb_own = *bb_own | piece_tmp;
	*piece = piece_tmp;
	return 0;
}