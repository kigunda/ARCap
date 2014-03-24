/*
 * InfraredSender.cpp
 *
 *  Created on: 2014-03-23
 *      Author: kigunda
 */

#include "InfraredSender.h"

#define INFRARED_SENDER_ON_TIME_SECONDS		1
#define INFRARED_SENDER_OFF_TIME_SECONDS	1

#define INFRARED_SENDER_ON		1
#define INFRARED_SENDER_OFF		0

// TASKS

extern InfraredSender *infraredSender;

/* Waits for infrared send commands and executes them as they arrive. */
void infrared_sender_update_task(void *pdata) {
	printf("InfraredSender [task: update, status: start]\n");
	while (true) {
		infraredSender->update();
	}
}

/* @test Periodically sends an infrared signal. */
void infrared_sender_test_task(void *pdata) {
	printf("InfraredSender [task: send test, status: start]\n");
	while (true) {
		infraredSender->send();
		OSTimeDlyHMSM(0, 0, INFRARED_SENDER_OFF_TIME_SECONDS, 0);
	}
}

// UPDATES

/*
 * Waits on the receive queue for send commands.
 * When a send command is received, this sender issues a signal to the emitters.
 */
void InfraredSender::update() {
	INT8U status;
	char *command = (char *)OSQPend(receiveQueue, 0, &status);
	if (status != OS_NO_ERR) {
		throw new QueuePendException();
	} else {
		INFRAREDSENDER_LOG(printf("InfraredSender [command: %s]\n", command));
		// First character is 'i' for infrared; second character gives subtype:
		switch (command[0]) {
		case 's':
			// Shoot.
			send();
			break;
		}
		free(command);
	}
}

/**
 * Sends a signal from the infrared emitters.
 * @return OK if the signal was sent successfully
 */
void InfraredSender::send() {
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, INFRARED_SENDER_ON);
	INFRAREDSENDER_LOG(printf("InfraredSender [emitter: on]\n"));
	OSTimeDlyHMSM(0, 0, INFRARED_SENDER_ON_TIME_SECONDS, 0);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, INFRARED_SENDER_OFF);
	INFRAREDSENDER_LOG(printf("InfraredSender [emitter: off]\n"));
}

