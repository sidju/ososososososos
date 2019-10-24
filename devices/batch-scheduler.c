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


static bus_t *bus;
static bus_t allo_bus;
static struct semaphore a;
static struct semaphore b;
static struct semaphore c;
static struct semaphore d;

/* initializes semaphores */ 
void init_bus(void){
  struct semaphore* temp;
   /* printf("init_bus ENTERED\n"); */

  bus = &allo_bus;
  
  bus->out_high = &a;
  bus->out = &b;
  bus->in_high = &c;
  bus->in = &d;

  sema_init(bus->out_high, 0);
  sema_init(bus->out, 0);
  sema_init(bus->in_high, 0);
  sema_init(bus->in, 0);

  bus->running = 0;
  bus->direction = SENDER;
  bus->next_direction = SENDER;
  random_init((unsigned int)123456789);
 /* printf("init_bus DONE\n"); */
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
 /* printf("scheduling starting\n"); */
 unsigned int i;
 /*  printf("Start scheduling\n"); */
  for(i = 0; i < num_tasks_send; i++)
    {
      thread_create ("", 0, senderTask, NULL);
    }
  for(i = 0; i < num_task_receive; i++)
    {
      thread_create ("", 0, receiverTask, NULL);
    }
  for(i = 0; i < num_priority_send; i++)
    {
      thread_create ("", 1, senderPriorityTask, NULL);
    }
  for(i = 0; i < num_priority_receive; i++)
    {
      thread_create ("", 1, receiverPriorityTask, NULL);
    }
  /* printf("Scheduling DONE\n"); */
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

   trafficController(&task, true);
   sema_down(get_sema(&task));
}

/* task processes data on the bus send/receive */
void transferData(task_t task)
{
  /*Sleepy boy (Does the task/data transfer).
  timer_usleep(1);*/
  /* printf("Doing stuffs\n"); */
}

/* task releases the slot */
void leaveSlot(task_t task) 
{
  /* Return the semaphor through traffic controller */
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
      /* printf("One freed, now %d running", bus->running); */
    }

  /* If currently changing direction, don't check for changing direction */
  if (bus->direction == bus->next_direction)
    {  
      /* See if direction should be changed based on calling task and semaphore waiters */
      if (bus->direction == SENDER)
	{
	  /*if no high priority using current direction change is possible*/
	  /*No high using*/
	  if(list_empty (&bus->out_high->waiters) &&
	     !(task->priority == HIGH && task->direction == SENDER && starting))
	    {
	      /*The if from hell*/
	      if(
		 /*if high in other direction exist, change direction*/
		 /*High wanting*/
		 (!list_empty (&bus->in_high->waiters) ||
		  (task->priority == HIGH && task->direction == RECEIVER && starting)
		  ) ||
		 /*if normal waiting and no normal using, change direction*/
		 (/*normal waiting*/
		  (!list_empty (&bus->in->waiters) ||
		   (task->priority == NORMAL && task->direction == RECEIVER && starting)
		   ) &&
		  /*No normal using*/
		  (list_empty (&bus->out->waiters) &&
		   !(task->priority == NORMAL && task->direction == SENDER && starting))
		  )
		 )
		{
	      
		  /* printf("Changing direction\n"); */
		  bus->next_direction = RECEIVER;
		}
	    }
	}
      else /* Is RECIEVER */
	{
	  /*if no high priority using current direction change is possible*/
	  /*No high using*/
	  if(list_empty (&bus->in_high->waiters) &&
	     !(task->priority == HIGH && task->direction == RECEIVER && starting))
	    {
	      /*The if from hell in the other way*/
	      if(
		 /*if high in other direction exist, change direction*/
		 /*High wanting*/
		 (!list_empty (&bus->out_high->waiters) ||
		  (task->priority == HIGH && task->direction == SENDER && starting)
		  ) ||
		 /*if normal waiting and no normal using, change direction*/
		 (/*normal waiting*/
		  (!list_empty (&bus->out->waiters) ||
		   (task->priority == NORMAL && task->direction == SENDER && starting)
		   ) &&
		  /*No normal using*/
		  (list_empty (&bus->in->waiters) &&
		   !(task->priority == NORMAL && task->direction == RECEIVER && starting))
		  )
		 )
		{
		 /* printf("Chaning back direction\n"); */
		  bus->next_direction = SENDER;
		}
	    }
	}
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
      
      /* printf("Looking to do stuff\n"); */

      if (bus->direction == SENDER)
	{
	  if (!list_empty (&bus->out_high->waiters) ||
	      (task->priority == HIGH && task->direction == SENDER))
	    {
	      if (bus->running < BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		 /* printf("Starting task\n"); */
		  bus->running++;
		  sema_up(bus->out_high);
		}
	    /*  printf("Bus capacity %d is smaller than nr running %d", BUS_CAPACITY, bus->running); */
	    }
	  else if (!list_empty (&bus->out->waiters) ||
	      (task->priority == NORMAL && task->direction == SENDER))
	    {
	      if (bus->running < BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		 /*  printf("Starting task\n"); */
		  bus->running++;
		  sema_up(bus->out);
		}
	    /*  printf("Bus capacity %d is smaller than nr running %d", BUS_CAPACITY, bus->running); */
	    }
	}
      else /* Bus is RECEIVER */
	{
	  if (!list_empty (&bus->in_high->waiters) ||
	      (task->priority == HIGH && task->direction == RECEIVER))
	    {
	      if (bus->running < BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		 /* printf("Starting task\n"); */
		  bus->running++;
		  sema_up(bus->in_high);
		}
	      /* printf("Bus capacity %d is smaller than nr running %d", BUS_CAPACITY, bus->running); */
	    }
	  else if (!list_empty (&bus->in->waiters) ||
	      (task->priority == NORMAL && task->direction == RECEIVER))
	    {
	      if (bus->running < BUS_CAPACITY)
		{
		  /* if there is space, start most relevant service */
		  
		 /* printf("Starting task\n"); */
		  bus->running++;
		  sema_up(bus->in);
		}
	    /*  printf("Bus capacity %d is smaller than nr running %d", BUS_CAPACITY, bus->running); */
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
   Semaphores held by controller is bus queues
   int holds current users on the bus
   Direction shown by current_direction
   When changing direction, set next_direction 
*/
