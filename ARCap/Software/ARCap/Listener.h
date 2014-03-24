/*
 * Listener.h
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include "includes.h"
#include "Status.h"

#define LISTENER_RECEIVE_QUEUE_SIZE	4

/**
 * The bridge abstract class provides a common structure for classes
 * that listen to data sources and forward relevant events to recipients.
 */
class Listener {
public:
	/**
	 * Creates a new listener.
	 */
	Listener();

	/*
	 * Gets the queue used to accept events.
	 * @return the receive queue
	 */
	OS_EVENT *listener();

protected:
	/* The buffer and queue used to receive events. */
	int receiveBuffer[LISTENER_RECEIVE_QUEUE_SIZE];
	OS_EVENT *receiveQueue;

};

class QueueCreateException : ARCapException {
public:
	virtual const char *what() const throw() {
		return "Failed to create listener queue.";
	}
};

class QueuePendException : ARCapException {
public:
	virtual const char *what() const throw() {
		return "Failed to pend on listener queue.";
	}
};

#endif /* LISTENER_H_ */
