/*
 * NetworkInfraredBridge.h
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#ifndef NETWORKINFRAREDBRIDGE_H_
#define NETWORKINFRAREDBRIDGE_H_

#include "includes.h"

#include "Bridge.h"
#include "Debug.h"
#include "InfraredHandler.h"
#include "NetworkCommand.h"
#include "Status.h"

/*
 * The network-to-infrared bridge listens for network events
 * and forwards events requiring infrared control, such as "shoot" commands,
 * to the associated infrared handler.
 */
class NetworkInfraredBridge: public Bridge {
public:
	/*
	 * Creates a new network-to-infrared bridge.
	 * @param target - the infrared handler to which this bridge will forward network events
	 */
	NetworkInfraredBridge(InfraredHandler *target);
	virtual ~NetworkInfraredBridge();

	/*
	 * Checks the network receive queue for infrared commands.
	 * @return OK when a new command is received and processed without error
	 */
	Status update();

private:
	/* The infrared handler to which network events will be forwarded. */
	InfraredHandler *infrared;

	/*
	 * Processes a network command. The command will be accepted if it is an infrared command.
	 * @param command - the command to process
	 * @return OK if the command was accepted and forwarded
	 * @return ERR_BRIDGE_MESSAGE_NOT_ACCEPTED if the command was not accepted
	 */
	Status process(NetworkCommand command);
};

#if defined(NETWORKINFRAREDBRIDGE_DEBUG) || defined(DEBUG)
#define NETWORKINFRAREDBRIDGE_LOG(info) info
#else
#define NETWORKINFRAREDBRIDGE_LOG(info)
#endif

#endif /* NETWORKINFRAREDBRIDGE_H_ */
