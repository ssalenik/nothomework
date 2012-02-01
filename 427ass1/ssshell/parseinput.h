/*
 * parseinput.h
 *
 *  Created on: Jan 31, 2012
 *      Author: test
 */

#ifndef PARSEINPUT_H_
#define PARSEINPUT_H_

#include <string.h>

#endif /* PARSEINPUT_H_ */

/* internal commands */
typedef enum {
	echo, cd, pwd, history, set, unset, pushd, popd, exitshell, external, none
} command;
