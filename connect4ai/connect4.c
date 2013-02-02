#include "connect4.h"

#define DEFAULT_START "default.txt"

/* game state */
enum player_turn {white, black} turn;

char w = 'o';	//white
char b = 'x';	//black
char e = '.';	//empty

char game_state[8][8] = {	{' ', '1', '2', '3', '4', '5', '6', '7'}, \
							{'1', '.', '.', '.', '.', '.', '.', '.'}, \
							{'2', '.', '.', '.', '.', '.', '.', '.'}, \
							{'3', '.', '.', '.', '.', '.', '.', '.'}, \
							{'4', '.', '.', '.', '.', '.', '.', '.'}, \
							{'5', '.', '.', '.', '.', '.', '.', '.'}, \
							{'6', '.', '.', '.', '.', '.', '.', '.'}, \
							{'7', '.', '.', '.', '.', '.', '.', '.'}};

uint64_t bitboard_white = 0;
uint64_t bitboard_black = 1;

/*
 * prints the game state
 */
int showstate() {
	int i, j;
	printf("\n");
	for(i = 0; i < 8; i++) {
		for(j = 0; j < 8; j++) {
			printf("%c ", game_state[i][j]);
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
/* parse input file */
int parsefile(char *ifile) {
	FILE *file;
	int c, num;
	int x, y;
	enum read_state {white_x, white_y, black_x, black_y, delim, nl, done} state;
	char delimchar = ',';
	int num_white = 0, num_black = 0;  // number of white/black pieces set

	// initial state is x coord of white
	state = white_x;

	file = fopen(ifile, "r");
	if (file) {
		printf("parsing file:\n");
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
				x = num;
				state = white_y;
				break;
			case white_y:
				y = num;
				game_state[y][x] = w;
				num_white++;
				if (num_white >= 7) {
					state = nl;
				} else {
					state = delim;
				}
				break;
			case black_x:
				x = num;
				state = black_y;
				break;
			case black_y:
				y = num;
				game_state[y][x] = b;
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
			printf("\nfinished parsing file\n");
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

	showstate();

	turn = white;

	int done = 0;
	while(!done) {
		if(turn == white) {
			printf("white turn:\n");

		} else {
			printf("black turn:\n");
		}
	}


	return 0;
}