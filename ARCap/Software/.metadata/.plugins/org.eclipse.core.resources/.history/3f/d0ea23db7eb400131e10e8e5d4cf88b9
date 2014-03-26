/*
 * InfraredTasks.cpp
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#include "InfraredTasks.h"

extern InfraredNetworkBridge *infraredToNetwork;

/* Waits for infrared updates to push to the network. */
void infrared_to_network_update_task(void* pdata) {
	printf("InfraredNetworkBridge [task: update, status: start]\n");
	while (true) {
		infraredToNetwork->update();
	}
}



