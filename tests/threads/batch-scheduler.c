/* Tests cetegorical mutual exclusion with different numbers of threads.
 * Automatic checks only catch severe problems like crashes.
 */
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "devices/batch-scheduler.c"



void test_batch_scheduler(void)
{
    init_bus();
    batchScheduler(0, 0, 0, 0);
    printf("efter test 1\n");
    
    batchScheduler(1, 0, 0, 0);
    printf("efter test 2\n");
    
    batchScheduler(0, 0, 0, 1);
    printf("efter test 3\n");
    
    batchScheduler(0, 4, 0, 0);
    printf("efter text 4\n");
    
    batchScheduler(0, 0, 4, 0);
    printf("efter test 5\n");
    
    batchScheduler(3, 3, 3, 3);
    printf("efter test 6\n");
    
    batchScheduler(4, 3, 4 ,3);
    printf("efter test 7\n");
    
    batchScheduler(7, 23, 17, 1);
    printf("efter test 8\n");
    
    batchScheduler(40, 30, 0, 0);
    printf("efter test 9\n");
    
    batchScheduler(30, 40, 0, 0);
    printf("efter test 10\n");
    
    batchScheduler(23, 23, 1, 11);
    printf("efter test 11\n");
    
    batchScheduler(22, 22, 10, 10);
    printf("efter test 12\n");

    batchScheduler(0, 0, 11, 12);
    printf("efter test 13\n");
    
    batchScheduler(0, 10, 0, 10);
    printf("efter test 14\n");
    
    batchScheduler(0, 10, 10, 0);
    printf("efter test 15\n");

    
    pass();
    printf("DÖN\n");
}
