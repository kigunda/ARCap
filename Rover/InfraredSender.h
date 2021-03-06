/*
 * InfraredSender.h
 *
 *  Created on: 2014-03-23
 *      Author: Kenan Kigunda
 */

#ifndef INFRAREDSENDER_H_
#define INFRAREDSENDER_H_

#include "altera_avalon_pio_regs.h"
#include "Listener.h"

#define INFRARED_SENDER_ON_TIME_SECONDS		3
#define INFRARED_SENDER_OFF_TIME_SECONDS	1

/* Waits for infrared send commands and executes them as they arrive. */
void infrared_sender_update_task(void *pdata);

/* @test Periodically sends an infrared signal. */
void infrared_sender_test_task(void *pdata);

/**
 * Listens for send commands on a queue and applies them to the infrared emitters.
 */
class InfraredSender: public Listener {
public:
	/**
	 * Creates a new infrared sender.
	 * Turns the emitters on for verification.
	 */
	InfraredSender();

	/**
	 * Tests the infrared emitters.
	 */
	void test();

private:
	/**
	 * Parses the given infrared command.
	 * @command - the infrared command to parse.
	 * If the command is a send command, this sender issues a signal to the emitters.
	 * The command must be dynamically allocated, and will be freed at the end of this method.
	 */
	void parse(char *command);

	/**
	 * Sends a signal from the infrared emitters.
	 */
	void send();
};

#if defined(INFRAREDSENDER_DEBUG) || defined(DEBUG)
#define INFRAREDSENDER_LOG(info) info
#else
#define INFRAREDSENDER_LOG(info)
#endif

#endif /* INFRAREDSENDER_H_ */
