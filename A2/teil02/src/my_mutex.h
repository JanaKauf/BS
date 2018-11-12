/*********************************************************
 * @file        my_mutex.h
 * @author      Vadim Budagov, Janaina Kaufmann
 * @Version     1.0
 * Created on:  01.05.2018
 * @brief       Funktionen für Semaphore und/oder Conditional Variable
 *********************************************************/

#ifndef _MY_MUTEX_H
#define _MY_MUTEX_H
#include <pthread.h>

/**
 * @brief    Doese Funktion initialisiert Semaphore bzw. Conditional Variable.
 *             Außerden werden die Queue und der Mutex,
 *             mit denen gearbeitet wird, initialisiert.
 */
extern int my_init (void);

/**
 * @brief     Schuetzt das schreiben in FIFO mit Mutex und
 *             CanselStates.
 *
 * @param    characrer Die zu producierende Buchstabe.
 */
extern int my_producer (char character);

/**
 * @brief    Schueztz das Lesen aus dem FIFO mit Mutex und
 *             CancelStates.
 */
extern int my_consumer ();

/**
 *    @brief    Hier werden Qieue, Semaphode bzw CV und Mutex zestoert, also
 *            es wird "aufgerauemt".
 */
extern int my_clean (void);

#endif /* _MYMUTEX_H */
