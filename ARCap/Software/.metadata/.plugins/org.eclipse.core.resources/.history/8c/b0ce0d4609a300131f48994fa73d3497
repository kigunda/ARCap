/*
 * InfaredHandler.cpp
 *
 *  Created on: 2014-02-28
 *      Author: Kenan Kigunda
 */

#include <stdio.h>

#include "InfraredHandler.h"

#define INFRARED_HANDLER_ADC_READ_UNUSED 	4
#define INFRARED_HANDLER_ADC_READ_BASE		(1 << 25)

#define INFRARED_HANDLER_EMITTTER_ON		1
#define INFRARED_HANDLER_EMITTER_OFF		0

// ALLOCATION
InfraredHandler::InfraredHandler() {}
InfraredHandler::~InfraredHandler() {}

// INITIALIZATION
/*
 * Initializes this handler.
 * @return INFRARED_OK if there are no initialization errors
 */
Status InfraredHandler::init() {
	// Open the ADC.
	adc_dev = alt_up_de0_nano_adc_open_dev(ADC_NAME);
	if (adc_dev == NULL) {
		return ERR_INFRARED;
	} else {
		return OK;
	}
}

// EMITTERS

/**
 * Sends a signal from the infrared emitters.
 * @return OK if the signal was sent successfully
 */
Status InfraredHandler::send() {
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, INFRARED_HANDLER_EMITTTER_ON);
	INFRAREDHANDLER_SEND_LOG(printf("InfraredHandler [emitter: on]\n"));
	OSTimeDlyHMSM(0, 0, INFRARED_HANDLER_EMITTER_ON_TIME_SECONDS, 0);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, INFRARED_HANDLER_EMITTER_OFF);
	INFRAREDHANDLER_SEND_LOG(printf("InfraredHandler [emitter: off]\n"));
	return OK;
}

// RECEIVERS

/*
 * Updates the infrared readings by checking all receivers.
 * @return OK if the infrared readings are accepted by all listeners
 */
Status InfraredHandler::update() {
	return onInfraredReceive(read(1));
}

/*
 * Reads the level of the given receive channel.
 * @param channel - the number of the ADC channel to read
 * @return the 12-bit level read from the channel, indicating the amount of infrared light
 * hitting the receivers
 * */
unsigned int InfraredHandler::read(int channel) {
	alt_up_de0_nano_adc_update(adc_dev);
	unsigned int level = (alt_up_de0_nano_adc_read(adc_dev, channel) >> INFRARED_HANDLER_ADC_READ_UNUSED) - INFRARED_HANDLER_ADC_READ_BASE;
	INFRAREDHANDLER_RECEIVE_LOG(printf("InfraredHandler [channel: %d, level: %u]\n", channel, level));
	return level;
}

/*
 * Posts the given infrared level readings to all listener queues.
 * @param level - the level read by the infrared receivers
 * @return OK if the readings are posted to all listener queues without error
 */
Status InfraredHandler::onInfraredReceive(unsigned int level) {
	Status overall = OK;
	INT8U status;
	for (list<OS_EVENT *>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
		status = OSQPost(*it, (void*)level);
		if (status != OS_NO_ERR) overall = ERR_INFRARED;
	} return overall;
}


