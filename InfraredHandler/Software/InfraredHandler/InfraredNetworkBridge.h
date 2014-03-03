/*
 * InfraredToNetwork.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef INFRAREDNETWORKBRIDGE_H_
#define INFRAREDNETWORKBRIDGE_H_

#include "includes.h"

#include "Status.h"

#define INFRARED_TO_NETWORK_RECEIVE_SIZE 4
#define INFRARED_TO_NETWORK_RECEIVE_THRESHHOLD 100

class InfraredNetworkBridge {
public:
	InfraredNetworkBridge();
	virtual ~InfraredNetworkBridge();

	/*
	 * Initializes this bridge.
	 * @return OK if there are no initialization errors
	 */
	Status init();

	/*
	 * Checks the infrared receive queue for new readings.
	 * @return OK when a new reading is received and processed without error
	 */
	Status update();

	/*
	 * Gets the queue used to accept infrared receive events.
	 * @return the infrared receive queue
	 */
	OS_EVENT *listener();

private:
	/* The buffer and queue used to receive infrared readings. */
	int receiveBuffer[INFRARED_TO_NETWORK_RECEIVE_SIZE];
	OS_EVENT *receiveQueue;

	/*
	 * Processes the infrared reading of the given level.
	 * The reading will be forwarded to the network if it is above the
	 * INFRARED_TO_NETWORK_RECEIVE_THRESHHOLD.
	 * @param level the 12-bit infrared input level to process
	 */
	Status process(unsigned int level);
};


#define INFRAREDNETWORKBRIDGE_DEBUG
#if defined(INFRAREDNETWORKBRIDGE_DEBUG) || defined(DEBUG)
#define INFRAREDNETWORKBRIDGE_LOG(info) info
#else
#define INFRAREDNETWORKBRIDGE_LOG(info)
#endif

#endif /* INFRAREDNETWORKBRIDGE_H_ */
