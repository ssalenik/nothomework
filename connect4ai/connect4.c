#include "connect4.h"

#define DEFAULT_START "default.txt"	//used if no input file is specified


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

uint64_t white_bits[7] = {1,1,1,1,1,1,1};	// position of individual white pieces
uint64_t black_bits[7] = {1,1,1,1,1,1,1};	// position of individual black pieces

/* prints the help */
int printhelp() {
	printf("usage: connect4 [options]\n"
			"options:\n"
			"\t-w\t\tuser plays as white\n"
			"\t-b\t\tuser plays as black\n"
			"\t-f <file>\tspecify input file for starting positions\n"
			"\t-d <number>\tspecify the depth cut-off (default 30)\n"
			"\t-1\t\tuse minimax AI\n"
			"\t-2\t\tuse alpha-beta AI\n"
			"\t-3\t\tuse alpha-beta AI with itterative deepening\n"
			"\t-4\t\tuse the eval function with alpha-beta AI and itterative deepening (default)\n"
			"in-game commands:\n"
			"\tmove:\t<x,y,direction>; eg: 12W - will move the piece at <1,2> to the right\n"
			"input file:\n"
			"\tThe input file should contain comma sperated starting locations for each piece.\n"
			"\tThe first line contains the positions for all of the white pieces, and then\n"
			"\tthe second line contains the position for all the black pieces.\n"
			"\texample for indicating the default staring position:\n"
			"\t\t11,13,15,17,72,74,76\n"
			"\t\t12,14,16,71,73,75,77\n");

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

int main(int argc, char** argv) {
	int c;
    int errflg = 0, fflg = 0;
    char *ifile;
    extern char *optarg;
    extern int optind, optopt;

    ai_t ai = eval1_ai; // the default ai, if none is specified
    int depth_cutoff = 30; 	// the default search depth cut-off

    turn_t ai_turn = white;	// the default first turn is white

	/* parse command line args */
	while ((c = getopt(argc, argv, "wbh12345d:f:")) != -1) {
        switch(c) {
        case 'w':
        	//AI white
        	ai_turn = white;
            printf("AI white - human black\n");
            break;
        case 'b':
        	//AI black
        	ai_turn = black;
            printf("human white - AI black\n");
            break;
        case '1':
        	// AI 1 - minimax
        	ai = minimax_ai;
            break;
        case '2':
        	// AI 2 - alpha-beta
        	ai = alphabeta_ai;
            break;
        case '3':
        	// AI 3 - iterative deepening alpha-beta
        	ai = ab_iter_ai;
            break;
        case '4':
        	// AI 4 - eval function 1
        	ai = eval1_ai;
            break;
        case '5':
        	// AI 5 - eval function 2
        	ai = eval2_ai;
            break;
        case 'h':
        	//print help
        	errflg++;
        	break;
       	case 'd':
        	//depth cutoff
            depth_cutoff = atoi(optarg);
            if(depth_cutoff == 0) {
            	fprintf(stderr,
                    "depth cut-off must be a non-zero integer\n");
           		errflg++;
            }
            break;
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

    // show the loaded state
    printf("starting position:\n");
	showstate(white_bits, black_bits);

	int result = 0;

	// check who starts
	if(ai_turn == white) {
		// AI starts
		printf("\nAI is white, AI starts.  Press any key to start AI turn.");
		
		getchar();	// wait for key to be pressed
		result = play_ai_turn(ai_turn, ai, &bitboard_white, bitboard_black, white_bits, black_bits, depth_cutoff);

		while(result == 0) {
			

			int bytes_read;
	  		int nbytes = 5;
	  		char *user_input = (char *) malloc (6);
	  		int inputerr = 1;
	  		int user_x, user_y, user_dir;

	  		printf("\nYour turn to move black:\n");

	  		while(inputerr != 0) {
	  			inputerr = 0;
		  		bytes_read = getline(&user_input, (size_t*)&nbytes, stdin);
		  		if(bytes_read == -1)
		  			inputerr++;
		  		user_x = (int)user_input[0] - 48;
		  		if(user_x < 1 || user_x > 7)
		  			inputerr++;
		  		user_y = (int)user_input[1] - 48;
		  		if(user_y < 1 || user_y > 7)
		  			inputerr++;
		  		switch(user_input[2]) {
		  		case 'N':
		  			user_dir = N;
		  			break;
		  		case 'E':
		  			user_dir = E;
		  			break;
		  		case 'W':
		  			user_dir = W;
		  			break;
		  		case 'S':
		  			user_dir = S;
		  			break;
		  		default:
		  			inputerr++;
		  		}
		  		if(inputerr) {
		  			printf("\nerror in input, try again:\n");
		  		} else {
		  			user_x = user_x - 1;
					user_y = user_y - 1;
					int bit_number = 8*user_y + user_x;
					uint64_t piece = (uint64_t)1 << bit_number;
					// make sure its a valid piece
					if(piece & bitboard_black) {
						int i = 0;
						while(piece != black_bits[i]){
							i++;
						}
		  				inputerr = trydir(user_dir, &piece, &bitboard_black, bitboard_white);
		  				if(inputerr == 0) {
		  					black_bits[i] = piece;
		  				}
		  			} else {
		  				inputerr++;
		  			}
		  			if(inputerr)
		  				printf("invalid move, try again:\n");
		  		}
		  	}

		  	showstate(white_bits, black_bits);
		  	printf("\nAI turn\n");
		  	result = play_ai_turn(ai_turn, ai, &bitboard_white, bitboard_black, white_bits, black_bits, depth_cutoff);
		}

	} else {
		// human starts, AI is black
		printf("AI is black, human starts.  Enter the first move to start.");

		while(result == 0) {

			int bytes_read;
	  		int nbytes = 5;
	  		char *user_input = (char *) malloc (nbytes + 1);

	  		int inputerr = 1;
	  		int user_x, user_y, user_dir;

	  		printf("\nYour turn to move white:\n");

	  		while(inputerr != 0) {
	  			inputerr = 0;
		  		bytes_read = getline(&user_input, (size_t*)&nbytes, stdin);
		  		if(bytes_read == -1)
		  			inputerr++;
		  		user_x = (int)user_input[0] - 48;
		  		if(user_x < 1 || user_x > 7)
		  			inputerr++;
		  		user_y = (int)user_input[1] - 48;
		  		if(user_y < 1 || user_y > 7)
		  			inputerr++;
		  		switch(user_input[2]) {
		  		case 'N':
		  			user_dir = N;
		  			break;
		  		case 'E':
		  			user_dir = E;
		  			break;
		  		case 'W':
		  			user_dir = W;
		  			break;
		  		case 'S':
		  			user_dir = S;
		  			break;
		  		default:
		  			inputerr++;
		  		}
		  		if(inputerr) {
		  			printf("\nerror in input, try again:\n");
		  		} else {
		  			user_x = user_x - 1;
					user_y = user_y - 1;
					int bit_number = 8*user_y + user_x;
					uint64_t piece = (uint64_t)1 << bit_number;
					// make sure its a valid piece
					if(piece & bitboard_white) {
						int i = 0;
						while(piece != white_bits[i]){
							i++;
						}
		  				inputerr = trydir(user_dir, &piece, &bitboard_white, bitboard_black);
		  				if(inputerr == 0) {
		  					white_bits[i] = piece;
		  				}
		  			} else {
		  				inputerr++;
		  			}
		  			if(inputerr)
		  				printf("invalid move, try again:\n");
		  		}
		  	}
		  	showstate(white_bits, black_bits);
			printf("\nAI turn\n");
			result = play_ai_turn(ai_turn, ai, &bitboard_black, bitboard_white, black_bits, white_bits, depth_cutoff);
		}
		
	}

	if(result == 1) {
		printf("\nAI wins!\n");
	} else {
		printf("\nThe winner is YOU!\n");
	}

	getchar();	

	return 0;
}