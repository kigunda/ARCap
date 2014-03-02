/*
 * InfraredTasks.cpp
 *
 *  Created on: 2014-03-01
 *      Author: kigunda
 */

#include "InfraredTasks.h"

extern InfraredHandler *infrared;
extern InfraredNetworkBridge *infraredToNetwork;

/* Polls the infrared handler for infrared updates. */
void infrared_handler_update_task(void* pdata) {
	int status = OK;
	while (status == OK) {
		// Update the infrared handler.
		status = infrared->update();
		// Wait for 50 ms.
		OSTimeDlyHMSM(0, 0, 0, 50);
	}
}

/* Waits for infrared updates to push to the network. */
void infrared_to_network_update_task(void* pdata) {
	while (true) {
		infraredToNetwork->update();
	}
}



