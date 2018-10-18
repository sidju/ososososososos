/* Tests cetegorical mutual exclusion with different numbers of threads.
 * Automatic checks only catch severe problems like crashes.
 */
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "lib/random.h" //generate random numbers

#define BUS_CAPACITY 3
#define SENDER 0
#define RECEIVER 1
#define NORMAL 0
#define HIGH 1

/*
 *	initialize task with direction and priority
 *	call o
 * */
typedef struct {
	int direction;
	int priority;
} task_t;

void batchScheduler(unsigned int num_tasks_send, unsigned int num_task_receive,
        unsigned int num_priority_send, unsigned int num_priority_receive);

void senderTask(void *);
void receiverTask(void *);
void senderPriorityTask(void *);
void receiverPriorityTask(void *);


void oneTask(task_t task);/*Task requires to use the bus and executes methods below*/
	void getSlot(task_t task); /* task tries to use slot on the bus */
	void transferData(task_t task); /* task processes data on the bus either sending or receiving based on the direction*/
	void leaveSlot(task_t task); /* task release the slot */



/* initializes semaphores */ 
void init_bus(void){ 
 
    random_init((unsigned int)123456789); 
    
    msg("NOT IMPLEMENTED");
    /* FIXME implement */

}

/*
 *  Creates a memory bus sub-system  with num_tasks_send + num_priority_send
 *  sending data to the accelerator and num_task_receive + num_priority_receive tasks
 *  reading data/results from the accelerator.
 *
 *  Every task is represented by its own thread. 
 *  Task requires and gets slot on bus system (1)
 *  process data and the bus (2)
 *  Leave the bus (3).
 */

void batchScheduler(unsigned int num_tasks_send, unsigned int num_task_receive,
        unsigned int num_priority_send, unsigned int num_priority_receive)
{
    msg("NOT IMPLEMENTED");
    /* FIXME implement */
}

/* Normal task,  sending data to the accelerator */
void senderTask(void *aux UNUSED){
        task_t task = {SENDER, NORMAL};
        oneTask(task);
}

/* High priority task, sending data to the accelerator */
void senderPriorityTask(void *aux UNUSED){
        task_t task = {SENDER, HIGH};
        oneTask(task);
}

/* Normal task, reading data from the accelerator */
void receiverTask(void *aux UNUSED){
        task_t task = {RECEIVER, NORMAL};
        oneTask(task);
}

/* High priority task, reading data from the accelerator */
void receiverPriorityTask(void *aux UNUSED){
        task_t task = {RECEIVER, HIGH};
        oneTask(task);
}

/* abstract task execution*/
void oneTask(task_t task) {
  getSlot(task);
  transferData(task);
  leaveSlot(task);
}


/* task tries to get slot on the bus subsystem */
void getSlot(task_t task) 
{
  /* Aquire the communication semaphor fo current thread */

  /* If priority and no priority waiting on other side (check the queue), change direction */

  /* If task and no task waiting on other side (check the queue), change direction */
  
  msg("NOT IMPLEMENTED");
  /* FIXME implement */
}

/* task processes data on the bus send/receive */
void transferData(task_t task)
{
  /* "transfer data" aka. sleep */
  msg("NOT IMPLEMENTED");
  /* FIXME implement */
}

/* task releases the slot */
void leaveSlot(task_t task) 
{
  /* Return the semaphor */

  /* If priority, no priority waiting on own side and priority waiting on ther side (check the queue), change direction */

  /* If task and no task waiting from own side (check the queue), change direction */
  

    msg("NOT IMPLEMENTED");
    /* FIXME implement */
}

/* 
   Global variables:
   out_high
   out
   in_high
   in


   batchScheduler:
   lots of loops declaring threads


   getSlot:
   add self to correct semaphor queue (always full)
   run trafficController(STARTING) to get semaphor if possible

   leaveSlot:
   run trafficController(STOPPING) to give semaphor to it


   Static variables in trafficController:
   Semaphores held by controller is bus
   Direction shown by current_direction
   When changing direction, set next_direction 

*/
