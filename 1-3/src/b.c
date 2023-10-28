#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
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
    free(get_item);
    return NULL;
}

int main() {
    pthread_t thread1;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int err;
    my_struct* item = malloc(sizeof(my_struct));
    item->int_val = 42;
    item->string = "!dlrow olleH";

    err = pthread_create(&thread1, &attr, new_thread, (void*) item);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        free(item);
        return -1;
    }

    sleep(2);

    printf("Main thread: [%d]\n", gettid());
}