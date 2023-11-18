#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void *mythread(void *arg) {
	printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
	return NULL;
}

int main() {
    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * 5);
    if (threads == NULL) {
        perror("Couldn't allocate memory for threads array");
        return -1;
    }
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for (int i = 0; i < 5; ++i) {
        err = pthread_create(&threads[i], NULL, mythread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    sleep(5);
    free(threads);
    return 0;
}
