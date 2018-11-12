#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "taskqueue.h"
#include "my_error.h"
#include "thpool.h"

struct threadpool *
thpool_create(mqd_t taskqueue) {
    int i;
    struct threadpool * pool;

    if ((pool = malloc(sizeof(struct threadpool))) == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Fail to allocate -pool-");
        goto err;
    }

    if ((pool->threads = malloc(sizeof(pthread_t) * NUM_THREADS)) == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Fail to allocate -threads-");
        goto err;
    }

    pool->taskqueue = taskqueue;

    errorhandler(pthread_mutex_init(&(pool->mutex), NULL),
                "Fail to initialize -mutex-");
    if (errno) {
        goto err;
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&(pool->threads[i]), NULL,
                           thpool_routine, (void *) pool) != 0) {
            thpool_destroy(pool);
            return NULL;
        }
   }

    return pool;

    err:
        if (pool) {
            thpool_free(pool);
        }
        return NULL;
}

int
thpool_add_task (struct threadpool * pool, struct task job, int prio) {
    if (pool == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Invalid Pool");
        return errno;
    }

    errorhandler (pthread_mutex_lock(&(pool->mutex)),
                  "Fail to lock mutex");
    errorhandler (pthread_mutex_unlock(&(pool->mutex)),
                  "Fail to unlock mutex");
    errorhandler(sendToTaskQueue(pool->taskqueue, job, prio, true),
                 "Fail to send a Task");
    return 0;
}

void *
thpool_routine(void * threadpool) {
    struct threadpool *pool = (struct threadpool *) threadpool;
    struct task job;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    while(true) {

        errorhandler (pthread_mutex_lock(&(pool->mutex)),
                      "Fail to lock mutex");
        errorhandler (pthread_mutex_unlock(&(pool->mutex)),
                      "Fail to unlock mutex");
        job = receiveFromTaskQueue(pool->taskqueue);

        (*(job.routineForTask))(job.arg);

    }

    pthread_exit(0);
    return NULL;
}

int
thpool_destroy(struct threadpool * pool) {
    int i;

    if (pool == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Invalid Pool");
        return errno;
    }

    errorhandler(pthread_mutex_lock(&(pool->mutex)),
                "Fail to lock -mutex-");
    if (errno) {
        return errno;
    }

    errorhandler(pthread_mutex_unlock(&(pool->mutex)),
                "Fail to unlock    -mutex-");
    if (errno) {
        return errno;
    }

    for (i = 0; i < NUM_THREADS; i++) {
        errorhandler(pthread_cancel(pool->threads[i]),
                     "Fail to cancel");
    }

    closeTaskQueue(pool->taskqueue);

    for (i = 0; i < NUM_THREADS; i++) {
        errorhandler(pthread_join(pool->threads[i], NULL),
                     "Fail to join");
        }

    if (!errno) {
        thpool_free(pool);
    }

    return errno;
}

int
thpool_free (struct threadpool * pool) {
    if (pool == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Invalid Pool");
        return errno;
    }

    if (pool->threads) {
        free(pool->threads);
        errorhandler(pthread_mutex_destroy(&(pool->mutex)),
                    "Fail to lock -mutex-");
    }

    free(pool);
    return 0;
}

