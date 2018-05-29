/*********************************************************
 * @file:       my_Mutex.c
 * @author:     Vadim Budagov, Janaina Kaufmann
 * @Version:    1.0
 * Created on:  01.05.2018
 * @brief       Protects the FIFO access.
 *********************************************************/
#include <stdio.h>
//#include <semaphore.h>

#include "my_mutex.h"
#include "FIFO.h"
#include "my_error.h"
#include "my_color.h"

#ifdef _SEMAPHORE_H
sem_t semOccupied;
sem_t semFree;

#else
pthread_cond_t cond;
#endif

pthread_mutex_t mutex;

int counter;

static void
cleanup_handler (void * args) {
    printf("Cleaning...\n");

    errorhandler((pthread_mutex_unlock(&mutex)),
                "Fail to unlock mutex");
}

int
my_init () {
    queue_init();

#ifdef _SEMAPHORE_H
    errorhandler(sem_init(&semFree, 0, PUFFER_SIZE),
                "Fail to initiate -semFree-");
    errorhandler(sem_init(&semOccupied, 0, 0),
                "Fail to initiate -occupiedFree-");

#else
    errorhandler(pthread_cond_init(&cond, NULL),
                "Fail to initiate -produce_cond");
#endif

    errorhandler(pthread_mutex_init(&mutex, NULL),
                "Fail to initialize -mutex-");
    return errno;
}

int
my_producer (char character) {

#ifdef _SEMAPHORE_H
    errorhandler(sem_wait(&semFree),
                "Fail to wait -semFree-");
    errorhandler(pthread_mutex_lock(&mutex),
                "Fail to lock -mutex-");

#else
    errorhandler(pthread_mutex_lock(&mutex),
                "Fail to lock -mutex-");
    pthread_cleanup_push(cleanup_handler, NULL);
    while (counter == PUFFER_SIZE) {
        errorhandler(pthread_cond_wait(&cond, &mutex),
                    "Fail to wait -produce_cond-");

    }
    pthread_cleanup_pop(0);
#endif

    queue_put(character);
    counter++;

#ifdef _SEMAPHORE_H
    errorhandler(pthread_mutex_unlock(&mutex),
                "Fail to unlock -mutex-");
    errorhandler(sem_post(&semOccupied),
                "Fail to post -semOccupied-");

#else
    errorhandler(pthread_cond_broadcast(&cond),
                "Fail to signal -cond-");
    errorhandler(pthread_mutex_unlock(&mutex),
                "Fail to unlock -mutex-");
#endif

    printf(YEL "Character produce: %c\
          \nNumber of occupied space on buffer: %d\n\n"
                           RESET , character, counter);
    return errno;

}

int
my_consumer() {
    char character;

#ifdef _SEMAPHORE_H
    errorhandler(sem_wait(&semOccupied),
                "Fail to wait -semOccupied-");
    errorhandler(pthread_mutex_lock(&mutex),
                "Fail to lock -mutex-");

#else
    errorhandler(pthread_mutex_lock(&mutex),
                "Fail to lock -mutex-");
    pthread_cleanup_push(cleanup_handler, NULL);
    while (counter == 0) {
        errorhandler(pthread_cond_wait(&cond, &mutex),
                "Fail to wait -consume_cond-");
    }
    pthread_cleanup_pop(0);
#endif

    queue_get(&character);
    counter--;

#ifdef _SEMAPHORE_H
    errorhandler(pthread_mutex_unlock(&mutex),
                "Fail to unlock -mutex-");
    errorhandler(sem_post(&semFree),
                "Fail to post -semFree-");
#else
    errorhandler(pthread_cond_broadcast(&cond),
                "Fail to signal -cond-");
    errorhandler(pthread_mutex_unlock(&mutex),
                "Fail to unlock -mutex-");
#endif

    printf(GRN"Character consume: %c\
          \nNumber of occupied space on buffer: %d\n\n"
                            RESET, character, counter);

    return errno;

}

int
my_clean () {
    queue_clean();

#ifdef _SEMAPHORE_H
    errorhandler(sem_destroy(&semFree),
                "Fail to destroy -semFree-");
    errorhandler(sem_destroy(&semOccupied),
                "Fail to destroy -occupiedFree-");

#else
    errorhandler(pthread_cond_destroy(&cond),
                "Fail to destroy -consume_cond-");
#endif

    errorhandler(pthread_mutex_destroy(&mutex),
                "Fail to destroy -mutex-");

    return errno;
}
