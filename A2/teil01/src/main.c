/*********************************************************
 * @file 	main.c
 * @author 	Vadim Budagov, Janaina Kaufmann
 * @Version 1.0
 * Created on: 01.05.2018
 * @brief	Der Main arbeiter entweder mit Semaphoren oder
 * 			mit Conditional Variables.
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "my_mutex.h"
#include "my_error.h"

#define ALPHABET_LENGHT 26
#define SLEEP_CONSUMER 2
#define SLEEP_PRODUCER 3

pthread_mutex_t consumer_mutex;
pthread_mutex_t producer_1_mutex;
pthread_mutex_t producer_2_mutex;

pthread_t consumer;
pthread_t producer_1;
pthread_t producer_2;
pthread_t thread_control;

/**
 * @brief 	Dieser Thread erzeugt alle 3 Sekunden ein Zeichen,
 * 			das er in einen QUEUE Puffer schreibt. Hierbei wird
 * 			zwischen Producer_1 und Producer_2 unterschieden.
 * 			Producer_1 schreibt kleine Buchstaben.
 * 			Producer_2 schreibt große Buchstaben.
 * 			Ist der Puffer voll, blockiert er (also Zustand "blocked" )
 * 			bis wieder Speicherplatz im Puffer frei ist.
 * 			Die produzierte Buchstabe, die in Puffer geschrieben wurde,
 * 			wird auf dem Bildschirm ausgegeben.
 *
 * @param	*args Die zu producierende Buchstabe.
 */
void *
producer_thread (void * args) {
    printf("Producer Thread started...\n");
    int i = 0;
    char *character = (char *) args;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    while (true) {
        if (*character == 'a') {
            errorhandler(pthread_mutex_lock(&producer_1_mutex),
                         "Fail to lock -producer_1_mutex-");
            errorhandler(pthread_mutex_unlock(&producer_1_mutex),
                    "Fail to unlock -producer_1_mutex-");
            my_producer(*character + (i % ALPHABET_LENGHT));
        } else {
            errorhandler(pthread_mutex_lock(&producer_2_mutex),
                    "Fail to lock -producer_2_mutex-");
            errorhandler(pthread_mutex_unlock(&producer_2_mutex),
                    "Fail to unlock -producer_2_mutex-");
            my_producer(*character + (i % ALPHABET_LENGHT));
        }
        i++;
        sleep(SLEEP_PRODUCER);
    }

    return NULL;
}
/**
 * @brief	Dieser Thread nimmt alle 2 Sekunden ein Zeichen aus dem
 * 			FIFO Puffer und gibt es auf dem Bildschirm aus. Ist der Puffer
 * 			leer, wartet er im Zustand "blocked", bis eine neues Zeichen
 * 			im Puffer liegt.
 */
void *
consumer_thread() {
    printf("Consumer Thread started...\n");

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    while (true) {
        errorhandler(pthread_mutex_lock(&consumer_mutex),
                "Fail to lock -mutex-");
        errorhandler(pthread_mutex_unlock(&consumer_mutex),
                "Fail to unlock -mutex-");
        my_consumer();
        sleep(SLEEP_CONSUMER);
    }
    return NULL;
}

/**
 * @brief	Dieser Thread steuert das Erzeuger - Verbraucher System
 * 			auf basis von Tastatureingaben wie folgt:
 * 			-1   Start/Stop Producer_1
            -2   Start/Stop Producer_2
            -c/C Start/Stop Consumer
            -q/Q Quit Main Thread
            -h   Help
            - 	 alle anderen Tastatureingaben werden ignoriert.
 */
