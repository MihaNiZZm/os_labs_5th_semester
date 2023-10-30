#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef short mythread_id;
typedef struct {
    mythread_id id;
    void* (*start_routine)(void*);
    void* arg;
    void* retval;
    volatile int joined;
    volatile int exited;
} mythread_t;

int child_function(void *arg);
int mythread_create(mythread_t* thread, void* (*start_routine)(void*), void* arg);
void mythread_join(mythread_t* thread, void** retval);
void* create_stack(long stack_size, mythread_id id);
