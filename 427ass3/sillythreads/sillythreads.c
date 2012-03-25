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

// debug mode = 1
#define DEBUG 1

/* global vars */

// the runqueue
List *runqueue;
list_release_t *destroy_runqueue;

// thread table, index points to the thread_control_block of a thread
thread_control_block_t **threads;

// total number of threads, ie: the next thread id
int next_thread_index;

// the thread currently running
int current_thread;

// array of semaphores
semaphore_t **semaphores;

// total number of semaphores, ie: the next semaphore id
int next_semaphore_index;

// quantum size in us (microseconds)
int quantum;

// interrupt context
ucontext_t signal_context; /* the interrupt context */
void *signal_stack; /* global interrupt stack */

// main context
static ucontext_t main_context; //the main context, to be able to return to run_threads

//timer
struct itimerval it;

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
	 * 	 since list_shift_int returns 0 when no items remain, start indexing threads and semaphores at 1
	 *
	 */
	threads = (thread_control_block_t **) malloc(
			sizeof(thread_control_block_t *) * (THREADS_MAX + 1));
	threads[0] = NULL;
	next_thread_index = 1;

	runqueue = list_create(destroy_runqueue);

	semaphores = (semaphore_t **) malloc(
			sizeof(semaphore_t *) * (SEMAPHORES_NUM + 1));
	semaphores[0] = NULL;
	next_semaphore_index = 1;

	quantum = QUANTUM_DEFAULT;

	/* allocate the global signal/interrupt stack */
	signal_stack = malloc(4096);
	if (signal_stack == NULL) {
		perror("malloc");
		exit(1);
	}

	getcontext(&main_context);

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
	if (next_thread_index == (THREADS_MAX + 1)) {
		fprintf(stderr,
				"cannot create new thread, max number of threads reached: %d\n",
				THREADS_MAX);
		return -1;
	} else if (stacksize <= 0) {
		fprintf(stderr, "stacksize must be greater than zero!\n");
		return -1;
	}

	thread_control_block_t* new_thread = (thread_control_block_t *) malloc(
			sizeof(thread_control_block_t *));

	// init thread stuff
	new_thread->run_time = 0;
	new_thread->total_time = 0;
	new_thread->stacksize = stacksize;
	new_thread->thread_name = (char *)malloc(sizeof(char)*strlen(threadname));
	strcpy(new_thread->thread_name, threadname); // copy name to make sure its not changed externally
	new_thread->state = RUNNABLE;
	new_thread->thread_id = next_thread_index;
	new_thread->threadfunc = threadfunc;

	new_thread->context = (ucontext_t *)malloc(sizeof(ucontext_t));

	if (mkcontext(new_thread->context, threadfunc, stacksize) != 0) {
		fprintf(stderr, "thread context creation failed");
		return -1;
	}

	// put in thread table
	threads[next_thread_index] = new_thread;

	printf("new thread created: %s\n", threads[next_thread_index]->thread_name);

	next_thread_index++;

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

	threads[current_thread]->state = EXIT;
#if DEBUG == 1
		perror("thread ended");
#endif


	while (1)
		; // wait for next alarm to go to the scheduler

}

/**
 * Switches control from the main thread to one of the threads in the runqueue.
 * Activates the thread switcher.
 */
void runthreads() {
	int i;


	current_thread = 1;

	setup_signals();

	//queue all the threads
	for(i = 2; i < next_thread_index; i++) {
		list_append_int(runqueue, i);
	}

	/* setup our timer */
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = quantum;
	it.it_value = it.it_interval;
	if (setitimer(ITIMER_REAL, &it, NULL))
		perror("setitiimer");

	/* force a swap to the first context */
	printf("ready to start\n");
	swapcontext(&main_context, threads[current_thread]->context);

	printf("destorying runqueue\n");
	list_destroy(&runqueue);
}

/**
 * Sets the quantum size of the round robin scheduler in microseconds.
 * The minimum and default quantum is 1000us
 */
