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
typedef enum {minimax_ai, alphabeta_ai, ab_iter_ai, eval1_ai, eval2_ai} ai_t;

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
	struct node *next;
} list_t, *list_p;

#endif /* AI_H_ */

uint64_t generate_key(uint64_t w, uint64_t b);
int has_visited_state(uint64_t bitboard_white, uint64_t bitboard_black);
int trydir(int dir, uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int trynorth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int trysouth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int tryeast(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int trywest(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int check_endgame(uint64_t board);
int position_lookup(uint64_t piece, int position[]);
int delete_hashtable();
int free_hashtable();
int play_ai_turn(	turn_t turn,
					ai_t ai,
					uint64_t *bb_1,
					uint64_t bb_2,
					uint64_t bits_1[],
					uint64_t bits_2[],
					int d_cutoff);

int minimax(int turn,
			uint64_t bb_1,
			uint64_t bb_2,
			uint64_t bits_1[],
			uint64_t bits_2[],
			int curr_ply);
int alphabeta(	int turn,
				int alpha,
				int beta,
				uint64_t bb_1,
				uint64_t bb_2,
				uint64_t bits_1[],
				uint64_t bits_2[],
				int curr_ply);
int eval1(	int turn,
			int alpha,
			int beta,
			uint64_t bb_1,
			uint64_t bb_2,
			uint64_t bits_1[],
			uint64_t bits_2[],
			int curr_ply);