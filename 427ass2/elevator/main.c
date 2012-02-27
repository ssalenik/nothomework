/*
 * main.c
 *
 *  Created on: Feb 26, 2012
 *      Author: test
 */
#include "main.h"

//globals
int people = 100;
int floors = 20;
int capacity = 10;
int end_time = 1000;
int level = 1;
int period = 100;

void print_usage() {
	printf(
			"usage:\n"
					"elevator [people] [floors] [capacity] [end time] [level] [period]\n"
					"\tpeople     - number of people who will be using the elevator\n"
					"\tfloors     - number of floors in the building\n"
					"\tcapacity   - max number of people in the elevator\n"
					"\tend time   - number of tics after which simulation is over\n"
					"\tlevel      - detail level of simulation printout: 1, 2, or 3\n"
					"\t             (1 is most detailed, 3 is least detailed)\n"
					"\tperiod     - the period in tics with which to print out the simulation summary\n"
					"\t             (must be less than or equal to the end time\n"
					"\n"
					"running elevator with no arguments will initialize these to their default values:\n"
					"elevator 100 20 10 1000 1 100\n");
}

int process_args(int argc, char** argv) {
	int return_value = 0;

	if (argc == 1) {
		// no args
		printf("initializing to default values: %d %d %d %d %d %d\n", people,
				floors, capacity, end_time, level, period);
	} else if (argc == 7) {
		// get args
		people = atoi(argv[1]);
		floors = atoi(argv[2]);
		capacity = atoi(argv[3]);
		end_time = atoi(argv[4]);
		level = atoi(argv[5]);
		period = atoi(argv[6]);

		// check args; value of 0 and less is not allowed or indicates no conversion possible
		if (people <= 0 || floors <= 0 || capacity <= 0 || end_time <= 0
				|| level <= 0 || period <= 0) {
			//reset values
			print_usage();
			return_value = 1;
		} else if (level < 1 || level > 3 || period > end_time) {
			// some basic arg limits
			//reset values
			print_usage();
			return_value = 1;
		}
	} else {
		// wrong number of args
		print_usage();
		return_value = 1;
	}

	return return_value;
}

int main(int argc, char** argv) {

	//check for correct input
	process_args(argc, argv);

	return 0;
}
