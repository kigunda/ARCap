/*
 * InfaredHandler.cpp
 *
 *  Created on: 2014-02-28
 *      Author: kigunda
 */

#include <stdio.h>
#include "InfraredHandler.h"

// CONSTRUCTION

/*
 * Creates a new infrared handler.
 */
InfraredHandler::InfraredHandler() {
}

/*
 * Destroys an infrared handler.
 */
InfraredHandler::~InfraredHandler() {
	// TODO Auto-generated destructor stub
}

/*
 * Initializes the infrared handler.
 */
int InfraredHandler::init() {
	// Open the ADc.
	adc_dev = alt_up_de0_nano_adc_open_dev(ADC_NAME);
	if (adc_dev == NULL) {
		return INFRARED_HANDLER_ERR;
	} else {
		return INFRARED_HANDLER_OK;
	}
}

// INFRARED RECEIVERS

bool InfraredHandler::read(int channel) {
	unsigned int status = alt_up_de0_nano_adc_read(adc_dev, channel);
	printf("InfraredHandler [channel: %d, status: %u] ", channel, status);
	if (status > 0) {
		return true;
	} else {
		return false;
	}
}

