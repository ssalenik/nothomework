/**
	* Stepan Salenikovich
	* ECSE 526
	* Assignment 1
	*
	* ai.c
	*/
#include "ai.h"

#define DEBUG 0 // enable/disable certain debug prting messages

// borders
#define BORDER_E 18085043209519168
#define BORDER_W 282578800148737
#define BORDER_N 127
#define BORDER_S 35747322042253312

/* hash table - which is not currently used */
KHASH_MAP_INIT_INT64(64, list_t*);
khash_t(64) *h;

/* transaltion of directions from int to char */
char direction_char[4] = {'N', 'E', 'W', 'S'};

/* order in which to visit each state by piece and direction */
int dir_order[4] = {E, W, N, S};
//int dir_order[4] = {S, N, W, E};
// int dir_order[4] = {W, S, E, N};

int piece_order[7] = {0, 1, 2, 3, 4, 5, 6};
//int piece_order[7] = {6, 5, 4, 3, 2, 1, 0};

int alpha_max; // the max possible alpha
int beta_min; // the min possible beta

ai_t ai_selected; // the AI type specified for use

int ply_cutoff;  // the ply/depth cutoff specified

/* vars used to keep track of algorithm */
uint64_t states_visited = 0;
uint64_t states_visited_curr_iter = 0;
int collisions = 0;
int deepest_ply = 0;
int deepest_ply_curr_iter = 0;

/* the result of the AI algo */
int piece_to_move = 0;
int dir_to_move = E;
uint64_t new_piece_position;
uint64_t new_bb;

/* time keeping */
struct timeval tvBegin, tvEnd, tvDiff;

/* plays 1 AI turn
	* assumes it is the turn of player 1
	* updates the state of the board to reflect the move
	*
	* returns: 1 if AI makes winning move
				0 if play continues
				-1 if input was a winning state for player 2
	*/
int play_ai_turn( turn_t turn,
					ai_t ai,
					uint64_t *bb_1,
					uint64_t bb_2,
					uint64_t bits_1[],
					uint64_t bits_2[],
					int d_cutoff)
{

	/* initializations */
	//h = kh_init(64);  // allocate a hash table
	int* value_p;
	int* set_p;
	//has_visited_state(*bb_1, bb_2, &set_p, &value_p); // save the initial state
	collisions = 0;
	states_visited = states_visited_curr_iter = 0;
	deepest_ply = deepest_ply_curr_iter = 0;

	int util; // ulitlity returned at the end of AI move

	ai_selected = ai;

	// get initial time
	gettimeofday(&tvBegin, NULL);

				
	switch(ai) {
	case minimax_ai:
		/* minimax */
		alpha_max = 1;
		beta_min = -1;
		ply_cutoff = d_cutoff;
		ply_cutoff = d_cutoff;
		util = minimax(1, *bb_1, bb_2, bits_1, bits_2, 0);
		states_visited = states_visited_curr_iter;
		deepest_ply = deepest_ply_curr_iter;
		break;
	case alphabeta_ai:
	case eval1_ai:
		/* alphabeta */
		// possibly using eval function
		switch(ai){
		case eval1_ai:
			alpha_max = INT_MAX;
			beta_min = INT_MIN;
			break;
		default:
			alpha_max = 1;
			beta_min = -1;
			break;
		}
		ply_cutoff = d_cutoff;
		// set the max and min possible values of alpha and beta
		alpha_max = 1;
		beta_min = -1;
		// perform alpha-beta algo
		util = alphabeta(1, beta_min, alpha_max, *bb_1, bb_2, bits_1, bits_2, 0);
		states_visited = states_visited_curr_iter;
					deepest_ply = deepest_ply_curr_iter;
		break;
	case ab_iter_ai:
	case eval1_iter_ai:
	
		/* alpha-beta with iterative deepening */
		// possibly using eval function
		ply_cutoff = 1;
		// set the max and min possible values of alpha and beta
		switch(ai){
		case eval1_iter_ai:
			alpha_max = INT_MAX;
			beta_min = INT_MIN;
			break;
		default:
			alpha_max = 1;
			beta_min = -1;
			break;
		}
		// perform iterative deepening
		while(ply_cutoff <= d_cutoff && util != alpha_max && !check_time()) {
			states_visited_curr_iter = 0;
			deepest_ply_curr_iter = 0;
			// if(ply_cutoff != 1) {
			//  free_hashtable(); //free memory except the first run
			//  has_visited_state(*bb_1, bb_2, &set_p, &value_p); // save the initial state
			// }
			util = alphabeta(1, beta_min, alpha_max, *bb_1, bb_2, bits_1, bits_2, 0);
			if(deepest_ply_curr_iter > deepest_ply)
				deepest_ply = deepest_ply_curr_iter;
			if(states_visited_curr_iter > states_visited)
				states_visited = states_visited_curr_iter;
			ply_cutoff++;
		}
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

	//other info
	printf("util: %i\nstates visited: %llu\ndeepest ply reached: %i\n", util, states_visited, deepest_ply);

	#if DEBUG == 1
	printf("collisions: %i\n", collisions);
	#endif

	// free memory
	//delete_hashtable();

	if(util == alpha_max && deepest_ply == 1) {
		// AI wins
		return 1;
	} else if (util == beta_min && deepest_ply == 0) {
		// other player wins
		return -1;
	} else {
		return 0;
	}

}
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

/* generates key for the hash table (not unique) */
uint64_t generate_key(uint64_t w, uint64_t b) {
	return w ^ (b << 8);
}
/* frees all the value memory and then destroyes the hashtable */
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

/* deletes all the key/values in the hash table; frees value memory */
int free_hashtable() {
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
			// delete key
			kh_del(64, h, k);
			}
		}
	}


	return 0;
}

