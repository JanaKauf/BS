/*********************************************************
 * @file 	my_mutex.h
 * @author 	Vadim Budagov, Janaina Kaufmann
 * @Version 1.0
 * Created on: 01.05.2018
 * @brief	Funktionen für Semaphore und/oder Conditional Variable
 *********************************************************/

#ifndef _MY_MUTEX_H
#define _MY_MUTEX_H
#include <pthread.h>

#define RED     "\x1B[31m"
#define GRN     "\x1B[32m"
#define YEL     "\x1B[33m"
#define BLU     "\x1B[34m"
#define MAG     "\x1B[35m"
#define CYN     "\x1B[36m"
#define RESET   "\x1B[0m"


extern pthread_mutex_t * get_mutex();
/**
 * @brief	Doese Funktion initialisiert Semaphore bzw. Conditional Variable.
 * 			Außerden werden die Queue und der Mutex,
 * 			mit denen gearbeitet wird, initialisiert.
 */
extern void my_init (void);
/**
 * @brief 	Schuetzt das schreiben in FIFO mit Mutex und
 * 			CanselStates.
 *
 * @param	characrer Die zu producierende Buchstabe.
 */
extern void my_producer (char character);
/**
 * @brief	Schueztz das Lesen aus dem FIFO mit Mutex und
 * 			CancelStates.
 */
extern void my_consumer ();
/**
 *	@brief	Hier werden Qieue, Semaphode bzw CV und Mutex zestoert, also
 *			es wird "aufgerauemt".
 */
extern void my_clean (void);

#endif /* _MYMUTEX_H */
