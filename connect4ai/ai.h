#ifndef AI_H_
#define AI_H_

#include <inttypes.h>
#include "khash.h"
typedef enum {white, black} turn_t;

struct linked_list {
   uint64_t bitboard_white;
   uint64_t bitboard_black;
   struct linked_list* next;
};

typedef struct linked_list bb_state_t;

#endif /* AI_H_ */

int check_endgame(uint64_t board);
int minimax(turn_t turn,
			uint64_t bitboard_white,
			uint64_t bitboard_black,
			uint64_t white_bits[],
			uint64_t black_bits[],
			int depth_cutoff,
			int* max_depth,
			int* states_visited,
			void* h);