/* function which checks if the state given has already been visited
	* does this by checking the hash table of states
	* if state is new, stores it in the hash table
	*
	* @return - 1 if new, 0 if visited already
	*/
int has_visited_state(uint64_t bitboard_white, uint64_t bitboard_black, int** set_p, int** value_p) {
	int ret;
	khiter_t k;

	uint64_t key = generate_key(bitboard_white, bitboard_black);

	k = kh_put(64, h, key, &ret);

	int has_visited;

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
		new_list->set = 0;  // not yet found
		new_list->value = 0; // arbitrary
		new_list->next = NULL;
		kh_value(h, k) = new_list;

		*set_p = &(new_list->set);
		*value_p = &(new_list->value);

		states_visited_curr_iter++; //incremente states visited count
		
		has_visited = 1;
	} else {
		// key already exists, check the state
		#if DEBUG == 1
		printf("old key, listing visited states:\n");
		#endif

		list_t *old_list;
		list_t *next_state;
		old_list = kh_value(h, k);
		next_state = old_list;

		has_visited = 1;

		while(next_state != NULL && has_visited) {
			#if DEBUG == 1
			printf("w: %llu\tb: %llu\n", old_list->b_white, old_list->b_black);
			#endif

			if( old_list->b_white == bitboard_white && old_list->b_black == bitboard_black) {
				// same state
				// set will be 1 if the value has already been determined 
				*set_p = &(old_list->set);
				*value_p = &(old_list->value);

				has_visited = 0;

			} else {
				next_state = old_list->next;
				if(next_state != NULL) {
					old_list = next_state;
				}
			}
		}

		if(has_visited) {
			// state was not found, add it to list
			// create linked list node to do so

			#if DEBUG == 1
			printf("did not find same state, adding new state to list\n");
			#endif

			list_t *new_state;
			new_state = malloc(sizeof(list_t));
			new_state->b_white = bitboard_white;
			new_state->b_black = bitboard_black;
			new_state->set = 0;  // not yet found
			new_state->value = 0; // arbitrary
			new_state->next = NULL;
			old_list->next = new_state;

			*set_p = &(new_state->set);
			*value_p = &(new_state->value);

			collisions++; // key was the same, but it was a different state
			states_visited_curr_iter++; //incremente states visited count

			has_visited = 1; // just to make sure
		}
	}

	return has_visited;

}

