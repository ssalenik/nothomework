#include "ai.h"

#define DEBUG 0

// borders
#define BORDER_W 18085043209519168
#define BORDER_E 282578800148737
#define BORDER_N 127
#define BORDER_S 35747322042253312

KHASH_MAP_INIT_INT64(64, list_t*);
khash_t(64) *h;

int alpha_min;
int beta_max;

int ply_cutoff; 
uint64_t states_visited = 0;
int collisions = 0;

int piece_to_move;
int dir_to_move;

struct timeval tvBegin, tvEnd, tvDiff;

/* 1 if running out of time, 0 otherwise */
int check_time()
{
	struct timeval tvEnd;
	// curr time
	gettimeofday(&tvEnd, NULL);
    int diff = (tvEnd.tv_sec) - (tvBegin.tv_sec);
    // tvDiff.tv_sec = diff / 1000000;
    // tvDiff.tv_usec = diff % 1000000;

    return (diff>=19);
}

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
	ply_cutoff = d_cutoff;
	has_visited_state(bb_1, bb_2); // save the initial state

	// get initial time
    gettimeofday(&tvBegin, NULL);

    int util;
    int alpha;
	util = minimax(1, bb_1, bb_2, bits_1, bits_2, 0);

	//alphabeta
	alpha_min = -1;
	beta_max = 1;
	// alpha = alphabeta(1, bb_1, bb_2, bits_1, bits_2, 0);

	// util = alpha;

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

	check_time();

	int utility; // the result
	int utility_piece[7] = {0, 0, 0, 0, 0, 0, 0}; // utility of each piece
	int utility_dir[7]; // direction of the utility picked
	int piece_idx;
	int dir_idx;
	int dir_order[4] = {E, W, N, S};
	int dir;
	int piece_order[7] = {0, 1, 2, 3, 4, 5, 6};
	int piece;
	if(turn == 1){
		// player 1 moves
		// player 1 is MAX, player 1 wants the max possible utility

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];			
			int utility_tmp[4] = {-2, -2, -2, -2};

			piece = piece_order[piece_idx];
			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir[dir] = bits_1[piece];
				bb_dir[dir] = bb_1;

				if (trydir(dir, &(bits_dir[dir]), &(bb_dir[dir]), bb_2) == 0) {
					// possible to move this direction
					utility_tmp[dir] = 0; // set to 0, as it is a possible state
					// check if state has been visited
					if(has_visited_state(bb_dir[dir], bb_2) == 1) {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
						bits_tmp[piece] = bits_dir[dir];
						// perform minimax on this move
						utility_tmp[dir] = alphabeta(2, bb_dir[dir], bb_2, bits_tmp, bits_2, curr_ply);
					}
				}
			}

			// now check in  order which one to take
			utility_piece[piece] = utility_tmp[dir_order[0]];
			utility_dir[piece] = dir_order[0];
			for(dir_idx = 1; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				if(utility_piece[piece] < utility_tmp[dir]) {
					utility_piece[piece] = utility_tmp[dir];
					utility_dir[piece] = dir;
				}
			}
		}

		// now check in order which one to take
		utility = utility_piece[piece_order[0]];
		piece = piece_order[0];
		for(piece_idx = 1; piece_idx < 7; piece_idx++) {
			piece = piece_order[piece_idx];
			if(utility < utility_piece[piece]) {
				utility = utility_piece[piece];
				piece_to_move = piece;
			}
		}

		dir_to_move = utility_dir[piece];
		// if(utility  == 1)
		// 	printf("uitl= 1 - %i, %i, %i, %i, %i, %i, %i\n", utility_piece[0], utility_piece[1], utility_piece[2], utility_piece[3], utility_piece[4], utility_piece[5], utility_piece[6]);
		return utility;
		
	} else {
		// player 2 moves
		// player 2 is MIN, player 2 wants the min possible utility

		// try the 7 possible pieces
		int states_tried = 0;
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];			
			int utility_tmp[4] = {2, 2, 2, 2};

			piece = piece_order[piece_idx];
			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir[dir] = bits_2[piece_idx];
				bb_dir[dir] = bb_2;

				if (trydir(dir, &(bits_dir[dir]), &(bb_dir[dir]), bb_1) == 0) {
					// possible to move this direction
					utility_tmp[dir] = 0; // set to 0, as it is a possible state
					// check if state has been visited
					if(has_visited_state(bb_1, bb_dir[dir]) == 1) {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
						bits_tmp[piece_idx] = bits_dir[dir];
						// perform minimax on this move
						utility_tmp[dir] = alphabeta(1, bb_1, bb_dir[dir], bits_1, bits_tmp, curr_ply);
					}
				}
			}

			// now check in  order which one to take
			utility_piece[piece_idx] = utility_tmp[dir_order[0]];
			utility_dir[piece_idx] = dir_order[0];
			for(dir_idx = 1; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				if(utility_piece[piece_idx] > utility_tmp[dir]) {
					utility_piece[piece_idx] = utility_tmp[dir];
					utility_dir[piece_idx] = dir;
				}
			}
		}

		// now check in order which one to take
		utility = utility_piece[piece_order[0]];
		piece = piece_order[0];
		for(piece_idx = 1; piece_idx < 7; piece_idx++) {
			piece = piece_order[piece_idx];
			if(utility > utility_piece[piece]) {
				utility = utility_piece[piece];
				piece_to_move = piece;
			}
		}
		
		dir_to_move = utility_dir[piece];
		// if(utility > 1 || utility < -1) {
		// 	printf("uitl > 1, wut? - %i, %i, %i, %i, %i, %i, %i\n", utility_piece[0], utility_piece[1], utility_piece[2], utility_piece[3], utility_piece[4], utility_piece[5], utility_piece[6]);
		// 	printf("states tried: %i\n", states_tried);
		// 	showstate(bits_1, bits_2);
		// }
		return utility;
	}
}

