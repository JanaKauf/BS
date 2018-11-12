/*********************************************************
 * @file        FIFO.c
 * @author      Vadim Budagov, Janaina Kaufmann
 * @Version     1.0
 * Created on:  01.05.2018
 * @brief       Queue implemantation.
 *********************************************************
 */
#include <stdio.h>
#include <stdlib.h>

#include "FIFO.h"
#include "my_error.h"
#include "my_color.h"

Queue* dummy = NULL;

int
queue_init() {
    if ((dummy = malloc(sizeof(Queue))) != NULL) {
        dummy->character = '0';
        dummy->previous = NULL;
        return errno;
    } else {
        errno = EADDRNOTAVAIL;
        perror(RED"Fail to initiate Queue!"RESET);
    }
    return errno;
}

int
queue_put(char newCharacter) {
    Queue* newQueue = NULL;
    Queue* curr = dummy;

    if (dummy == NULL) {
        errno = EADDRNOTAVAIL;
        perror(RED"Queue ist not initiatet yet!"RESET);
        return errno;
    }

    if ((newQueue = malloc(sizeof(Queue))) == NULL) {
        errno = EADDRNOTAVAIL;
        perror(RED"Fail to allocate -newQueue"RESET);
        return errno;
    }

    newQueue->character = newCharacter;

    if (dummy->previous == NULL) {
        dummy->previous = newQueue;
        newQueue->previous = NULL;
    } else {
        while(curr->previous != NULL){
            curr = curr->previous;
        }
        curr->previous = newQueue;
        newQueue->previous = NULL;
    }
    return errno;
}

int
queue_get(char *currChar) {
    Queue* curr = dummy;

    if (dummy->previous != NULL) {
        curr = dummy->previous;
        *currChar = curr->character;
        dummy->previous = curr->previous;
        free(curr);
    } else {
        *currChar = 0;
        errno = EACCES;
        perror(RED"Queue ist empty"RESET);
    }

    return errno;
}

int
queue_clean() {
    char d = 0;

    while (dummy->previous != NULL) {
        queue_get(&d);
    }
    free(dummy);
    return errno;
}