/* performs recursive minimax algo
	* assumes player 1 is max player
	*
	* @return: 1 if player who starts (turn) wins
	*   0 if its a draw/no winning state is found before cutoff is reached
	*   -1 if player who starts (turn) loses
	*/
int minimax(int turn,
			uint64_t bb_1,
			uint64_t bb_2,
			uint64_t bits_1[],
			uint64_t bits_2[],
			int curr_ply) {

	states_visited_curr_iter++;

	if(turn == 1) {
		// previous turn was of player 2
		// check if it was a winning move
		if(check_endgame(bb_2)) {
			// player 2 is min
			return -1;
		}
	} else {
		// previous turn was of player 1
		if(check_endgame(bb_1)) {
			return 1;
		}
	}
	// check if we have reached the depth cutoff
	
	if (curr_ply == ply_cutoff) { // || check_time()) {
		// depth cutoff, and no winner
		return 0;
	}
	curr_ply++;
	if(curr_ply > deepest_ply_curr_iter)
		deepest_ply_curr_iter = curr_ply;

	int utility; // the result
	int piece_idx;
	int dir_idx;
	int dir;
	int piece;
	if(turn == 1){
		// player 1 moves
		// player 1 is MAX, player 1 wants the max possible utility
		utility = beta_min;

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir;
			uint64_t bb_dir;   
			piece = piece_order[piece_idx];

			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir = bits_1[piece];
				bb_dir = bb_1;

				if (trydir(dir, &bits_dir, &bb_dir, bb_2) == 0) {
					// possible to move this direction
					int utility_tmp;

					// check if state has been visited
					int* set_p;
					int* value_p;
					
					/* note: not currently storing the states */
					if(1) { // has_visited_state(bb_dir[dir], bb_2, &set_p, &value_p); {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
						bits_tmp[piece] = bits_dir;
						// perform minimax on this move
						utility_tmp = minimax(2, bb_dir, bb_2, bits_tmp, bits_2, curr_ply);
						// store the eval of this state and mark it as set
						//*value_p = utility_tmp[dir];
						//*set_p = 1;
					} else {
						// state has alreayd been visited
						// check if the value has been set
						// if not, its a loop, set val to 0

						if(*set_p) {
							// the value has been alreayd found by a previous branch
							utility_tmp = *value_p;
						} else {
							// this means we have returned to the same state in the same branch
							utility_tmp = 0;  // because a loop is effectively a draw
						}
					}
					if(utility_tmp > utility) {
						utility = utility_tmp;
						if(curr_ply == 1 && deepest_ply_curr_iter >= deepest_ply) {
							// make sure we only set the new way to go if this iteration had time to go deeper
							piece_to_move = piece;
							dir_to_move = dir;
							new_piece_position = bits_dir;
							new_bb = bb_dir;
						}
					}
				}
			}
		}
		return utility;
		
	} else {
		// player 2 moves
		// player 2 is MIN, player 2 wants the min possible utility
		utility = alpha_max;

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir;
			uint64_t bb_dir;   
			piece = piece_order[piece_idx];

			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir = bits_2[piece];
				bb_dir = bb_2;

				if (trydir(dir, &bits_dir, &bb_dir, bb_1) == 0) {
					// possible to move this direction
					int utility_tmp;

					// check if state has been visited
					int* set_p;
					int* value_p;

					/* note: not currently storing the states */
					if(1) { //has_visited_state(bb_dir[dir], bb_2, &set_p, &value_p) == 1) {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
						bits_tmp[piece] = bits_dir;
						// perform minimax on this move
						utility_tmp= minimax(1, bb_1, bb_dir, bits_1, bits_tmp, curr_ply);
						// store the eval of this state and mark it as set
						//*value_p = utility_tmp;
						//*set_p = 1;
					} else {
						// state has alreayd been visited
						// check if the value has been set
						// if not, its a loop, set val to 0
						if(*set_p) {
							// the value has been alreayd found by a previous branch
							utility_tmp = *value_p;
						} else {
							// this means we have returned to the same state in the same branch
							utility_tmp = 0;  // because a loop is effectively a draw
						}
					}
					if(utility_tmp < utility) {
						utility = utility_tmp;
					}
				}
			}
		}
		return utility;
	}
}

