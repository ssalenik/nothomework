/*
 * main.h
 *
 *  Created on: Feb 26, 2012
 *      Author: test
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void print_usage(void);
int process_args(int argc, char** argv);
int advance_clock(void);
int move_elevator(void);
int process_request(int unsigned id, int unsigned current_floor, int unsigned requested_floor);
void *advance_worker(void* ptr);
int random_in_range (unsigned int min, unsigned int max, int seed);

typedef enum {
	up, down, stopped
}direction;

typedef enum {
	working, waiting, in_elevator
}worker_state;

typedef struct {
	direction direction;
	direction new_request;  // used to decide on direction when elevator is empty
	int unsigned new_request_floor; // records floor of the new request
	int unsigned current_floor;
	int unsigned people; // number of people currently in elevator
	int unsigned waiting; // number of people waiting for elevator
	pthread_mutex_t mutex;
}elevator;

typedef struct {
	worker_state state;
	int unsigned current_floor;
	int unsigned requested_floor;
	direction direction; // based on current floor and requested floor
}worker;

#endif /* MAIN_H_ */
