#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* new_thread(void* arg) {
    sleep(5);
    printf("New thread: [%d]\n", gettid());
    char** res = malloc(sizeof(char) * 32);
    *res = "Hello world!";
    return (void*) res;
}

int main() {
    pthread_t thread1;
    int err;

    err = pthread_create(&thread1, NULL, new_thread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    char** res;
    err = pthread_join(thread1, (void**) &res);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }
    
    printf("Main thread: [%d]. Got message \"%s\" from new thread.\n", gettid(), *res);
    free(res);
}