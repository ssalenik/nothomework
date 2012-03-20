/*
 * =====================================================================================
 *
 *       Filename:  sillythreads.h
 *
 *    Description:  Real SillyThreads Library Header File.
 *
 *        Version:  1.0
 *        Created:  03/18/2012 6:42:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Stepan Salenikovich, stepan.salenikovich@gmail.com
 *   Organization:  not organized
 *
 * =====================================================================================
 */
#ifndef _SILLYTHREADS
#define _SILLYTHREADS

#define THREADS_MAX 20		// defines the maximum number of threads allowed
#define SEMAPHORES_NUM	20	// defines space allocated for semaphores initially
#define QUANTUM_DEFAULT 1000	// sets default quantum size in us; this is also the minimum allowed

/* includes */
#include <slack/std.h>
#include <slack/list.h>
#include <ucontext.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

/* data structures */
typedef struct {
	int index;							// index of semaphore used to address it
	int init_value;						// initial value of semaphore
	int value;							// current value of semaphore
	List *thread_queue;					// queue of threads waiting on semaphore
	list_release_t *queue_destructor;	// pointer to destroy queue

}semaphore_t;

typedef enum { RUNNING, RUNNABLE, WAIT, EXIT }state_t;
static char *state_str[4] = {"RUNNING", "RUNNABLE", "WAIT", "EXIT"};	// to make printing easier


typedef struct {
	ucontext_t *context;				// context
	char* thread_name;				// thread name
	int thread_id;					// thread id; same as its id in the thread array
	state_t state;					// current state of thread
	void (*threadfunc)();			// pointer to the function which will run in this thread
	int stacksize;					// size of stack for this thread
	int start_time;					// time at which current RUNNING state started; used to calculate runtime
	int run_time;					// time spent in the current RUNNING state, 0 if not running
	int total_time;					// total time spent running on CPU
}thread_control_block_t;

/* function prototypes */
int init_my_threads();
int create_my_thread(char *threadname, void (*threadfunc)(), int stacksize);
void exit_my_thread();
void runthreads();
void set_quantum_size(int quantum);
int create_semaphore(int value);
void semaphore_wait(int semaphore);
void semaphore_signal(int semaphore);
void destroy_semaphore(int semaphore);
void my_threads_state();

/* helper functions - not to be called by processes using the library */
int mkcontext(ucontext_t *uc, void (*threadfunc)(), int stacksize);
void setup_signals(void);
void timer_interrupt(int j, siginfo_t *si, void *old_context);
void scheduler();

#endif