void set_quantum_size(int quantum_size) {
	if (quantum_size < QUANTUM_DEFAULT) {
		quantum = QUANTUM_DEFAULT;
		printf("warning: quantum size too small, setting to default: %dus\n",
				QUANTUM_DEFAULT);
	} else
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

	printf("creating semaphore %d with initial value %d\n", next_semaphore_index, value);

	// check if we can make another semaphore
	// TODO: implement memory allocation if we need more semaphores
	if (next_semaphore_index == (SEMAPHORES_NUM + 1)) {
		fprintf(stderr, "Cannot create new semaphore, too many: %d\n",
				SEMAPHORES_NUM);
		return -1;
	}

	// init new semaphore
	semaphore_t *sem = (semaphore_t *) malloc(sizeof(semaphore_t));
	sem->index = next_semaphore_index;
	sem->init_value = value;
	sem->value = value;
	sem->thread_queue = list_create(sem->queue_destructor);

	// add it to list of semaphores
	semaphores[next_semaphore_index] = sem;

	printf("semaphore %d created with value %d\n", semaphores[next_semaphore_index]->index, semaphores[next_semaphore_index]->value);

	next_semaphore_index++;

	return (next_semaphore_index - 1);
}

/**
 * Decrements the value of the given semaphore.
 * If the value goes below 0, the thread is put into a WAIT state.
 */
void semaphore_wait(int semaphore) {
	char print[100];
	// disable alarm while working with semaphore
	sighold(SIGALRM);
	sprintf(print, "value of semaphore %d is %d\n", semaphore,semaphores[semaphore]->value);
	perror(print);

	semaphores[semaphore]->value -= 1;
	if(semaphores[semaphore]->value < 0) {
		sprintf(print, "thread %d put on waitqueue\n", current_thread);
		perror(print);
		// block thread
		threads[current_thread]->state = WAIT;
		// put it on the wait queue
		list_append_int(semaphores[semaphore]->thread_queue, current_thread);

		//unblock alarm and wait for scheduler to take over
		sigrelse(SIGALRM);
		while(threads[current_thread]->state == WAIT); // signal should make the thread RUNNABLE again
	} else {
		//don't block thread, unblock alarm and go back to the thread
		sigrelse(SIGALRM);
	}
}

/**
 * Increments the value of the given semaphore.
 * If the value was  0, then the thread at the top of the wait queue is put on the runqueue.
 */
void semaphore_signal(int semaphore) {
	int next_thread;
	char print[100];
	// disable alarm while working with semaphore
	sighold(SIGALRM);
	sprintf(print, "signaling semaphore %d with value %d\n", semaphore,semaphores[semaphore]->value);
	perror(print);


	if(semaphores[semaphore]->value < 0) {

		// make first thread on the wait queue runnable
		next_thread = list_shift_int(semaphores[semaphore]->thread_queue);
		if(next_thread == 0) {
			perror("no threads on waitqueue\n");
			exit(-1);
		}

		sprintf(print, "signaling thread %d\n", next_thread);
				perror(print);

		threads[next_thread]->state = RUNNABLE;
		list_append_int(runqueue, next_thread);
	}
	semaphores[semaphore]->value += 1;
	sigrelse(SIGALRM);
}

/**
 * Removes the given semaphore.
 *
 * Fails and gives error message if any threads are still waiting on the given semaphore.
 *
 * Prints warning message if the current value of the semaphore is not the same as the initial value.
 */
void destroy_semaphore(int semaphore) {

	// check if any threads are still on the wait queue
	if(list_shift_int(semaphores[semaphore]->thread_queue) != 0 ) {
		perror("semaphore wait queue is not empty!\n");
		exit(-1);
	} else {
		// else decrement semaphore counter and detroy the queue
		next_semaphore_index--;
		list_destroy(&(semaphores[semaphore]->thread_queue));
	}

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
	printf("|  Thread Name  |  State  |  run time  |\n");

	for (i = 1; i < next_thread_index; i++) {
		printf("|%15s|%9s|%12d|\n", threads[i]->thread_name,
				state_str[threads[i]->state], threads[i]->total_time);
	}

}

