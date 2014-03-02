/*
 * InfaredHandler.cpp
 *
 *  Created on: 2014-02-28
 *      Author: Kenan Kigunda
 */

#include <stdio.h>

#include "InfraredHandler.h"

#define ADC_READ_UNUSED 	4
#define ADC_READ_BASE		(1 << 25)

// ALLOCATION
InfraredHandler::InfraredHandler() {}
InfraredHandler::~InfraredHandler() {}

// INITIALIZATION

/* A mock implemementation of InfraredHandler::init, for testing. */
//Status InfraredHandler::init() {
//	return OK;
//}

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

// UPDATES

/*
 * Updates the infrared readings by checking all receivers.
 * @return OK if the infrared readings are accepted by all listeners
 */
Status InfraredHandler::update() {
	return onInfraredReceive(read(1));
}

// RECEIVERS

/* A mock implementation of InfraredHandler::read, for testing. */
//unsigned int InfraredHandler::read(int channel) {
//	return 10;
//}

/* Reads the level of the given receive channel. */
unsigned int InfraredHandler::read(int channel) {
	alt_up_de0_nano_adc_update(adc_dev);
	unsigned int level = (alt_up_de0_nano_adc_read(adc_dev, channel) >> ADC_READ_UNUSED) - ADC_READ_BASE;
	INFRAREDHANDLER_LOG(printf("InfraredHandler [channel: %d, level: %u]\n", channel, level));
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


