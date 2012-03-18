/*
 * elevator simulator
 *
 * ECSE 427
 * Stepan Salenikovich
 * 260326129
 * Feb 27, 2012
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void print_usage(void);
int process_args(int argc, char** argv);
int advance_clock(void);
int process_request(int unsigned id, int unsigned current_floor,
		int unsigned requested_floor);
void *advance_worker(void* ptr);
int random_in_range(unsigned int min, unsigned int max, int seed);

typedef enum {
	up, down, stopped
} direction;

typedef enum {
	working, waiting, in_elevator
} worker_state;

typedef struct {
	direction direction;
	direction new_request; // used to decide on direction when elevator is empty
	int unsigned new_request_floor; // records floor of the new request
	int unsigned current_floor;
	int unsigned people; // number of people currently in elevator
	int unsigned waiting; // number of people waiting for elevator
	pthread_mutex_t mutex;
} elevator;

typedef struct {
	worker_state state;
	int unsigned current_floor;
	int unsigned requested_floor;
	direction direction; // based on current floor and requested floor
} worker;

/* arg globals */
int people = 10;
int floors = 5;
int capacity = 5;
int max_sleep = 10; //max time in tics a worker will spend working
int period = 1;
int end_time = 20;
int level = 1;
int summary = 10;

/* state globals */
int current_time = 0; // time in tics of the simulation
int running = 1; // 1 = simulation is running; 0 = simulation is over; kept by clock?
worker* workers; //array of workers

// fun fact: "lift" is the Russian word for elevator
elevator lift = { .direction = stopped, .new_request = stopped,
		.new_request_floor = 0, .current_floor = 1, .people = 0, .mutex =
				PTHREAD_MUTEX_INITIALIZER };
int unsigned workers_waiting_to_enter = 0;
pthread_cond_t waiting_notifier = PTHREAD_COND_INITIALIZER; // used to notify waiting workers when elevator has changed floor
pthread_cond_t in_elevator_notifier = PTHREAD_COND_INITIALIZER; // used to notify workers in the elevator

int transported = 0; //keeps track of how many people transported
void print_usage() {
	printf(
			"usage:\n"
					"elevator [people] [floors] [capacity] [period] [end time] [level] [summary]\n"
					"\tpeople     - number of people who will be using the elevator\n"
					"\tfloors     - number of floors in the building\n"
					"\tcapacity   - max number of people in the elevator\n"
					"\tperiod     - time in seconds elevator spends on a floor\n"
					"\t             (may be longer if people are coming off the elevator)\n"
					"\tend time   - number of tics after which simulation is over\n"
					"\tlevel      - detail level of simulation printout: 1, 2, or 3\n"
					"\t             (1 is most detailed, 3 is least detailed)\n"
					"\tsummary    - the period in tics with which to print out the simulation summary\n"
					"\t             (must be less than or equal to the end time\n"
					"\n"
					"running elevator with no arguments will initialize these to their default values:\n"
					"elevator 10 5 5 10 1 20 1 10\n");
}

