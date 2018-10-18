/* Tests cetegorical mutual exclusion with different numbers of threads.
 * Automatic checks only catch severe problems like crashes.
 */
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "lib/random.h" //generate random numbers
#include "devices/timer.h"
#include "threads/interrupt.h"

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


typedef struct {
  struct semaphore *out_high;
  struct semaphore *out;
  struct semaphore *in_high;
  struct semaphore *in;
  int running;
  int direction;
  int next_direction;
  
} bus_t;

void batchScheduler(unsigned int num_tasks_send, unsigned int num_task_receive,
        unsigned int num_priority_send, unsigned int num_priority_receive);

void init_bus(void);
void senderTask(void *);
void receiverTask(void *);
void senderPriorityTask(void *);
void receiverPriorityTask(void *);
struct semaphore* get_sema(task_t *);
void trafficController(task_t *, bool );



void oneTask(task_t task);/*Task requires to use the bus and executes methods below*/
	void getSlot(task_t task); /* task tries to use slot on the bus */
	void transferData(task_t task); /* task processes data on the bus either sending or receiving based on the direction*/
	void leaveSlot(task_t task); /* task release the slot */


bus_t *bus;

/* initializes semaphores */ 
void init_bus(void){
  sema_init(bus->out_high, 0);
  sema_init(bus->out, 0);
  sema_init(bus->in_high, 0);
  sema_init(bus->in, 0);

  bus->running = 0;
  bus->direction = SENDER;
  bus->next_direction = SENDER;
  
  random_init((unsigned int)123456789); 
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
 sema_down(get_sema(&task));
 trafficController(&task, true);

 
}

/* task processes data on the bus send/receive */
void transferData(task_t task)
{
  //Sleepy boy (Does the task/data transfer).
  timer_msleep(1); 
}

/* task releases the slot */
void leaveSlot(task_t task) 
{
  /* Return the semaphor */

  /* If priority, no priority waiting on own side and priority waiting on ther side (check the queue), change direction */

  /* If task and no task waiting from own side (check the queue), change direction */

 trafficController(&task, false);


}

/* get correct semaphore for the given task */
struct semaphore* get_sema(task_t *task)
{
  if (task->direction == SENDER)
    {
      if (task->priority == HIGH)
	{
	  return bus->out_high;
	}
      else
	{
	  return bus->out;
	}
    }
  else
    {
      if (task->priority == HIGH)
	{
	  return bus->in_high;
	}
      else
	{
	  return bus->in;
	}
    }
}

void trafficController(task_t *task, bool starting)
{
  /* Disable interrupts to safely poke at semaphores */
  enum intr_level old_level = intr_disable();

  /* keep track of tasks running */
  if (!starting)
    {
      bus->running--;
    }

  /* if ready to change direction, do so */
  if (bus->running == 0 && bus->direction!=bus->next_direction)
    {
      bus->direction = bus->next_direction;
    }
  
  /* if direction is not currently changing, see if it should */
  /* if direction shouldn't change, add task */    
  if (bus->direction == bus->next_direction)
    {
      if (bus->direction == SENDER)
	{
	  if (!list_empty (&bus->out_high->waiters))
	    {
	      if (bus->running > BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		  bus->running++;
		  sema_up(bus->out_high);
		}
	    }
	  else if (!list_empty (&bus->in_high->waiters))
	    {
	      /* if change if direction is due, start it */
	      bus->next_direction = RECEIVER;
	    }
	  else if (!list_empty (&bus->out->waiters))
	    {
	      if (bus->running > BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		  bus->running++;
		  sema_up(bus->out_high);
		}
	    }
	  else if (!list_empty (&bus->in->waiters))
	    {
	      /* if change if direction is due, start it */
	      bus->next_direction = RECEIVER;
	    }
	}
      else /* Bus is RECEIVER */
	{
	  if (!list_empty (&bus->in_high->waiters))
	    {
	      if (bus->running < BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		  bus->running++;
		  sema_up(bus->out_high);
		}
	    }
	  else if (!list_empty (&bus->out_high->waiters))
	    {
	      /* if change if direction is due, start it */
	      bus->next_direction = RECEIVER;
	    }
	  else if (!list_empty (&bus->in->waiters))
	    {
	      if (bus->running < BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		  bus->running++;
		  sema_up(bus->out_high);
		}
	    }
	  else if (!list_empty (&bus->out->waiters))
	    {
	      /* if change if direction is due, start it */
	      bus->next_direction = RECEIVER;
	    }
	}
    }
  intr_set_level (old_level);
}
  
  
  
/* 
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
