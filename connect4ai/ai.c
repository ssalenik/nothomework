#include "ai.h"

#define DEBUG 2

// borders
#define BORDER_W 18085043209519168
#define BORDER_E 282578800148737
#define BORDER_N 127
#define BORDER_S 35747322042253312

KHASH_MAP_INIT_INT64(64, list_t*);
khash_t(64) *h;

char direction_char[4] = {'N', 'E', 'W', 'S'};

int alpha_max;	// the max possible alpha
int beta_min;	// the min possible beta
int alpha, beta;	// the current vals of alpha, beta

int ply_cutoff; 
uint64_t states_visited = 0;
int collisions = 0;
int deepest_ply = 0;

int piece_to_move;
int dir_to_move;
uint64_t new_piece_position;
uint64_t new_bb;

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

/* Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
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

int play_ai_turn(	turn_t turn,
					ai_t ai,
					uint64_t *bb_1,
					uint64_t bb_2,
					uint64_t bits_1[],
					uint64_t bits_2[],
					int d_cutoff)
{

	/* initializations */
	h = kh_init(64);  // allocate a hash table
	ply_cutoff = d_cutoff;
	has_visited_state(*bb_1, bb_2); // save the initial state
	collisions = 0;
	states_visited = 0;
	deepest_ply = 0;

	int util;

	// get initial time
    gettimeofday(&tvBegin, NULL);

    
    switch(ai) {
    case minimax_ai:
    	/* minimax */
    	util = minimax(1, *bb_1, bb_2, bits_1, bits_2, 0);
    	break;
    case alphabeta_ai:
    	/* alphabeta */
		// set the max and min possible values of alpha and beta
		alpha_max = 1;
		beta_min = -1;
		// set the initial values of alpha and beta
		alpha = beta_min;
		beta = alpha_max;
		// perform alpha-beta algo
		util = alphabeta(1, *bb_1, bb_2, bits_1, bits_2, 0);
		break;
	case ab_iter_ai:
		/* alpha-beta with iterative deepening */

		break;
	case eval1_ai:

		break;
	case eval2_ai:

		break;

    }

    // show the move
    int pos[2];
    position_lookup(bits_1[piece_to_move], pos);

    printf("\nmove: %i%i%c\n", pos[0], pos[1], direction_char[dir_to_move]);

    // update board and bits
    *bb_1 = new_bb;
    bits_1[piece_to_move] = new_piece_position;

    if(turn == white) {
    	// AI moved white
    	showstate(bits_1, bits_2);
    } else {
    	// AI moved black
    	showstate(bits_2, bits_1);
    }

	//time elapsed
	gettimeofday(&tvEnd, NULL);
	timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
	printf("time elapsed: %ld.%06ld\n", (long int)tvDiff.tv_sec, (long int)tvDiff.tv_usec);

	#if DEBUG == 2
	printf("util: %i\nstates: %llu\ncollisions: %i\n", util, states_visited, collisions);
	#endif

	// free memory
	delete_hashtable();

	return util;
}

int delete_hashtable() {
	khiter_t k;

	// iterates through table and frees memory
	for (k = kh_begin(h); k != kh_end(h); ++k) {
		if (kh_exist(h, k)){
			// free list
			list_t *list = kh_value(h, k);
			list_t *next;
			while(list != NULL) {
				next = list->next;
				free(list);
				list = next;
			}
		}
	}

	kh_destroy(64, h);

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
						utility_tmp[dir] = minimax(2, bb_dir[dir], bb_2, bits_tmp, bits_2, curr_ply);
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
						utility_tmp[dir] = minimax(1, bb_1, bb_dir[dir], bits_1, bits_tmp, curr_ply);
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
	
	if (curr_ply == ply_cutoff || check_time()) {
		// depth cutoff or time cutoff, and no winner
		return 0;
	}
	curr_ply++;

	

	int utility; // the result
	int utility_piece[7]; // utility of each piece
	int utility_dir[7]; // direction of the utility picked
	int piece_idx;
	int dir_idx;
	int dir_order[4] = {E, W, N, S};
	int dir;
	int piece_order[7] = {0, 1, 2, 3, 4, 5, 6};
	int piece;
	int curr_alpha = beta_min;
	int curr_beta = alpha_max;
	if(turn == 1){
		// player 1 moves
		// player 1 is MAX, player 1 wants the max possible utility
		utility = -2;

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];			
			int utility_tmp[4];
			piece = piece_order[piece_idx];
			utility_piece[piece] = -2; //initial

			//printf("util: %i\tutil_piece: %i\n", utility, utility_piece[piece]);

			
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
						curr_alpha = utility_tmp[dir] = alphabeta(2, bb_dir[dir], bb_2, bits_tmp, bits_2, curr_ply);
						// check alpha value
						if(curr_alpha > alpha) {
							// set as current alpha
							alpha = curr_alpha;
						}

					}

					if(utility_piece[piece] < utility_tmp[dir]) {
						utility_piece[piece] = utility_tmp[dir];
						utility_dir[piece] = dir;
					}
					
				}
				// break if we have reached max possible alpha
				if(curr_alpha == alpha_max)
					break;
			}
			if(utility < utility_piece[piece]) {

				utility = utility_piece[piece];
				piece_to_move = piece;
				dir_to_move = utility_dir[piece];
				new_piece_position = bits_dir[dir_to_move];
				new_bb = bb_dir[dir_to_move];
			}
			// break if we have reached max possible alpha
			if(curr_alpha == alpha_max)
				break;
		}
		
		// if(utility  == 1)
		// 	printf("uitl= 1 - %i, %i, %i, %i, %i, %i, %i\n", utility_piece[0], utility_piece[1], utility_piece[2], utility_piece[3], utility_piece[4], utility_piece[5], utility_piece[6]);
		return utility;
		
	} else {
		// player 2 moves
		// player 2 is MIN, player 2 wants the min possible utility
		utility = 2;

		// try the 7 possible pieces
		int states_tried = 0;
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];			
			int utility_tmp[4] = {2, 2, 2, 2};
			piece = piece_order[piece_idx];

			utility_piece[piece] = 2; //initial

			
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
						curr_beta = utility_tmp[dir] = alphabeta(1, bb_1, bb_dir[dir], bits_1, bits_tmp, curr_ply);
						// check beta value
						if(curr_beta < beta) {
							// set as current beta
							beta = curr_beta;
						}
					}

					if(utility_piece[piece] > utility_tmp[dir]) {
						utility_piece[piece] = utility_tmp[dir];
						//utility_dir[piece] = dir;
					}
					
				}
				// break if we have reached min possible beta
				// or if beta is lower than alpha
				if(curr_beta == beta_min || curr_beta < alpha)
					break;
			}
			if(utility > utility_piece[piece]) {
				utility = utility_piece[piece];
				//piece_to_move = piece;
				//dir_to_move = utility_dir[piece];
			}
			// break if we have reached min possible beta
			// or if beta is lower than alpha
			if(curr_beta == beta_min || curr_beta < alpha)
				break;
		}
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
	return 1;	// not possible if incompatible dir is specified
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

