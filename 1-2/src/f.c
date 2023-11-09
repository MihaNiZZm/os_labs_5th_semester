#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* new_thread(void* arg) {
    printf("New thread: [%d]\n", gettid());
    return NULL;
}

int main() {
    int err;
    pthread_t thread1;
    pthread_attr_t attr;
    err = pthread_attr_init(&attr);
    if (err) {
        printf("pthread_attr_init failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err) {
        printf("pthread_attr_setdetachstate failed: %s\n", strerror(err));
        return -1;
    }

    while (1) {
        err = pthread_create(&thread1, &attr, new_thread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    pthread_attr_destroy(&attr);
}