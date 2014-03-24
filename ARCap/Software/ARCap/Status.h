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
using namespace std;

#include "stdlib.h"
#include "includes.h"

#include "Debug.h"

typedef enum Status {
	OK = 0,
	ERR_INFRARED,
	ERR_WIFI,
	ERR_NETWORK_COMMAND_NOT_ACCEPTED,
	ERR_BRIDGE_CREATE,
	ERR_BRIDGE_READ,
	ERR_BRIDGE_MESSAGE_NOT_ACCEPTED
} Status;

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

#endif /* STATUS_H_ */
