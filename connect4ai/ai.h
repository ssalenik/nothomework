#ifndef AI_H_
#define AI_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include "khash.h"
#include "printstate.h"

typedef enum {white, black} turn_t;
#define N 0
#define E 1
#define W 2
#define S 3

typedef struct node {
 	uint64_t b_white;
	uint64_t b_black;
	struct node *next;
} list_t;

#endif /* AI_H_ */

uint64_t generate_key(uint64_t w, uint64_t b);
int has_visited_state(uint64_t bitboard_white, uint64_t bitboard_black);
int trynorth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int trysouth(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int tryeast(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int trywest(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int check_endgame(uint64_t board);
int ai_turn(uint64_t bb_1,
			uint64_t bb_2,
			uint64_t bits_1[],
			uint64_t bits_2[],
			int d_cutoff,
			int* states);

int minimax(int turn,
			uint64_t bb_1,
			uint64_t bb_2,
			uint64_t bits_1[],
			uint64_t bits_2[],
			int curr_ply);