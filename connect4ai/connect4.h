#ifndef CONNECT4_H_
#define CONNECT4_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include "ai.h"

int bitboard_lookup[56][2] = { \
{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1},{-1,-1},
{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2},{-1,-1},
{1,3},{2,3},{3,3},{4,3},{5,3},{6,3},{7,3},{-1,-1},
{1,4},{2,4},{3,4},{4,4},{5,4},{6,4},{7,4},{-1,-1},
{1,5},{2,5},{3,5},{4,5},{5,5},{6,5},{7,5},{-1,-1},
{1,6},{2,6},{3,6},{4,6},{5,6},{6,6},{7,6},{-1,-1},
{1,7},{2,7},{3,7},{4,7},{5,7},{6,7},{7,7},{-1,-1}};

#endif /* CONNECT4_H_ */

int showstate();
int showstate_old();
int printhelp();
int parsefile(char *ifile);
int parsefile_old(char *ifile);
int getinput();
int validatemove();
int executemove();