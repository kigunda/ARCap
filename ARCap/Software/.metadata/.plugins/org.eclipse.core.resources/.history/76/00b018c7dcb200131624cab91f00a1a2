/*
 * InfraredTasks.cpp
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#include "InfraredTasks.h"

extern InfraredHandler *infrared;
extern InfraredNetworkBridge *infraredToNetwork;

/* Polls the infrared handler for infrared updates. */
void infrared_handler_update_task(void* pdata) {
	printf("InfraredHandler [task: update, status: start]\n");
	int status = OK;
	while (status == OK) {
		// Update the infrared handler.
		status = infrared->update();
		// [Test] Wait for 1 second.
		OSTimeDlyHMSM(0, 0, 1, 0);
//		// Wait.
//		OSTimeDlyHMSM(0, 0, 0, INFRARED_HANDLER_UPDATE_TIME_MILLIS);
	}
}

/* @test Periodically sends an infrared signal. */
void infrared_handler_send_test_task(void *pdata) {
	printf("InfraredHandler [task: send test, status: start]\n");
	while (true) {
		infrared->send();
		OSTimeDlyHMSM(0, 0, INFRARED_HANDLER_EMITTER_OFF_TIME_SECONDS, 0);
	}
}

/* Waits for infrared updates to push to the network. */
void infrared_to_network_update_task(void* pdata) {
	printf("InfraredNetworkBridge [task: update, status: start]\n");
	while (true) {
		infraredToNetwork->update();
	}
}



