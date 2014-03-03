/*
 * Bridge.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#include <stdlib.h>
#include "Bridge.h"

// ALLOCATION
Bridge::Bridge() {}
Bridge::~Bridge() {}

// INITIALIZATION

/*
 * Initializes this bridge.
 * @return OK if there are no initialization errors
 */
Status Bridge::init() {
	/* Create the infrared receive queue. */
	receiveQueue = OSQCreate((void**)&receiveBuffer, BRIDGE_RECEIVE_QUEUE_SIZE);
	if (receiveQueue == NULL) {
		return ERR_BRIDGE_CREATE;
	} else {
		return OK;
	}
}

// LISTENERS

/*
 * Gets the queue used to accept infrared receive events.
 * @return the infrared receive queue
 */
OS_EVENT *Bridge::listener() {
	return receiveQueue;
}