int process_args(int argc, char** argv) {
	int return_value = 0;

	if (argc == 1) {
		// no args
		printf("initializing to default values: %d %d %d %d %d %d %d\n", people,
				floors, capacity, period, end_time, level, summary);
	} else if (argc == 8) {
		// get args
		people = atoi(argv[1]);
		floors = atoi(argv[2]);
		capacity = atoi(argv[3]);
		period = atoi(argv[4]);
		end_time = atoi(argv[5]);
		level = atoi(argv[6]);
		summary = atoi(argv[7]);

		// check args; value of 0 and less is not allowed or indicates no conversion possible
		if (people <= 0 || floors <= 0 || capacity <= 0 || period <= 0
				|| end_time <= 0 || level <= 0 || summary <= 0) {
			print_usage();
			return_value = 1;
		} else if (level < 1 || level > 3 || summary > end_time) {
			// some basic arg limits
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
	int i;
	int unsigned *ids;

	//check for correct input
	process_args(argc, argv);

	//create people threads
	workers = (worker*) malloc(sizeof(worker) * people); //array of workers
	pthread_t threads[people];
	ids = (int unsigned*) malloc(sizeof(int) * people);

	for (i = 0; i < people; i++) {
		ids[i] = i;
		workers[i].state = working;
		workers[i].current_floor = 1;
		workers[i].requested_floor = 1;
		workers[i].direction = up; //doesn't matter, but they will always go up first

		pthread_create(&threads[i], NULL, advance_worker, ((void *) &ids[i]));
	}

	//start the clock
	printf("starting clock\n");
	advance_clock();
	printf("ending simulation\n");

	//wait for threads to end
	for (i = 0; i < people; i++) {
		pthread_join(threads[i], NULL);
	}

	return 0;
}
/*essentially the clock thread*/
int advance_clock(void) {

	while (current_time <= end_time) {
		// sleep, let stuff happen
		sleep(period);
		// get elevator mutex
		pthread_mutex_lock(&lift.mutex);

		// clear new request when someone enters
		// not the best solution, but makes sure that the elevator is always sweeping and serving someone
		if (lift.people != 0) {
			lift.new_request = stopped;
		}

		if (lift.direction == up) {
			if (lift.current_floor != floors) {
				// if not at the top floor, increment floors
				// otherwise do nothing as we're probably waiting for people to exit
				lift.current_floor++;
			}
		} else if (lift.direction == down) {
			if (lift.current_floor != 1) {
				// if not at the bottom floor, decrement floors
				// otherwise do nothing as we're probably waiting for people to exit
				lift.current_floor--;
			}
		}

		if (lift.people == 0) {
			if (lift.waiting == 0) {
				//if no one waiting, stop
				lift.direction = stopped;
			} else if (lift.new_request != stopped) {
				// if people waiting, take the new request and go there
				lift.direction = lift.new_request;
			}
		}

		if (level < 3) {
			printf(
					"\n-- TIC %d--\n\nlift on floor %d with %d in and %d waiting; ",
					current_time, lift.current_floor, lift.people,
					lift.waiting);
			if (lift.direction == up)
				printf("going UP\n");
			if (lift.direction == down)
				printf("going DOWN\n");
			if (lift.direction == stopped)
				printf("STOPPED\n");
		}

		if(current_time % summary == 0) {
			printf("\n summary:\n%d workers transported in the last %d tics\n\n", transported, current_time);
		}

		current_time++;
		pthread_mutex_unlock(&lift.mutex);

		// broadcast to those waiting to get on that time has advanced
		pthread_cond_broadcast(&waiting_notifier);
		// broadcast to everyone in the elvator that time has advanced
		pthread_cond_broadcast(&in_elevator_notifier);
	}

	running = 0;

	return 0;
}

/* used only by the worker threads to make request, get on elevator, or get off the elevator */
int process_request(int unsigned id, int unsigned current_floor,
		int unsigned requested_floor) {
	// get elevator mutex
	pthread_mutex_lock(&lift.mutex);

	lift.waiting++;

	if (level == 1)
		printf("worker %d on %d waiting to go to %d\n", id,
				workers[id].current_floor, workers[id].requested_floor);

	// workers wait for when the elevator is not full and on the the same floor and going the same direction
	while (!(workers[id].state == waiting
			&& lift.current_floor == workers[id].current_floor
			&& lift.people < capacity && lift.direction == workers[id].direction)) {
		pthread_cond_wait(&waiting_notifier, &lift.mutex);

		//if elevator is stopped, make it go towards the requestor
		// not perfect solution, but will do for now
		if ((lift.direction == stopped || lift.people == 0)
				&& lift.new_request == stopped) {

			if (level < 3) {
				printf("lift stopped or empty on %d\n", lift.current_floor);
				printf(
						"worker %d on %d waiting to go to %d has requested lift ",
						id, workers[id].current_floor,
						workers[id].requested_floor);
			}

			if (lift.current_floor > workers[id].current_floor) {
				//lift above, so go down
				lift.new_request = down;
				if (level < 3)
					printf("DOWN\n");
			} else if (lift.current_floor < workers[id].current_floor) {
				// lift below, so go up
				lift.new_request = up;
				if (level < 3)
					printf("UP\n");
			} else {
				// lift on same floor, so go towards destination
				lift.new_request = workers[id].direction;

				if (lift.new_request == up) {
					if (level < 3)
						printf("UP\n");
				} else if (level < 3)
					printf("DOWN\n");
			}

		}
	}
	//workers_waiting_to_enter++;

	if (level < 3)
		printf("worker %d on %d waiting to go to %d has entered lift\n", id,
				workers[id].current_floor, workers[id].requested_floor);

	//enter elevator
	lift.people++;
	lift.waiting--;
	if (level < 3)
		printf("%d workers in lift\n", lift.people);
	workers[id].state = in_elevator;
	pthread_mutex_unlock(&lift.mutex);

	//ride da elevator!

	pthread_mutex_lock(&lift.mutex);
	// workers wait for when the elevator gets to their floor
	while (!(workers[id].state == in_elevator
			&& lift.current_floor == workers[id].requested_floor)) {
		pthread_cond_wait(&in_elevator_notifier, &lift.mutex);

	}

	if (level < 3)
		printf("worker %d from %d exited at %d\n", id,
				workers[id].current_floor, workers[id].requested_floor);
	transported++;
	//exit the elevator
	lift.people--;

	if (level < 3)
		printf("%d workers in lift\n", lift.people);
	workers[id].state = working;
	workers[id].current_floor = lift.current_floor;
	pthread_mutex_unlock(&lift.mutex);

	return 0;
}

void *advance_worker(void* ptr) {
	int unsigned next_request_time = 0;
	int this_id = *((int *) ptr);

	// run until the end of simulation
	while (running == 1) {
		if (next_request_time == 0
				|| (next_request_time < current_time
						&& workers[this_id].state == working)) {
			next_request_time = current_time
					+ random_in_range(1, max_sleep, this_id);
			workers[this_id].state = waiting;
		} else if (next_request_time <= current_time
				&& workers[this_id].state == waiting) {
			;
			// generate next request, and make sure its not the current floor
			while (workers[this_id].requested_floor
					== workers[this_id].current_floor) {
				srand(this_id); //use system time to get seed
				workers[this_id].requested_floor = random_in_range(1, floors,
						this_id);
			}
			// set worker's direction based on requested floor
			if (workers[this_id].requested_floor
					> workers[this_id].current_floor) {
				workers[this_id].direction = up;
			} else {
				workers[this_id].direction = down;
			}
			process_request(this_id, workers[this_id].current_floor,
					workers[this_id].requested_floor);
		}
	}

	return 0;
}

/* Would like a semi-open interval [min, max)
 * NOTE: this code was taken form a StackOverflow answer
 */
int random_in_range(unsigned int min, unsigned int max, int seed) {
	srand(seed);
	int base_random = rand(); /* in [0, RAND_MAX] */
	if (RAND_MAX == base_random)
		return random_in_range(min, max, seed);
	/* now guaranteed to be in [0, RAND_MAX) */
	int range = max - min, remainder = RAND_MAX % range, bucket = RAND_MAX
			/ range;
	/* There are range buckets, plus one smaller interval
	 within remainder of RAND_MAX */
	if (base_random < RAND_MAX - remainder) {
		return min + base_random / bucket;
	} else {
		return random_in_range(min, max, seed);
	}
}
