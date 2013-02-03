#include "connect4.h"

#define DEFAULT_START "default.txt"

/* game state */
char w = 'o';	//white
char b = 'x';	//black
char e = '.';	//empty

char empty_state[8][8] = {	{' ', '1', '2', '3', '4', '5', '6', '7'}, \
							{'1', '.', '.', '.', '.', '.', '.', '.'}, \
							{'2', '.', '.', '.', '.', '.', '.', '.'}, \
							{'3', '.', '.', '.', '.', '.', '.', '.'}, \
							{'4', '.', '.', '.', '.', '.', '.', '.'}, \
							{'5', '.', '.', '.', '.', '.', '.', '.'}, \
							{'6', '.', '.', '.', '.', '.', '.', '.'}, \
							{'7', '.', '.', '.', '.', '.', '.', '.'}};

/* bitboard representation of board
 *    1  2  3  4  5  6  7
 *	 ____________________
 * 1| 0, 1, 2, 3, 4, 5, 6, .
 * 2| 8, 9,10,11,12,13,14, .
 * 3|16,17,18,19,20,21,22, .
 * 4|24,25,26,27,28,29,30, .
 * 5|32,33,34,35,36,37,38, .
 * 6|40,41,42,43,44,45,46, .
 * 7|48,49,50,51,52,53,54, .
 * 
 * each number refers to the bit in the 49(64) bit u_int
 */
uint64_t bitboard_white = 0;	// the entire white board
uint64_t bitboard_black = 0;	// the entire black board

// uint64_t white_bits[7] = {1,1,1,1,1,1,1};	// position of individual white pieces
// uint64_t black_bits[7] = {1,1,1,1,1,1,1};	// position of individual black pieces
uint64_t white_bits[7] = {1,128,16384,2097152,268435456,34359738368,4398046511104};	// position of individual white pieces
uint64_t black_bits[7] = {64,8192,1048576,134217728,17179869184,2199023255552,281474976710656};	// position of individual black pieces



/*
 * prints the game state from the bitboard representation
 */
int showstate() {
	int i, j;
	printf("\n");
	char state_tmp[8][8];

	// copy empty state
	memcpy(state_tmp, empty_state, 64*sizeof(char));

	// get all the pieces
	uint64_t white_tmp;
	uint64_t black_tmp;
	int white_idx = 0;
	int black_idx = 0;
	for(i = 0; i < 7; i++) {
		white_tmp = white_bits[i];
		black_tmp = black_bits[i];
		white_idx = 0;
		black_idx = 0;
		// get the bit position of the white piece
		while(white_tmp != 1) {
			white_tmp = white_tmp >> 1;
			white_idx++;
		}
		// get the bit position of the black piece
		while(black_tmp != 1) {
			black_tmp = black_tmp >> 1;
			black_idx++;
		}
		// mark the board
		state_tmp[bitboard_lookup[white_idx][1]][bitboard_lookup[white_idx][0]] = w;
		state_tmp[bitboard_lookup[black_idx][1]][bitboard_lookup[black_idx][0]] = b;
	}

	// now print the board
	printf("\n");
	for(i = 0; i < 8; i++) {
		for(j = 0; j < 8; j++) {
			printf("%c ", state_tmp[i][j]);
		}
		printf("\n");
	}

	return 0;
}

/* prints the help */
int printhelp() {
	printf("usage: connect4 [options]\n"
			"options:\n"
			"\t-w\t\tuser plays as white\n"
			"\t-b\t\tuser plays as black\n"
			"\t-f <file>\tspecify input file for starting positions\n"
			"in-game commands:\n"
			"\tmove:\t<x,y,direction>; eg: 12W - will move the piece at <1,2> to the right\n"
			"\thelp:\ttype \'help\'\n"
			"\texit:\ttype \'exit\'\n\n"
			"input file:\n"
			"\tThe input file should contain comma sperated starting locations for each piece.\n"
			"\tThe first line contains the positions for all of the white pieces, and then\n"
			"\tthe second line contains the position for all the black pieces.\n"
			"\texample for indicating the default staring position:\n"
			"\t\t11,13,15,17,72,74,76\n"
			"\t\t12,14,16,71,73,75,77\n");

	return 0;
}

