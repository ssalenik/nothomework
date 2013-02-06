connect 4 AI by Stepan Salenikovich

---------------------------
ususage: connect4 [options]
options:
        -w              user plays as white
        -b              user plays as black
        -f <file>       specify input file for starting positions
        -d <number>     specify the depth cut-off (default 30)
        -1              use minimax AI
        -2              use alpha-beta AI
        -3              use alpha-beta AI with itterative deepening
        -4              use the eval function with alpha-beta AI and itterative deepening (default)
in-game commands:
        move:   <x,y,direction>; eg: 12W - will move the piece at <1,2> to the right
input file:
        The input file should contain comma sperated starting locations for each piece.
        The first line contains the positions for all of the white pieces, and then
        the second line contains the position for all the black pieces.
        example for indicating the default staring position:
                11,13,15,17,72,74,76
                12,14,16,71,73,75,77

---------------------------
To compile the program, simply run 'make optimze' - this will compile the program with -O3 optimizaton for improved performance.
Running just 'make' will compile with the default '-o' option.
Running 'make debug' will compile for running in gdb.
