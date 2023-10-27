#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    int int_val;
    char* string;
} my_struct;

void* new_thread(void* arg) {
    my_struct* get_item = (my_struct*) arg;
    printf("New thread: [%d]\nStruct info:\nInt num: %d\nString: %s\n", gettid(), get_item -> int_val, get_item -> string);
    return NULL;
}

int main() {
    pthread_t thread1;
    int err;
    my_struct item;
    item.int_val = 42;
    item.string = "!dlrow olleH";

    err = pthread_create(&thread1, NULL, new_thread, (void*) &item);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    pthread_join(thread1, NULL);

    printf("Main thread: [%d]\n", gettid());
}