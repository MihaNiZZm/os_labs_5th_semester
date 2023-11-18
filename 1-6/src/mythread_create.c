#include "mythread_create.h"

#define PAGE_SIZE 4096
#define STACK_SIZE 100 * PAGE_SIZE

void* create_stack(long stack_size, int id) {
    char stack_file[128];
    int stack_fd;
    void* stack;
    
    snprintf(stack_file, sizeof(stack_file), "stack-%d", id);
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
        return NULL;
    }

    return stack;
}

int thread_startup(void* arg) {
    mythread_t tid = (mythread_t) arg;
    mythread_struct_t* thread = tid;
    void* retval;

    printf("Thread startup: starting a thread function for the thread #%d\n", thread->mythread_id);
    retval = thread->routine(thread->arg);

    thread->retval = retval;
    thread->finished = 1;

    printf("Thread startup: waiting for join() the thread #%d\n", thread->mythread_id);
    while (!thread->joined) {
        sleep(1);
    }

    printf("Thread startup: the thread function finished for the thread #%d\n", thread->mythread_id);

    return 0;
}

int mythread_create(mythread_t* mytid, void* (*start_routine)(void*), void* arg) {
    static int mythread_id = 0;
    mythread_struct_t* thread;
    int child_pid;
    void* child_stack;

    ++mythread_id;

    printf("Mythread create: creating thread #%d\n", mythread_id);

    child_stack = create_stack(STACK_SIZE, mythread_id);
    mprotect(child_stack + PAGE_SIZE, STACK_SIZE - PAGE_SIZE, PROT_READ | PROT_WRITE);
    memset(child_stack + PAGE_SIZE, 0x7f, STACK_SIZE - PAGE_SIZE);

    thread = (mythread_struct_t*) (child_stack + STACK_SIZE - sizeof(mythread_struct_t));
    thread->mythread_id = mythread_id;
    thread->arg = arg;
    thread->routine = start_routine;
    thread->retval = NULL;
    thread->finished = 0;
    thread->joined = 0;

    child_stack = (void*) thread;

    printf("Child stack: %p; Mythread_struct: %p;\n", child_stack, thread);

    child_pid = clone(
        thread_startup,
        child_stack,
        CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
        thread
    );
    if (child_pid == -1) {
        printf("Clone failed\n");
        return -1;
    }

    *mytid = thread;

    return 0;
}

void mythread_join(mythread_t mytid, void** retval) {
    mythread_struct_t* thread = mytid;

    printf("Thread join: waiting for the thread #%d to finish.\n", thread->mythread_id);

    while (!thread->finished) {
        sleep(1);
    }

    printf("Thread join: the thread #%d finished.\n", thread->mythread_id);

    *retval = thread->retval;

    thread->joined = 1;
}
