/*
 * Listener.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#include <stdlib.h>
#include "Listener.h"

// CONSTRUCTION

/**
 * Creates a new listener.
 */
Listener::Listener() {
	/* Create the receive queue. */
	receiveQueue = OSQCreate((void**)&receiveBuffer, LISTENER_RECEIVE_QUEUE_SIZE);
	if (receiveQueue == NULL) {
		throw new QueueCreateException();
	}
}

// LISTENERS

/*
 * Gets the queue used to accept events.
 * @return the receive queue
 */
OS_EVENT *Listener::listener() {
	return receiveQueue;
}

// UPDATES

/**
 * Waits on the receive queue for event.
 * When events are received, they are forwarded to parse().
 * @throw QueuePendException if this listener cannot read the receive queue
 */
void Listener::update() {
	INT8U status;
	char *event = (char *)OSQPend(receiveQueue, 0, &status);
	if (status != OS_NO_ERR) {
		throw new QueuePendException();
	} else {
		parse(event);
	}
}