/* performs recursive alphabeta algo
 * assumes player 1 is max player
 *
 * @return: 1 if player who starts (turn) wins
 *			0 if its a draw/no winning state is found before cutoff is reached
 *			-1 if player who starts (turn) loses
 */
int alphabeta(	int turn,
				//int alpha,
				//int beta,
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

	check_time();

	int utility; // the result
	int utility_piece[7] = {0, 0, 0, 0, 0, 0, 0}; // utility of each piece
	int utility_dir[7]; // direction of the utility picked
	int piece_idx;
	int dir_idx;
	int dir_order[4] = {E, W, N, S};
	int dir;
	int piece_order[7] = {0, 1, 2, 3, 4, 5, 6};
	int piece;
	if(turn == 1){
		// player 1 moves
		// player 1 is MAX, player 1 wants the max possible utility

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];			
			int utility_tmp[4] = {-2, -2, -2, -2};

			piece = piece_order[piece_idx];
			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir[dir] = bits_1[piece];
				bb_dir[dir] = bb_1;

				if (trydir(dir, &(bits_dir[dir]), &(bb_dir[dir]), bb_2) == 0) {
					// possible to move this direction
					utility_tmp[dir] = 0; // set to 0, as it is a possible state
					// check if state has been visited
					if(has_visited_state(bb_dir[dir], bb_2) == 1) {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
						bits_tmp[piece] = bits_dir[dir];
						// perform minimax on this move
						utility_tmp[dir] = alphabeta(2, bb_dir[dir], bb_2, bits_tmp, bits_2, curr_ply);
					}
				}
			}

			// now check in  order which one to take
			utility_piece[piece] = utility_tmp[dir_order[0]];
			utility_dir[piece] = dir_order[0];
			for(dir_idx = 1; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				if(utility_piece[piece] < utility_tmp[dir]) {
					utility_piece[piece] = utility_tmp[dir];
					utility_dir[piece] = dir;
				}
			}
		}

		// now check in order which one to take
		utility = utility_piece[piece_order[0]];
		piece = piece_order[0];
		for(piece_idx = 1; piece_idx < 7; piece_idx++) {
			piece = piece_order[piece_idx];
			if(utility < utility_piece[piece]) {
				utility = utility_piece[piece];
				piece_to_move = piece;
			}
		}

		dir_to_move = utility_dir[piece];
		// if(utility  == 1)
		// 	printf("uitl= 1 - %i, %i, %i, %i, %i, %i, %i\n", utility_piece[0], utility_piece[1], utility_piece[2], utility_piece[3], utility_piece[4], utility_piece[5], utility_piece[6]);
		return utility;
		
	} else {
		// player 2 moves
		// player 2 is MIN, player 2 wants the min possible utility

		// try the 7 possible pieces
		int states_tried = 0;
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];			
			int utility_tmp[4] = {2, 2, 2, 2};

			piece = piece_order[piece_idx];
			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir[dir] = bits_2[piece_idx];
				bb_dir[dir] = bb_2;

				if (trydir(dir, &(bits_dir[dir]), &(bb_dir[dir]), bb_1) == 0) {
					// possible to move this direction
					utility_tmp[dir] = 0; // set to 0, as it is a possible state
					// check if state has been visited
					if(has_visited_state(bb_1, bb_dir[dir]) == 1) {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
						bits_tmp[piece_idx] = bits_dir[dir];
						// perform minimax on this move
						utility_tmp[dir] = alphabeta(1, bb_1, bb_dir[dir], bits_1, bits_tmp, curr_ply);
					}
				}
			}

			// now check in  order which one to take
			utility_piece[piece_idx] = utility_tmp[dir_order[0]];
			utility_dir[piece_idx] = dir_order[0];
			for(dir_idx = 1; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				if(utility_piece[piece_idx] > utility_tmp[dir]) {
					utility_piece[piece_idx] = utility_tmp[dir];
					utility_dir[piece_idx] = dir;
				}
			}
		}

		// now check in order which one to take
		utility = utility_piece[piece_order[0]];
		piece = piece_order[0];
		for(piece_idx = 1; piece_idx < 7; piece_idx++) {
			piece = piece_order[piece_idx];
			if(utility > utility_piece[piece]) {
				utility = utility_piece[piece];
				piece_to_move = piece;
			}
		}
		
		dir_to_move = utility_dir[piece];
		// if(utility > 1 || utility < -1) {
		// 	printf("uitl > 1, wut? - %i, %i, %i, %i, %i, %i, %i\n", utility_piece[0], utility_piece[1], utility_piece[2], utility_piece[3], utility_piece[4], utility_piece[5], utility_piece[6]);
		// 	printf("states tried: %i\n", states_tried);
		// 	showstate(bits_1, bits_2);
		// }
		return utility;
	}
}

int trydir(int dir, uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent) {
	switch(dir) {
	case N:
		return trynorth(piece, bb_own, bb_oponent);
		break;
	case S:
		return trysouth(piece, bb_own, bb_oponent);
		break;
	case E:
		return tryeast(piece, bb_own, bb_oponent);
		break;
	case W:
		return trywest(piece, bb_own, bb_oponent);
		break;
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