/**
 * Modified version of the swap.c example provided on http://cgi.cs.mcgill.ca/~xcai9/2012_comp_310.html
 *
 * The scheduling algorithm; selects the next context to run, then starts it.
 */
void scheduler() {
	char print[100];

	sighold(SIGALRM);

	// increment current thread run time
	threads[current_thread]->total_time += quantum;

	// check if thread has completed
	if (threads[current_thread]->state == EXIT) {
#if DEBUG == 1
		sprintf(print, "thread %d completed, run time: %d\n", current_thread, threads[current_thread]->total_time);
		perror(print);
#endif

	} else if (threads[current_thread]->state == WAIT) {
#if DEBUG == 1
		sprintf(print, "thread %d blocked\n", current_thread);
		perror(print);
#endif
	} else {
		// else append it to the end of the runqueue
		threads[current_thread]->state = RUNNABLE;
		list_append_int(runqueue, current_thread);

#if DEBUG == 1
		sprintf(print, "thead %d put on runqueue, run time: %d\n", current_thread, threads[current_thread]->total_time);
		perror(print);
#endif
	}

	// get the next thread waiting to run from the run queue
	current_thread = list_shift_int(runqueue);

	// check if there is a next item
	// current_thread is 0 if there is not
	if (current_thread == 0) {
		// return to main context
#if DEBUG == 1
		perror("all threads completed\n");
#endif
		//turn off timer


		sighold(SIGALRM);
		it.it_interval.tv_usec = 0;
		it.it_value.tv_usec = 0;
		return;
	} else {
		// else run next thread on queue
		threads[current_thread]->state = RUNNING;
#if DEBUG == 1
		sprintf(print, "scheduling thead %d\n", current_thread);
		perror(print);
#endif
		sigrelse(SIGALRM);
		setcontext(threads[current_thread]->context);
	}
}

/**
 * Modified version of the swap.c example provided on http://cgi.cs.mcgill.ca/~xcai9/2012_comp_310.html
 *
 *Timer interrupt handler.
 *Creates a new context to run the scheduler in, masks signals, then swaps contexts saving the previously executing thread and jumping to the scheduler.
 */
void timer_interrupt(int j, siginfo_t *si, void *old_context) {
	//printf("timer\n");
	/* Create new scheduler context */
	getcontext(&signal_context);
	signal_context.uc_stack.ss_sp = signal_stack;
	signal_context.uc_stack.ss_size = 4096;
	signal_context.uc_stack.ss_flags = 0;
	signal_context.uc_link = &main_context; // returns to main context
	sigemptyset(&signal_context.uc_sigmask);
	makecontext(&signal_context, scheduler, 0);
	/* save running thread, jump to scheduler */
	//printf("time done\n");
	swapcontext(threads[current_thread]->context, &signal_context);
}

/**
 * Modified version of the swap.c example provided on http://cgi.cs.mcgill.ca/~xcai9/2012_comp_310.html
 *
 * helper function to create a context.
 * initialize the context from the current context, setup the new stack, signal mask, and tell it which function to call.
 */
int mkcontext(ucontext_t *uc, void(*threadfunc)(), int stacksize) {
	void * stack;

	getcontext(uc);

	stack = malloc(sizeof(char) * stacksize);
	if (stack == NULL) {
		fprintf(stderr, "stack malloc failed\n");
		return -1;
	}
	/* we need to initialize the ucontext structure, give it a stack,
	 flags, and a sigmask */
	uc->uc_stack.ss_sp = stack;
	uc->uc_stack.ss_size = stacksize;
	uc->uc_stack.ss_flags = 0;
	uc->uc_link = &main_context; // returns to main context
	sigemptyset(&uc->uc_sigmask);

	/* setup the function we're going to, and n-1 arguments. */
	makecontext(uc, threadfunc, 0);

	printf("context is %lx\n", (unsigned long) uc);

	return 0;
}

/**
 * Set up SIGALRM signal handler
 */
void setup_signals(void) {
	struct sigaction act;

	act.sa_sigaction = timer_interrupt;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART | SA_SIGINFO;

	if (sigaction(SIGALRM, &act, NULL) != 0) {
		perror("Signal handler");
	}
}
