/*
 * InfraredNetworkBridge.cpp
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#include <stdio.h>
#include "InfraredNetworkBridge.h"

// ALLOCATION
InfraredNetworkBridge::InfraredNetworkBridge() {}
InfraredNetworkBridge::~InfraredNetworkBridge() {}

// INITIALIZATION

/*
 * Initializes this bridge.
 * @return OK if there are no initialization errors
 */
Status InfraredNetworkBridge::init() {
	/* Create the infrared receive queue. */
	receiveQueue = OSQCreate((void**)&receiveBuffer, INFRARED_TO_NETWORK_RECEIVE_SIZE);
	if (receiveQueue == NULL) {
		return ERR_INFRARED;
	} else {
		return OK;
	}
}

/*
 * Checks the infrared receive queue for new readings.
 * @note This method is blocking.
 * @return OK when a new reading is received and processed without error
 */
Status InfraredNetworkBridge::update() {
	INT8U status;
	int level = (int)OSQPend(receiveQueue, 0, &status);
	if (status != OS_NO_ERR) {
		return ERR_INFRARED;
	} else {
		return process(level);
	}
}

/*
 * Gets the queue used to accept infrared receive events.
 * @return the infrared receive queue
 */
OS_EVENT *InfraredNetworkBridge::listener() {
	return receiveQueue;
}

/*
 * Processes an infrared receive event.
 * @param level - the level recorded by the infrared receiver
 */
Status InfraredNetworkBridge::process(unsigned int level) {
	if (level > INFRARED_TO_NETWORK_RECEIVE_THRESHHOLD) {
		INFRAREDNETWORKBRIDGE_LOG(printf("InfraredToNetwork [event: infrared receive, level: %u]\n", level));
	} return OK;
}
