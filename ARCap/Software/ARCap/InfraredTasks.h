/*
 * InfraredTasks.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef INFRAREDTASKS_H_
#define INFRAREDTASKS_H_

#include "InfraredNetworkBridge.h"

/* Waits for infrared updates to push to the network. */
void infrared_to_network_update_task(void* pdata);

#endif /* INFRAREDTASKS_H_ */
