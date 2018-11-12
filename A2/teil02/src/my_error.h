/*********************************************************
 * @file 	my_error.h
 * @author 	Vadim Budagov, Janaina Kaufmann
 * @Version 1.0
 * Created on: 01.05.2018
 * @brief	Funktionen für Semaphore und/oder Conditional Variable
 *********************************************************/


#ifndef _MY_ERROR_H
#define _MY_ERROR_H
#include <errno.h>

#define errorhandler(code, message) if((errno = code) != 0)perror(message)

#endif
