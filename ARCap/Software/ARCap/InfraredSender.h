/*
 * InfraredSender.h
 *
 *  Created on: 2014-03-23
 *      Author: kigunda
 */

#ifndef INFRAREDSENDER_H_
#define INFRAREDSENDER_H_

#include "altera_avalon_pio_regs.h"
#include "Listener.h"

/* Waits for infrared send commands and executes them as they arrive. */
void infrared_sender_update_task(void *pdata);

/* @test Periodically sends an infrared signal. */
void infrared_sender_test_task(void *pdata);

/**
 * Listens for send commands on a queue and applies them to the infrared emitters.
 */
class InfraredSender: public Listener {
public:
	/*
	 * Waits on the receive queue for send commands.
	 * When a send command is received, this sender issues a signal to the emitters.
	 */
	void update();

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
