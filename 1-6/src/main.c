#include "my_thread_create.h"


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