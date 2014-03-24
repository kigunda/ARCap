/*
 * NetworkTasks.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#include "NetworkTasks.h"

extern NetworkHandler *network;

/* Polls the network handler for network updates. */
void network_handler_update_task(void *pdata) {
	printf("NetworkHandler [task: update, status: start]\n");
	Status status = network->connect();
	while (status == OK) {
//		// Update the network handler.
//		status = network->update();
		// [Test] Wait for 10 seconds.
		OSTimeDlyHMSM(0, 0, 10, 0);
//		// Wait.
//		OSTimeDlyHMSM(0, 0, 0, NETWORK_HANDLER_UPDATE_TIME_MILLIS);
	}
}



