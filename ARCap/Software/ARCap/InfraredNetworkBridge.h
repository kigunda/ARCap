/*
 * InfraredNetworkBridge.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef INFRAREDNETWORKBRIDGE_H_
#define INFRAREDNETWORKBRIDGE_H_

#include "includes.h"

#include "Bridge.h"
#include "Debug.h"
#include "NetworkHandler.h"
#include "Status.h"

#define INFRARED_TO_NETWORK_RECEIVE_THRESHOLD 50
#define INFRARED_TO_NETWORK_RECEIVE_TIMEOUT_SECONDS 1

/*
 * The infrared-to-network-bridge listens for infrared events
 * and forwards valid events, such as infrared receive signals exceeding a certain threshold,
 * to the associated network handler.
 */
class InfraredNetworkBridge: public Bridge {
public:
	/*
	 * Creates a new infrared-to-network bridge.
	 * @param target - the network handler to which this bridge will forward infrared events
	 */
	InfraredNetworkBridge(NetworkHandler *target);
	virtual ~InfraredNetworkBridge();

	/*
	 * Checks the infrared receive queue for new events.
	 * @return OK when a new infrared event is received and processed without error
	 */
	Status update();

private:
	/* The network handler to which infrared events will be forwarded. */
	NetworkHandler *network;

	/*
	 * Processes the infrared reading of the given level.
	 * The reading will be forwarded to the network if it is above the
	 * INFRARED_TO_NETWORK_RECEIVE_THRESHOLD.
	 * @param level the 12-bit infrared input level to process
	 * @return OK if the reading was processed without error
	 */
	Status process(unsigned int level);
};

#if defined(INFRAREDNETWORKBRIDGE_DEBUG) || defined(DEBUG)
#define INFRAREDNETWORKBRIDGE_LOG(info) info
#else
#define INFRAREDNETWORKBRIDGE_LOG(info)
#endif

#endif /* INFRAREDNETWORKBRIDGE_H_ */
