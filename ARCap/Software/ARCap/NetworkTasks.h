/*
 * NetworkTasks.h
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#ifndef NETWORKTASKS_H_
#define NETWORKTASKS_H_

#include "NetworkHandler.h"

/* Polls the network handler for network updates. */
void network_handler_update_task(void* pdata);

#endif /* NETWORKTASKS_H_ */
