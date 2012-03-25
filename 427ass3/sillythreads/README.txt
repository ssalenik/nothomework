/*
 * =====================================================================================
 *
 *       Filename:  README.txt
 *
 *    Description:  SillyThreads Library readme
 *
 *        Version:  1.0
 *        Created:  03/19/2012 25:43:75 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Stepan Salenikovich, stepan.salenikovich@gmail.com
 *   Organization:  not organized
 *
 * =====================================================================================
 */
 
 Everything works!
 
 To compile, simly run "make".  I renamed my library to "sillythreads", so test programs should be modified appropriately (or else you can simply rename the .h and .c files).  However, the API is unchanged from the sem-ex.c originally provided (asside from the helper methods which were added).
 
 Currently the way the timing works, any time a thread is run, it is assumed to run for the full quantum, even if it goes into a wait or ends before it uses all of its time.  In fact, this is actually true, since the thread exit function and the semaphore wait function both idle (loop) until the scheduler is called by the alarm again.  So, though the thread isn't executing its instructions, the current thread context is technically running.
 
 Debug information can be enabled by setting the DEBUG define in sillythreads.c to '1'. It is enabled through compiler if statements.  Notice that the compiler "#if" and "#endif" are not tabbed "properly", as this is the default formating of Eclipse when you invoke autoformat.
 
 sillythreads.h options:
 
THREADS_MAX		- the max number of threads allowed
SEMAPHORES_NUM	- the max number of semaphores allowed
QUANTUM_DEFAULT	- the default quantum time in us (should not be less than 500)

sillythreads.c options:

DEBUG	- set to 1 to print debug information
 