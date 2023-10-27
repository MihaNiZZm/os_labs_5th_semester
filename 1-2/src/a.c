#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* new_thread(void* arg) {
    sleep(5);
    printf("New thread: [%d]\n", gettid());
    return NULL;
}

int main() {
    pthread_t thread1;
    int err;

    err = pthread_create(&thread1, NULL, new_thread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    pthread_join(thread1, NULL);

    printf("Main thread: [%d]\n", gettid());
}