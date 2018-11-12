/*********************************************************
 * @file        FIFO.h
 * @author      Vadim Budagov, Janaina Kaufmann
 * @Version     1.0
 * Created on:  01.05.2018
 * @brief       Diese Header-Datei beschreibt einen beschraenkten Puffer
 *              mit der festen Groesse von 10 Elem. Diese wird im FIFO.c
 *              implementiert. C Modul wird sowohl für Semaphoren als auch
 *              für Conditional Variablen angewandt.
 *********************************************************
 */
#ifndef _FIFO_H
#define _FIFO_H

//Maximale Puffergroesse
#define PUFFER_SIZE 10

/**
 * @brief    Datensttruktur zur Beschreibung eines FIFO's
 *             Linked List mit vorherigem Element
 *             Jede neue Buchtabe wird im Struktur hinten rangehaengt.
 *            buchstabe -> eine Buchstabe im Struktur
 *            *previous -> Zeiger auf vorheriges Element
 */
typedef struct queue{
    char character;
    struct queue *previous;
} Queue;

/**
 * @brief    Diese Funktion initialisiert FIFO
 *             und allociert den Speicher für dessen Struktur.
 */
extern int queue_init(void);

/**
 * @brief    Diese Funktion schreibt einzelne Buchstaben in FIFO
 *             Das Schreiben soll mit Mutexe geschuezt werden.
 *
 *     @param newCaracter Die zu schreibende/abzuspeichernde Buchstabe.
 */
extern int queue_put(char newCharacter);

/**
 *    @brief    Diese Funktion liesst einzelne Buchstaben aus dem FIFO
 *
 *    @param    *currChar Zeiger auf eine Buchstabe im Struktur,
 *                      die augelesen werden soll.
 */
extern int queue_get(char *currChar);

/**
 *    @brief     Diese Funktion dient zum Aufraumen von Struktur.
 *            Der allozierter Speicher wird wieder freigegeben.
 */
extern int queue_clean(void);

#endif /* _FIFO_H */
