#ifndef __FITOS_QUEUE_VAR_H__
#define __FITOS_QUEUE_VAR_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

typedef struct qnode {
    int val;
    struct qnode *next;
} qnode_t;

typedef struct {
    qnode_t *first;
    qnode_t *last;
    int max_count;
    int count;

    // Mutex and condition variable
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    // Monitor thread information
    pthread_t qmonitor_tid;

    // Statistics
    long add_attempts;
    long get_attempts;
    long add_count;
    long get_count;
} queue_t;

void queue_cond_init(queue_t *q);
queue_t* queue_init(int max_count);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int val);
int queue_get(queue_t *q, int *val);
void queue_print_stats(queue_t *q);

#endif		// __FITOS_QUEUE_VAR_H__
