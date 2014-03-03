/*
 * Bridge.h
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#ifndef BRIDGE_H_
#define BRIDGE_H_

#include "includes.h"

#include "Status.h"

#define BRIDGE_RECEIVE_QUEUE_SIZE	4

/**
 * The bridge abstract class provides a common structure for classes
 * that listen to data sources and forward relevant events to recipients.
 */
class Bridge {
public:
	Bridge();
	virtual ~Bridge();

	/*
	 * Initializes this bridge.
	 * @return OK if there are no initialization errors
	 */
	Status init();

	/*
	 * Checks the receive queue for new events.
	 * @return OK when a new event is received and processed without error
	 */
	virtual Status update() = 0;

	/*
	 * Gets the queue used to accept events.
	 * @return the receive queue
	 */
	OS_EVENT *listener();

protected:
	/* The buffer and queue used to receive events. */
	int receiveBuffer[BRIDGE_RECEIVE_QUEUE_SIZE];
	OS_EVENT *receiveQueue;
};

#endif /* BRIDGE_H_ */
