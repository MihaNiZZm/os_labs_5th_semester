#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void* thread1(void* arg) {
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_SETMASK, &mask, NULL);
    
    printf("Thread 1: Blocking all signals\n");
    
    while (1) {
        // Infinite loop to keep the thread running
    }
    
    return NULL;
}

void sigint_handler(int sig) {
    printf("Received SIGINT in Thread 2\n");
}

void* thread2(void* arg) {
    // Signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, &sigint_handler);
    
    while (1) {
        // Infinite loop to keep the thread running
    }
    
    return NULL;
}

void* thread3(void* arg) {
    sigset_t mask;
    int sig;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    
    printf("Thread 3 is waiting for SIGQUIT using sigwait()\n");
    
    if (sigwait(&mask, &sig) == 0) {
        printf("Thread 3 received SIGQUIT\n");
    }
    
    return NULL;
}

int main() {
    pthread_t t1, t2, t3;
    int err;
    
    // Create threads
    err = pthread_create(&t1, NULL, thread1, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(&t2, NULL, thread2, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(&t3, NULL, thread3, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    
    // Wait for threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    
    return 0;
}