/* gets input from user */
int getinput() {

	return 0;
}
/* parse input file and save state into bitboards */
int parsefile(char *ifile) {
	FILE *file;
	int c, num;
	int x, y;
	enum read_state {white_x, white_y, black_x, black_y, delim, nl, done} state;
	char delimchar = ',';
	int num_white = 0, num_black = 0;  // number of white/black pieces set
	int bit_number = 0; //the bit index on the board

	// initial state is x coord of white
	state = white_x;

	file = fopen(ifile, "r");
	if (file) {
		printf("parsing file:\n\n\'%s\':\n", ifile);
		while((c = getc(file)) != EOF && state != done) {
			//check for valid input
			printf("%c", (char)c);
			if(state != delim && state != nl) {
				if ((char)c < '1' || (char)c > '7') {
					//invalid!
					fprintf(stderr, "parsing file - expected number (1 - 7), read %c\n", (char)c);
					exit(2);
				} else {
					// get the number from the char
					num = c - 48;
				}
			}
			//parse
			switch(state) {
			case white_x:
				x = num - 1;
				state = white_y;
				break;
			case white_y:
				y = num - 1;
				bit_number = 8*y + x;
				white_bits[num_white] = (uint64_t)1 << bit_number;
				num_white++;
				if (num_white >= 7) {
					state = nl;
				} else {
					state = delim;
				}
				break;
			case black_x:
				x = num - 1;
				state = black_y;
				break;
			case black_y:
				y = num - 1;
				bit_number = 8*y + x;
				black_bits[num_black] = (uint64_t)1 << bit_number;
				num_black++;
				if (num_black >= 7) {
					state = done;
				} else {
					state = delim;
				}
				break;
			case delim:
				if ((char)c != delimchar) {
					// invalid
					fprintf(stderr, "reading input file - expected \'%c\', read %c\n", delimchar, (char)c);
					return -1;
				} else if (num_white < 7) {
					state = white_x;
				} else {
					state = black_x;
				}
				break;
			case nl:
				if ((char)c != '\n') {
					// invalid
					fprintf(stderr, "reading input file - expected newline, read %c\n", (char)c);
					return -1;
				} else {
					state = black_x;
				}
				break;
			default:
				//error
				break;
			}
		}
		fclose(file);

		if(state != done) {
			// invalid file contents
			fprintf(stderr, "reached EOF - not all positions were set\n");
			return -1;
		} else {
			// set the black and white board state
			int i;
			for(i = 0; i < 7; i++) {
				bitboard_white = bitboard_white | white_bits[i];
				bitboard_black = bitboard_black | black_bits[i];
			}
			printf("\n\nfinished parsing file\n");
		}
	}

	return 0;
}

/* validates entered move */
int validatemove() {
	return 0;
}
/* changes game state to reflect move*/
int executemove() {

	return 0;
}

int main(int argc, char** argv) {
	int c;
    int errflg = 0, fflg = 0;
    char *ifile;
    extern char *optarg;
    extern int optind, optopt;

	/* parse command line args */
	while ((c = getopt(argc, argv, "wbhf:")) != -1) {
        switch(c) {
        case 'w':
        	//player white
            printf("player white\n");
            break;
        case 'b':
        	//player black
            printf("player black\n");
            break;
        case 'h':
        	//print help
        	errflg++;
        case 'f':
        	//input file
        	fflg++;
            ifile = optarg;
            break;
		case ':':       /* -f or -o without operand */
            fprintf(stderr,
                    "Option -%c requires an operand\n", optopt);
            errflg++;
            break;
        case '?':
            fprintf(stderr,
                	"Unrecognized option: -%c\n", optopt);
            errflg++;
        }
    }

    if (errflg) {
        printhelp();
        exit(2);
    }

    //if no input file set, set to default
    if (fflg == 0) {
    	ifile = DEFAULT_START;
    }

    //load starting position
    if(parsefile(ifile) != 0)
    	exit(1);

    printf("starting position:\n");
	showstate();

	turn_t turn = white;

	int done = 0;
	done = check_endgame(bitboard_white);

	printf("\n");

	switch(done) {
	case 0:
		printf("no winner\n");
		break;
	case 1:
		printf("white wins horizontal\n");
		break;
	case 2:
		printf("white wins vertical\n");
		break;
	case 3:
		printf("white wins diagonal /\n");
		break;
	case 4:
		printf("white wins diagonal \\\n");
		break;
	}
	printf("\n");

    int states_visited;
    //ai_turn(turn, bitboard_white, bitboard_black, white_bits, black_bits, 5, &states_visited);

    //north(&white_bits[3], &bitboard_white, bitboard_black);
    //south(&white_bits[3], &bitboard_white, bitboard_black);
    int result = east(&white_bits[5], &bitboard_white, bitboard_black);
    //west(&white_bits[3], &bitboard_white, bitboard_black);
    printf("result: %i\n", result);
    showstate();
    return 0;

	// while(!done) {
	// 	if(turn == white) {
	// 		printf("white turn:\n");

	// 	} else {
	// 		printf("black turn:\n");
	// 	}
	// }


	return 0;
}