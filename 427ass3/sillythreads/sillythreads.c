/*
 * =====================================================================================
 *
 *       Filename:  sillythreads.h
 *
 *    Description:  Real SillyThreads Library File.
 *
 *        Version:  1.0
 *        Created:  03/18/2012 6:46:69 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Stepan Salenikovich, stepan.salenikovich@gmail.com
 *   Organization:  not organized
 *
 * =====================================================================================
 */
#include "sillythreads.h"

/* global vars */

// the runqueue
List *runqueue;
list_release_t *destroy_runqueue;

// thread table, index points to the thread_control_block of a thread
thread_control_block_t **threads;

// total number of threads, ie: the next thread id
int num_threads;

// the thread currently running
int current_thread;

// array of semaphores
semaphore_t **semaphores;

// total number of semaphores, ie: the next semaphore id
int num_semaphores;

// quantum size in ms
int quantum;

/**
 * Initialized global data structures:
 *
 * returns 0 on success, -1 on failure.
 *
 */
int init_my_threads() {
	/**
	 * 	 Initializes the semaphore table and sets the total number of active semaphores to 0.
	 * 	 An entry in the semaphore table is a struct that defines the complete state of the semaphore.
	 * 	 The table also has a queue to hold the threads that will be waiting on the semaphore.
	 *
	 */
	threads = (thread_control_block_t **) malloc(
			sizeof(thread_control_block_t *) * THREADS_MAX);
	num_threads = 0;

	runqueue = list_create(destroy_runqueue);

	semaphores = (semaphore_t **) malloc(
			sizeof(semaphore_t *) * SEMAPHORES_NUM);
	num_semaphores = 0;

	quantum = QUANTUM_DEFAULT;

	return 0;
}

/**
 * Creates a new thread.
 *
 * Returns an int that points to the thread control block that is allocated to the thread.
 * Returns -1 and prints error message on failure.
 *
 */
int create_my_thread(char *threadname, void(*threadfunc)(), int stacksize) {
	/* Allocate the stack and set up the user context.
	 *
	 * The new thread should run the threadfunc when it starts.
	 *
	 * threadname is stored in the thread control block.
	 *
	 * A new thread is in the RUNNABLE  state when it is inserted into the system.
	 *
	 * New thread *may* be included in the runqueue
	 *
	 */

	// first check if we can create any more threads
	if (num_semaphores == THREADS_MAX) {
		fprintf(stderr,
				"Cannot create new thread, max number of threads reached: %d\n",
				THREADS_MAX);
		return -1;
	}

	thread_control_block_t* new_thread = (thread_control_block_t *) malloc(
			sizeof(thread_control_block_t *));

	// init thread stuff
	new_thread->runtime = 0;
	new_thread->total_time = 0;
	new_thread->stacksize = stacksize;
	strcpy(new_thread->thread_name, threadname); // copy name to make sure its not changed externally
	new_thread->state = RUNNABLE;
	new_thread->state_str = "RUNNABLE";
	new_thread->thread_id = num_threads;
	new_thread->threadfunc = threadfunc;
	new_thread->context; //TODO

	// put in thread table
	threads[num_threads] = new_thread;

	num_threads++;

	return 0;
}

/**
 * Removes the thread from the runqueue.
 * To be called at the end of the function invoked by the thread.
 */
void exit_my_thread() {
	/**
	 * The entry in the thread control bloc may remain, the state should be set to EXIT
	 */

}

/**
 * Switches control from the main thread to one of the threads in the runqueue.
 * Activates the thread switcher.
 */
void runthreads() {

}

/**
 * Sets the quantum size of the round robin scheduler.
 */
void set_quantum_size(int quantum_size) {
	/**
	 * The scheduler picks the next thread from the runqueue and appends the current to the end of the runqueue
	 * Then switches to the new thread.
	 *
	 */
	quantum = quantum_size;
}

/**
 * Creates semaphore and sets the initial value.
 *
 * Returns the index of the semaphore on success, -1 on failure.
 */
int create_semaphore(int value) {
	/**
	 * Inserts an entry into the semaphore table initialized by init_my_threads()
	 *
	 */

	// check if we can make another semaphore
	// TODO: implement memory allocation if we need more semaphores
	if (num_semaphores == SEMAPHORES_NUM) {
		fprintf(stderr, "Cannot create new semaphore, too many: %d\n",
				SEMAPHORES_NUM);
		return -1;
	}

	// init new semaphore
	semaphore_t *sem = (semaphore_t *) malloc(sizeof(semaphore_t));
	sem->index = num_semaphores;
	sem->init_value = value;
	sem->value = value;
	sem->thread_queue = list_create(sem->queue_destructor);

	// add it to list of semaphores
	semaphores[num_semaphores] = sem;

	num_semaphores++;

	return 0;
}

/**
 * Decrements the value of the given semaphore.
 * If the value goes below 0, the thread is put into a WAIT state.
 */
void semaphore_wait(int semaphore) {
	/**
	 * WAIT - the thread is taken out of the runqueue
	 *
	 * accesses the semaphore record and manipulates its contents
	 */

}

/**
 * Increments the value of the given semaphore.
 * If the value is less than 1, then the thread at the top of the wait queue is put on the runqueue.
 */
void semaphore_signal(int semaphore) {
	/**
	 * if less than 1, takes the first WAIT thread and makes it RUNNABLE on the runqueue
	 */

}

/**
 * Removes the given semaphore.
 *
 * Fails and gives error message if any threads are still waiting on the given semaphore.
 *
 * Prints warning message if the current value of the semaphore is not the same as the initial value.
 */
void destroy_semaphore(int semaphore) {

}

/**
 * Prints the state of all the current threads.
 */
void my_threads_state() {
	/**
	 * print in tabular form:
	 * thread name, thread state, CPU run time
	 */
	int i;

	//TODO: ensure nice, tabular output formatting
	printf("State of Threads:\n");
	printf("|  Thread Name  |  State  |  run time  |");

	for (i = 0; i < num_threads; i++) {
		printf("|%15s|%9s|%12d|\n", threads[i]->thread_name,
				threads[i]->state_str, threads[i]->total_time);
	}

}
