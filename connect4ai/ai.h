#ifndef AI_H_
#define AI_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include "khash.h"
typedef enum {white, black} turn_t;

// struct linked_list {
//    uint64_t bitboard_white;
//    uint64_t bitboard_black;
//    struct linked_list* next;
// };

typedef struct node {
 	uint64_t b_white;
	uint64_t b_black;
	struct node *next;
} list_t;

#endif /* AI_H_ */

uint64_t generate_key(uint64_t w, uint64_t b);
int has_visited_state(uint64_t bitboard_white, uint64_t bitboard_black);
int north(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int south(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int east(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int west(uint64_t *piece, uint64_t *bb_own, uint64_t bb_oponent);
int check_endgame(uint64_t board);
int ai_turn(turn_t turn,
			uint64_t bitboard_white,
			uint64_t bitboard_black,
			uint64_t white_bits[],
			uint64_t black_bits[],
			int d_cutoff,
			int* states_visited);

int minimax(turn_t turn,
			uint64_t bitboard_white,
			uint64_t bitboard_black,
			uint64_t white_bits[],
			uint64_t black_bits[],
			int curr_ply);