void *
control_thread () {
    char eingabe = '0';
    bool producer_1_blocked = false;
    bool producer_2_blocked = false;
    bool consumer_blocked = false;

    while (true) {
        eingabe = getchar();
        switch (eingabe) {
            case '1':
                if (producer_1_blocked) {
                    errorhandler(pthread_mutex_unlock(&producer_1_mutex),
                            "Fail to unlock -producer_1_mutex");
                    printf("Producer_1 unblocked...\n");
                    producer_1_blocked = false;
                } else {
                    errorhandler(pthread_mutex_lock(&producer_1_mutex),
                            "Fail to lock -producer_1_mutex");
                    printf("Producer_1 blocked...\n");
                    producer_1_blocked = true;
                }
                break;
            case '2':
                if (producer_2_blocked) {
                    errorhandler(pthread_mutex_unlock(&producer_2_mutex),
                            "Fail to unlock -producer_2_mutex");
                    printf("Producer_2 unblocked...\n");
                    producer_2_blocked = false;
                } else {
                    errorhandler(pthread_mutex_lock(&producer_2_mutex),
                            "Fail to lock -producer_2_mutex");
                    printf("Producer_2 blocked...\n");
                    producer_2_blocked = true;
                }
                break;
            case 'c':
            case 'C':
                if (consumer_blocked) {
                    errorhandler(pthread_mutex_unlock(&consumer_mutex),
                            "Fail to unlcok");
                    printf("Consumer unblocked...\n");
                    consumer_blocked = false;
                } else {
                    pthread_mutex_lock(&consumer_mutex);
                    printf("Consumer blocked...\n");
                    consumer_blocked = true;
                }
                break;
            case 'q':
            case 'Q':
				errorhandler(pthread_cancel(producer_1),
                        "Fail to cancel -producer_1-");
				errorhandler(pthread_cancel(producer_2),
                        "Fail to cancel -producer_2-");
				errorhandler(pthread_cancel(consumer),
                        "Fail to cancel -consumer-");

                if (consumer_blocked) {
                    errorhandler(pthread_mutex_unlock(&consumer_mutex),
                            "Fail to unlock -consumer_mutex");
                }
                if (producer_1_blocked) {
                    errorhandler(pthread_mutex_unlock(&producer_1_mutex),
                            "Fail to unlock -producer_1_mutex");
                }
                if (producer_2_blocked) {
                    errorhandler(pthread_mutex_unlock(&producer_2_mutex),
                            "Fail to unlock -producer_2_mutex");
                }
                printf("For pthread_join\n");
                errorhandler(pthread_join(producer_1, NULL),
                    "Fail to join -producer_1-");
                printf("----------------1\n");
                errorhandler(pthread_join(producer_2, NULL),
                    "Fail to join -producer_1-");
                printf("----------------2\n");
                errorhandler(pthread_join(consumer, NULL),
                    "Fail to join -producer_1-");
                printf("----------------c\n");

                pthread_exit(0);
                break;
            case 'h':
            case 'H':
                printf("-----------HELP----------\
                      \n-1   Start/Stop Producer_1\
                      \n-2   Start/Stop Producer_2\
                      \n-c/C Start/Stop Consumer\
                      \n-q/Q Quit Main Thread\n");
                break;
            default:
                break;
        }
    }
}
/**
 * @brief	Main-Thread.
 * 			Er fuehrt alle notwendigen Initialisierungen durch
 * 			und erzeugt alle weiteren Threads des Systems.
 *
 */
int
main (void) {
    printf("---------------------------MAIN---------------------------\n");

    char small = 'a';
    char big = 'A';

    printf("Initialize Mutexe\n");
    errorhandler(pthread_mutex_init(&consumer_mutex, NULL),
            "Fail to initialize -consumer_mutex");
    errorhandler(pthread_mutex_init(&producer_1_mutex, NULL),
            "Fail to initialize -producer_1_mutex-");
    errorhandler(pthread_mutex_init(&consumer_mutex, NULL),
            "Fail to initialize -consumer_mutex");
    my_init();

    printf("Create Threads\n");
    errorhandler(pthread_create(&producer_1, NULL, producer_thread, &small),
            "Fail to create thread -producer1-");
    errorhandler(pthread_create(&producer_2, NULL, producer_thread, &big),
            "Fail to create thread -producer2-");
    errorhandler(pthread_create(&consumer, NULL, consumer_thread, NULL),
            "Fail to create thread -consumer-");
    errorhandler(pthread_create(&thread_control, NULL, control_thread, NULL),
            "Fail to create -thread_control-");
    printf("Creating of Threads successfully completed!\n");

    printf("Threads were joined!\n");
    errorhandler(pthread_join(thread_control, NULL),
            "Fail to join -thread_control");

    errorhandler(pthread_mutex_destroy(&producer_1_mutex),
            "Fail to destroy -producer_1_mutex");
    errorhandler(pthread_mutex_destroy(&producer_2_mutex),
            "Fail to destroy -producer_2_mutex");
    errorhandler(pthread_mutex_destroy(&consumer_mutex),
            "Fail to destroy -consumer_mutex");
    my_clean();

    printf("------------------------END OF MAIN-THREAD------------------------\n");
    return 0;
}