/* returns the x,y position */
int position_lookup(uint64_t piece, int pos[]) {
	switch(piece){
	/* row 1 */
	case 1:
		pos[0] = 1; pos[1] = 1; break;
	case 2:
		pos[0] = 2; pos[1] = 1; break;
	case 4:
		pos[0] = 3; pos[1] = 1; break;
	case 8:
		pos[0] = 4; pos[1] = 1; break;
	case 16:
		pos[0] = 5; pos[1] = 1; break;
	case 32:
		pos[0] = 6; pos[1] = 1; break;
	case 64:
		pos[0] = 7; pos[1] = 1; break;
	
	/* row 2 */
	case 256:
		pos[0] = 1; pos[1] = 2; break;
	case 512:
		pos[0] = 2; pos[1] = 2; break;
	case 1024:
		pos[0] = 3; pos[1] = 2; break;
	case 2048:
		pos[0] = 4; pos[1] = 2; break;
	case 4096:
		pos[0] = 5; pos[1] = 2; break;
	case 8182:
		pos[0] = 6; pos[1] = 2; break;
	case 1638:
		pos[0] = 7; pos[1] = 2; break;

	/* row 3 */
	case 65536:
		pos[0] = 1; pos[1] = 3; break;
	case 131072:
		pos[0] = 2; pos[1] = 3; break;
	case 262144:
		pos[0] = 3; pos[1] = 3; break;
	case 524288:
		pos[0] = 4; pos[1] = 3; break;
	case 1048576:
		pos[0] = 5; pos[1] = 3; break;
	case 2097152:
		pos[0] = 6; pos[1] = 3; break;
	case 4194304:
		pos[0] = 7; pos[1] = 3; break;

	/* row 4 */
	case 16777216:
		pos[0] = 1; pos[1] = 4; break;
	case 33554432:
		pos[0] = 2; pos[1] = 4; break;
	case 67108864:
		pos[0] = 3; pos[1] = 4; break;
	case 134217728:
		pos[0] = 4; pos[1] = 4; break;
	case 268435456:
		pos[0] = 5; pos[1] = 4; break;
	case 536870912:
		pos[0] = 6; pos[1] = 4; break;
	case 1073741824:
		pos[0] = 7; pos[1] = 4; break;

	/* row 5 */
	case 4294967296:
		pos[0] = 1; pos[1] = 5; break;
	case 8589934592:
		pos[0] = 2; pos[1] = 5; break;
	case 17179869184:
		pos[0] = 3; pos[1] = 5; break;
	case 34359738368:
		pos[0] = 4; pos[1] = 5; break;
	case 68719476736:
		pos[0] = 5; pos[1] = 5; break;
	case 137438953472:
		pos[0] = 6; pos[1] = 5; break;
	case 274877906944:
		pos[0] = 7; pos[1] = 5; break;

	/* row 6 */
	case 1099511627776:
		pos[0] = 1; pos[1] = 6; break;
	case 2199023255552:
		pos[0] = 2; pos[1] = 6; break;
	case 4398046511104:
		pos[0] = 3; pos[1] = 6; break;
	case 8796093022208:
		pos[0] = 4; pos[1] = 6; break;
	case 17592186044416:
		pos[0] = 5; pos[1] = 6; break;
	case 35184372088832:
		pos[0] = 6; pos[1] = 6; break;
	case 70368744177664:
		pos[0] = 7; pos[1] = 6; break;

	/* row 7 */
	case 281474976710656:
		pos[0] = 1; pos[1] = 7; break;
	case 562949953421312:
		pos[0] = 2; pos[1] = 7; break;
	case 1125899906842624:
		pos[0] = 3; pos[1] = 7; break;
	case 2251799813685248:
		pos[0] = 4; pos[1] = 7; break;
	case 4503599627370496:
		pos[0] = 5; pos[1] = 7; break;
	case 9007199254740992:
		pos[0] = 6; pos[1] = 7; break;
	case 18014398509481984:
		pos[0] = 7; pos[1] = 7; break;

	default:
		// not a valid position
		pos[0] = -1; pos[1] = -1;
		return 1; break;
	
	}
	return 0;
}