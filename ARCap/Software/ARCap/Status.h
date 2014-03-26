/*
 * Status.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef STATUS_H_
#define STATUS_H_

#include <cstdio>
#include <exception>
#include <string>
using namespace std;

#include "stdbool.h"
#include "stdlib.h"
#include "includes.h"

#include "Debug.h"

#define OK	0

class ARCapException : exception {
public:
	virtual const char *what() const throw() = 0;
};

class QueuePostException : ARCapException {
public:
	virtual const char *what() const throw() {
		return "Failed to post to listener queue.";
	}
};

class UARTOpenException : ARCapException {
public:
	virtual const char *what() const throw() {
		return "Failed to open connection to UART.";
	}
};

class SemCreateException : ARCapException {
public:
	virtual const char *what() const throw() {
		return "Failed to create semaphore.";
	}
};

#endif /* STATUS_H_ */
