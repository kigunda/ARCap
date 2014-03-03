/*
 * NetworkInfraredBridge.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#include <stdio.h>
#include "NetworkInfraredBridge.h"

// ALLOCATION

/*
 * Creates a new network-to-infrared bridge.
 * @param target - the infrared handler to which this bridge will forward network events
 */
NetworkInfraredBridge::NetworkInfraredBridge(InfraredHandler *target) {
	infrared = target;
}
NetworkInfraredBridge::~NetworkInfraredBridge() {}

// UPDATES

/*
 * Checks the network receive queue for infrared commands.
 * @return OK when a new command is received and processed without error
 */
Status NetworkInfraredBridge::update() {
	INT8U status;
	NetworkCommand command = (NetworkCommand)(int)OSQPend(receiveQueue, 0, &status);
	if (status != OS_NO_ERR) {
		return ERR_BRIDGE_READ;
	} else {
		return process(command);
	}
}

/*
 * Processes a network command. The command will be accepted if it is an infrared command.
 * @param command - the command to process
 * @return OK if the command was accepted and forwarded
 * @return ERR_BRIDGE_MESSAGE_NOT_ACCEPTED if the command was not accepted
 */
Status NetworkInfraredBridge::process(NetworkCommand command) {
	if (command == NETWORK_COMMAND_SHOOT) {
		NETWORKINFRAREDBRIDGE_LOG(printf("NetworkToInfrared [event: infrared shoot]\n"));
		infrared->send();
		return OK;
	} else {
		return ERR_BRIDGE_MESSAGE_NOT_ACCEPTED;
	}
}
