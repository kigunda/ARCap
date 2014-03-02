/*
 * InfraredTasks.cpp
 *
 *  Created on: 2014-03-01
 *      Author: kigunda
 */

#include "InfraredTasks.h"

extern InfraredHandler *infrared;

/* Polls the infrared handler for infrared updates. */
void infrared_read_task(void* pdata) {
	int status = OK;
	while (status == OK) {
		// Update the infrared handler.
		status = infrared->update();
		// Wait for 500 ms.
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
}



