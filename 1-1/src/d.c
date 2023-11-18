#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int global_var = 42;

void *mythread(void *arg) {
    int local_var = 99;
    static int static_var = 11;
    const int const_var = 101;
	printf("mythread [pid: %d\tppid: %d\ttid: %d\tpthread_self(): %ld]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());
	printf("local var: %p\tstatic var: %p\tconst var: %p\tglobal var: %p\n", &local_var, &static_var, &const_var, &global_var);
    global_var = 10;
    local_var = pthread_self();
    printf("Local var after change: %d\n", local_var);
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
            free(threads);
            return -1;
        }
    }

    for (int i = 0; i < 5; ++i) {
        printf("Child thread #%d pthread_t: %ld\n", i, threads[i]);
    }
    sleep(5);
    free(threads);
    printf("main: global_var: %d\n", global_var);
    return 0;
}
