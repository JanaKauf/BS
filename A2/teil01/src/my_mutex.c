/*********************************************************
 * @file 	my_Mutex.c
 * @author 	Vadim Budagov, Janaina Kaufmann
 * @Version 1.0
 * Created on: 01.05.2018
 * @brief	Implementier und realisiert die Header-Datei my_Mutex.h,
 * 			welche entweder mit SEMAPHORE oder mit CONDITIONAL VARIABLE arbeitet.
 *********************************************************/
#include "my_mutex.h"
#include <stdio.h>
//#include <semaphore.h>
#include <pthread.h>
#include "my_error.h"
#include "FIFO.h"

#ifdef _SEMAPHORE_H
sem_t semOccupied;
sem_t semFree;
#else
pthread_cond_t cond;
#endif

pthread_mutex_t mutex;

int counter;

static void
cleanup_handler(void * args){
    printf("Cleaning...\n");

    errorhandler((pthread_mutex_unlock(&mutex)),
            "Fail to unlock mutex");
}

void
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
}

void
my_producer(char character) {
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
          \nNumber of occupied space on buffer: %d\n\n"RESET , character, counter);

}

void
my_consumer() {
    char character;
    //errorhandler(pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL),
    //        "Fail to disable cancel state");
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
          \nNumber of occupied space on buffer: %d\n\n"RESET, character, counter);

}

void
my_clean(){
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
}
