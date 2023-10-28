#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>

#define STACK_SIZE (1024 * 1024)

typedef short mythread_id;
typedef struct {
    mythread_id id;
    void* (*start_routine)(void*);
    void* arg;
    void* retval;
    volatile int joined;
    volatile int exited;
} mythread_t;

int child_function(void *arg) {
    mythread_t *args = (mythread_t*) arg;
    void* (*start_routine)(void*) = args->start_routine;
    void *start_routine_arg = args->arg;

    args->retval = start_routine(start_routine_arg);
    args->exited = 1;

    while(!args->joined) {
        sleep(1);
    }

    return 0;
}

int mythread_create(mythread_t* thread, void* (*start_routine)(void*), void* arg) {
    static mythread_id id = 0;

    void* stack = (void*) malloc(STACK_SIZE);
    if (!stack) {
        perror("Couldn't allocate memory for a stack of a new thread");
        return -1;
    }

    thread = malloc(sizeof(mythread_t));
    if (!thread) {  
        perror("Couldn't allocate memory for new thread arguments");
        free(stack);
        return -1;
    }

    thread->id = id++;
    thread->start_routine = start_routine;
    thread->arg = arg;
    thread->exited = 0;
    thread->joined = 0;
    thread->retval = NULL;

    int child_pid = clone(
        child_function,
        stack + STACK_SIZE,
        CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
        thread);
    if (child_pid == -1) {
        perror("Couldn't create a thread");
        free(thread);
        free(stack);
        return -1;
    }

    return 0;
}

void mythread_join(mythread_t* thread, void** retval) {
    while (!thread->exited) {
        sleep(1);
    }
    *retval = thread->retval;
    thread->joined = 1;
}

// Example usage
void* my_thread_function(void *arg) {
    printf("Hello from my_thread_function\n");
    return NULL;
}

int main() {
    mythread_t new_thread;
    void* retval;
    if (mythread_create(&new_thread, my_thread_function, NULL) != 0) {
        perror("Couldn't create custom thread.");
    }
    mythread_join(&new_thread, &retval);

    return 0;
}
