#include "my_thread_create.h"

#define PAGE_SIZE 4096
#define STACK_SIZE 100 * PAGE_SIZE

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

void *create_stack(long stack_size, mythread_id id) {
    char* stack_file = malloc(sizeof(char) * 16);
    int stack_fd;
    void* stack;
    
    snprintf(stack_file, 15, "stack-%d", id);
    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    if (stack_fd == -1) {
        perror("Couldn't create a file for a child's thread stack");
        return NULL;
    }
    ftruncate(stack_fd, 0);
    ftruncate(stack_fd, stack_size);

    stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, stack_fd, 0);
    if (close(stack_fd) < 0) {
        perror("Couldn't close stack file descriptor");
        munmap(stack, stack_size);
    }

    memset(stack, 0, stack_size);

    return stack;
}

int mythread_create(mythread_t* thread, void* (*start_routine)(void*), void* arg) {
    static mythread_id id = 0;

    void* stack = create_stack(STACK_SIZE, id);

    thread = (mythread_t*) malloc(sizeof(mythread_t));
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