/* performs recursive alphabeta algo
	* assumes player 1 is max player
	*
	* uses evaluation function if the global var speicies to do so
	*/
int alphabeta( int turn,
				int alpha,
				int beta,
				uint64_t bb_1,
				uint64_t bb_2,
				uint64_t bits_1[],
				uint64_t bits_2[],
				int curr_ply) {

	states_visited_curr_iter++;

	if(turn == 1) {
		// previous turn was of player 2
		// check if it was a winning move
		if(check_endgame(bb_2)) {
			// player 2 is min
			return beta_min;
		}
	} else {
		// previous turn was of player 1
		if(check_endgame(bb_1)) {
			//player 1 is max
			return alpha_max;
		}
	}
	// check if we have reached the depth cutoff
	
	if (curr_ply == ply_cutoff || check_time()) {
		// depth cutoff or time cutoff, and no winner
		// return 0 if we using just alpha-beta
		// perform eval function otherwise
		if(ai_selected == eval1_ai || ai_selected == eval1_iter_ai) {
			return eval1(bits_1, bits_2);
		} else {
			return 0;
		}
	}
	curr_ply++;
	if(curr_ply > deepest_ply_curr_iter)
		deepest_ply_curr_iter = curr_ply;

	int utility; // the result
	int piece_idx;
	int dir_idx;
	int dir;
	int piece;
	if(turn == 1){
		// player 1 moves
		// player 1 is MAX, player 1 wants the max possible utility
		utility = beta_min;

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];   
			
			piece = piece_order[piece_idx];
			
			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir[dir] = bits_1[piece];
				bb_dir[dir] = bb_1;

				if (trydir(dir, &(bits_dir[dir]), &(bb_dir[dir]), bb_2) == 0) {
					// possible to move this direction
					int utility_tmp;
					// check if state has been visited
					int* set_p;
					int* value_p;
					/* note: not currently sotring the states */
					if(1) { //has_visited_state(bb_dir[dir], bb_2, &set_p, &value_p) == 1) {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_1, 7*sizeof(uint64_t));
						bits_tmp[piece] = bits_dir[dir];
						// perform minimax on this move
						utility_tmp = alphabeta(2, alpha, beta, bb_dir[dir], bb_2, bits_tmp, bits_2, curr_ply);
						// store the eval of this state and mark it as set
						// *value_p = utility_tmp;
						// *set_p = 1;
					} else {
						// state has alreayd been visited
						// check if the value has been set
						// if not, its a loop, set val to 0
						if(set_p) {
							utility_tmp = *value_p;
						} else {
							utility_tmp = 0;  // because a loop is effectively a draw
						}
					} 
					// check alpha value
					if(utility_tmp > utility) {
						// set as current alpha
						utility = utility_tmp;

						if(curr_ply == 1 && deepest_ply_curr_iter >= deepest_ply) {
							// make sure we only set the new way to go if this iteration had time to go deeper
							piece_to_move = piece;
							dir_to_move = dir;
							new_piece_position = bits_dir[dir];
							new_bb = bb_dir[dir];
						}
					}
				}    
				// break if we have reached max possible alpha
				// or if beta is less than or qual to alpha
				if(utility >= beta) {
					break;
				}
				if(utility > alpha )
					alpha = utility;
			}
			// break if we have reached max possible alpha
			// or if beta is less than or qual to alpha
			if(utility >= beta)
				break;
		}
		return utility;
	} else {
		// player 2 moves
		// player 2 is MIN, player 2 wants the min possible utility
		utility = alpha_max;

		// try the 7 possible pieces
		for(piece_idx = 0; piece_idx < 7; piece_idx++) {
			uint64_t bits_dir[4];
			uint64_t bb_dir[4];   
			
			piece = piece_order[piece_idx];
			
			// try the 4 possible directions
			for (dir_idx = 0; dir_idx < 4; dir_idx++) {
				dir = dir_order[dir_idx];
				bits_dir[dir] = bits_2[piece];
				bb_dir[dir] = bb_2;

				if (trydir(dir, &(bits_dir[dir]), &(bb_dir[dir]), bb_1) == 0) {
					// possible to move this direction
					int utility_tmp;
					// check if state has been visited
					int* set_p;
					int* value_p;
					/* note: not currently storing states */
					if(1) { //has_visited_state(bb_dir[dir], bb_2, &set_p, &value_p) == 1) {
						// new state!
						// need a copy of the modified board for each try
						uint64_t bits_tmp[7];
						memcpy(bits_tmp, bits_2, 7*sizeof(uint64_t));
						bits_tmp[piece] = bits_dir[dir];
						// perform minimax on this move
						utility_tmp = alphabeta(1, alpha, beta, bb_1, bb_dir[dir], bits_1, bits_tmp, curr_ply);
						// store the eval of this state and mark it as set
						// *value_p = utility_tmp;
						// *set_p = 1;
					} else {
						// state has alreayd been visited
						// check if the value has been set
						// if not, its a loop, set val to 0
						if(set_p) {
							utility_tmp = *value_p;
						} else {
							utility_tmp = 0;  // because a loop is effectively a draw
						}
					}
					// check value
					if(utility_tmp < utility) {
						// set as current util
						utility = utility_tmp; 
					}   
				}
				// break if we have reached min possible beta
				// or if beta is less than or qual to alpha
				if(utility <= alpha) {
					break;
				}
				if(utility < beta )
					beta = utility;
			}
			// break if we have reached min possible beta
			// or if beta is less than or qual to alpha
			if(utility <= alpha)
				break;
		}
		return utility;
	}
}

