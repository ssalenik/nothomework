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
#include <slack/std.h>
#include <slack/list.h>
#include <ucontext.h>


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
