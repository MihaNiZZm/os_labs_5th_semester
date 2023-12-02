#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

typedef void* (*start_routine_t)(void*);
typedef struct _mythread {
    int mythread_id;
    start_routine_t routine;
    void* arg;
    void* retval;
    volatile int joined;
    volatile int finished;
} mythread_struct_t;
typedef mythread_struct_t* mythread_t;

int thread_startup(void* arg);
int mythread_create(mythread_t* mytid, void* (*start_routine)(void*), void* arg);
void mythread_join(mythread_t mytid, void** retval);
void* create_stack(long stack_size, int id);
