#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* new_thread(void* arg) {
    while (1) {
        printf("I'm a new thread!!!\n");
    }
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
    sleep(5);
    err = pthread_cancel(thread1);
    if (err) {
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(thread1, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    printf("Main thread: [%d]\n", gettid());
}