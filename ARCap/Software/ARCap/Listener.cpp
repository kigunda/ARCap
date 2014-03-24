/*
 * Listener.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#include <stdlib.h>
#include "Listener.h"

// ALLOCATION

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
