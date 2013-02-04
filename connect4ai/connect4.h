#ifndef CONNECT4_H_
#define CONNECT4_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include "ai.h"
#include "printstate.h"

#endif /* CONNECT4_H_ */

int showstate();
int printhelp();
int parsefile(char *ifile);
int getinput();
int validatemove();
int executemove();