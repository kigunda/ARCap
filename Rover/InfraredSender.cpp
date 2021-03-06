/*
 * InfraredSender.cpp
 *
 *  Created on: 2014-03-23
 *      Author: kigunda
 */

#include "InfraredSender.h"

#define INFRARED_SENDER_ON		1
#define INFRARED_SENDER_OFF		0

// TASKS

extern InfraredSender *infraredOut;

/* Waits for infrared send commands and executes them as they arrive. */
void infrared_sender_update_task(void *pdata) {
	TASK_LOG(printf("InfraredSender [task: update, status: start]\n"));
	while (true) {
		try {
			// Update the infrared sender.
			infraredOut->update();
		} catch (ARCapException &e) {
			// Log exceptions.
			INFRAREDSENDER_LOG(printf("%s\n", e.what()));
		}
	}
}

/* @test Periodically sends an infrared signal. */
void infrared_sender_test_task(void *pdata) {
	TASK_LOG(printf("InfraredSender [task: test, status: start]\n"));
	while (true) {
		infraredOut->test();
	}
}

// CONSTRUCTION

/**
 * Creates a new infrared sender.
 * Turns the emitters on for verification.
 */
InfraredSender::InfraredSender() {
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, INFRARED_SENDER_OFF);
}

// COMMANDS

/**
 * Parses the given infrared command.
 * @command - the infrared command to parse.
 * If the command is a send command, this sender issues a signal to the emitters.
 * The command must be dynamically allocated, and will be freed at the end of this method.
 */
void InfraredSender::parse(char *command) {
	if (command[COMMAND_TYPE_INDEX] == INFRARED_COMMAND) {
		INFRAREDSENDER_LOG(printf("InfraredSender [command: %s]\n", command));
		switch (command[INFRARED_TYPE_INDEX]) {
		case INFRARED_SHOOT:
			// Shoot.
			send();
			break;
		}
		free(command);
	}
}

// EMITTERS

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

// TESTING

/**
 * Tests the infrared emitters.
 */
void InfraredSender::test() {
	parse(INFRARED_COMMAND_SHOOT);
	OSTimeDlyHMSM(0, 0, INFRARED_SENDER_OFF_TIME_SECONDS, 0);
}

