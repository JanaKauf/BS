/*
 * main.c
 *  Created on: 05.05.2018
 *      Author: Janaina Flor Kaufmann, Vadim Bugadov
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "thpool.h"
#include "my_error.h"
#include "my_mutex.h"
#include "my_color.h"

#include "taskqueue.h"
#include "getcharTimeout.h"

#define CONSUMERTASKS   "/consumertask"
#define PRODUCERTASKS   "/producertask"
#define ALPHABET_LENGHT 26
#define SLEEP_CONSUMER  2
#define SLEEP_PRODUCER  3

pthread_t thread_control;
struct threadpool * consumerPool;
struct threadpool * producerPool;
mqd_t producertasks;
mqd_t consumertasks;
int count = 0;

void
quit_task () {
    printf(MAG"Quit task...\n"RESET);

    thpool_destroy(producerPool);
    destroyTaskQueue(PRODUCERTASKS);
    thpool_destroy(consumerPool);
    destroyTaskQueue(CONSUMERTASKS);

    my_clean();
    printf(MAG"Quit task finished\n"RESET);
}

void
producer_task (void * argument) {
    printf(MAG"Producer Task...\n"RESET);
    char * character = (char *)argument;

    if (*character == 'a') {
        my_producer(*character + (count % ALPHABET_LENGHT));
    } else { 
        my_producer(*character + (count % ALPHABET_LENGHT));
    }
    count++;
}

void
consumer_task() {
    printf(MAG"Consumer Task started...\n"RESET);
    my_consumer();
}

void *
control_thread () {
    char eingabe = '0';
    bool producer = true;
    bool big = true;
    bool consumer = true;


    char argument = 'A';
    struct task job = {NULL, NULL};

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    while (true) {
        if (big) {
            argument = 'A';
        } else {
            argument = 'a';
        }

        if (producer) {
            job.routineForTask = producer_task;
            job.arg = &argument;
            thpool_add_task(producerPool, job, 0);
       }

        if (consumer) {
            job.routineForTask = consumer_task;
            job.arg = NULL;
            thpool_add_task(consumerPool, job, 0);
        }

        eingabe = getcharTimeout(1);
        switch (eingabe) {
            case 'p':
            case 'P':
                if (producer && big) {
                  printf(MAG"Producer produces small characters. \n"RESET);
                  big = false;
                } else if (producer && !big) {
                  printf(MAG"Producer stoped. \n"RESET);
                  producer = false;
                } else if (!producer && !big){
                  printf(MAG"Producer started and produces big characters. \n"RESET);
                  big = true;
                  producer = true;
                }
                break;
            case 'c':
            case 'C':
                if (consumer) {
                  printf(MAG"Consumer stoped.\n"RESET);
                    consumer = false;
                } else { 
                  printf(MAG"Consumer started.\n"RESET);
                    consumer = true;
                }
                break;
            case 'q':
            case 'Q':
                job.routineForTask = quit_task;
                job.arg = NULL;
                (*(job.routineForTask))(job.arg);
                pthread_exit(0);
                break;
            case 'h':
            case 'H':
                printf(MAG"-----------HELP----------\
                      \n-1   Start/Stop Producer_1\
                      \n-2   Start/Stop Producer_2\
                      \n-c/C Start/Stop Consumer\
                      \n-q/Q Quit Main Thread\n"RESET);
                break;
            default:
                break;
        }
    }
}

int
main (void) {
    printf("---------------------------MAIN---------------------------\n");

    printf(MAG"Initializing from Task Queues...\n"RESET);
    producertasks = createTaskQueue(PRODUCERTASKS, NUM_TASKS);
    consumertasks = createTaskQueue(CONSUMERTASKS, NUM_TASKS);
    printf(MAG"Initializing my_mutex...\n"RESET);
    errorhandler (my_init(),
                  "Fail to my_init" );

    printf(MAG"Initializing pools...\n"RESET);
    if ((producerPool = thpool_create(producertasks)) == NULL) {
        perror ("Fail to create producerPool");
    }

    if ((consumerPool = thpool_create(consumertasks)) == NULL) {
        perror ("Fail to create consumerPool");
    }

    printf(MAG"Creating Thread_control...\n"RESET);
    errorhandler (pthread_create(&thread_control, NULL, control_thread, NULL),
                  "Fail to create -thread_control");

    printf(MAG"Join thread_control...\n"RESET);
	errorhandler (pthread_join(thread_control, NULL),
                  "Fail to join -thread_control-");

    return 0;


}
