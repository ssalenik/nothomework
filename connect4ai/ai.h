/**
 * Stepan Salenikovich
 * ECSE 526
 * Assignment 1
 *
 * ai.h
 */
#ifndef AI_H_
#define AI_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

#include "khash.h"
#include "printstate.h"

/* ai algo selection */
typedef enum {minimax_ai, alphabeta_ai, ab_iter_ai, eval1_ai, eval1_iter_ai} ai_t;

/* white/black turn selection */
typedef enum {white, black} turn_t;

/* directions */
#define N 0
#define E 1
#define W 2
#define S 3

/* linked list struct */
typedef struct node {
  uint64_t b_white;
 uint64_t b_black;
 int set; // 0 if not yet set, 1 otherwise
 int value;  // the eval of this state, if found
 struct node *next;
} list_t, *list_p;

#endif /* AI_H_ */

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
	 int d_cutoff);

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
   int curr_ply);

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
	int curr_ply);

/* evalutation function
 * takes the state of the board in individual pieces as input
 * retuns a value between INT_MAX and INT_MIN
 * the higher the value, the better for player 1
 */
int eval1(uint64_t bits_1[], uint64_t bits_2[]);

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
int check_endgame(uint64_t board);

/* checks time passed stince start of turn
 * 1 if running out of time, 0 otherwise
 */
int check_time();

/* checks time passed between two vals
 * Return 1 if the difference is negative, otherwise 0
 */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);

/* calls the try<direction> function for the given dir */
int trydir(int dir, uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);

/* attempts to move the given piece <dir> one spot
 * returns 0 if successful, 1 if move not possible
 * updates piece and the corresponding bitboard if possible
 */
int trynorth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int trysouth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int tryeast(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int trywest(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);

/* returns the x,y position */
int position_lookup(uint64_t piece, int position[]);

/* function which checks if the state given has already been visited
 * does this by checking the hash table of states
 * if state is new, stores it in the hash table
 *
 * @return - 1 if new, 0 if visited already
 */
 int has_visited_state(uint64_t bitboard_white, uint64_t bitboard_black, int** set_p, int** value_p);

/* generates key for the hash table (not unique) */
uint64_t generate_key(uint64_t w, uint64_t b);

/* deletes all the key/values in the hash table; frees value memory */
int delete_hashtable();

/* frees all the value memory and then destroyes the hashtable */
int free_hashtable();