/* evalutation function
	* takes the state of the board in individual pieces as input
	* retuns a value between INT_MAX and INT_MIN
	* the higher the value, the better for player 1
	*/
int eval1(uint64_t bits_1[], uint64_t bits_2[]) {
	int i, j;
	int sum_1 = 0, sum_2 = 0;
	int pos_start[2];
	int pos_end[2];
	int x_diff, y_diff;

	for(i = 0; i < 7; i++) {
		position_lookup(bits_1[i], pos_start);
		// add up the distances squared of player 1
		for(j = 0; j < 7; j++) {
			position_lookup(bits_1[j], pos_end);
			// if(pos_end[0] < 0 || pos_end[1] < 0)
			//  printf("bit: %llu\n", bits_1[j]);
			// x distance
			x_diff = abs(pos_start[0] - pos_end[0]);
			// since 7 is the max distance
			// if they are 7 appart, we don't worry
			x_diff = (7 - x_diff)*2;
			// same for y distance
			y_diff = abs(pos_start[1] - pos_end[1]);
			y_diff = (7 - y_diff)*2;
			// square it - the closer they are, the more dangerous it is
			sum_1 += (x_diff*x_diff) + (y_diff*y_diff);

		}
	}

	for(i = 0; i < 7; i++) {
		position_lookup(bits_2[i], pos_start);
		// add up the distances squared of player 2
		for(j = 0; j < 7; j++) {
			position_lookup(bits_2[i], pos_end);
			// x distance
			x_diff = abs(pos_start[0] - pos_end[0]);
			// since 7 is the max distance
			// if they are 7 appart, we don't worry
			x_diff = (7 - x_diff)*2;
			// same for y distance
			y_diff = abs(pos_start[1] - pos_end[1]);
			y_diff = (7 - y_diff)*2;
			// square it - the closer they are, the more dangerous it is
			sum_2 += (x_diff*x_diff) + (y_diff*y_diff);
		}
	}
	return sum_1 - sum_2;
}

/* calls the try<direction> function for the given dir */
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
	return 1; // not possible if incompatible dir is specified
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
	if (piece_tmp & BORDER_W)
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
	if (piece_tmp & BORDER_E)
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
	case 8192:
		pos[0] = 6; pos[1] = 2; break;
	case 16384:
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