/*
 * InfraredReceiver.cpp
 *
 *  Created on: 2014-03-23
 *      Author: Kenan Kigunda
 */

#include "InfraredReceiver.h"

#define INFRARED_RECEIVER_ADC_READ_UNUSED 	4
#define INFRARED_RECEIVER_ADC_READ_BASE		(1 << 25)

// TASKS

extern InfraredReceiver *infrared;

/* Polls the infrared receivers. */
void infrared_receiver_update_task(void* pdata) {
	printf("InfraredReceiver [task: update, status: start]\n");
	while (true) {
		try {
			// Update the infrared receiver.
			infrared->update();
		} catch (ARCapException &e) {
			// If exception, print.
			INFRAREDRECEIVER_LOG(printf("%s", e.what()));
		}
		// [Test] Wait for 1 second.
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}

// ALLOCATION

/**
 * Creates a new infrared receiver.
 * @throw ADCOpenException if the receiver cannot connect to the analog-to-digital converter
 */
InfraredReceiver::InfraredReceiver() {
	// Open the ADC.
	adc_dev = alt_up_de0_nano_adc_open_dev(ADC_NAME);
	if (adc_dev == NULL) {
		throw new ADCOpenException();
	}
}

// LISTENERS

/**
 * Sets the listener to which the infrared readings will be posted.
 * @param queue - the queue to set as the listener
 */
void InfraredReceiver::setListener(OS_EVENT *queue) {
	listener = queue;
}

/*
 * Posts the given infrared level readings to the listener.
 * @param level - the level read by the infrared receivers
 * @throw PostException is the reading cannot be posted to the listener
 */
void InfraredReceiver::post(unsigned int level) {
	INT8U status;
	status = OSQPost(listener, (void *)level);
	if (status != OS_NO_ERR) {
		throw new QueuePostException();
	}
}

// UPDATES

/*
 * Updates this receiver.
 * @return OK if the infrared readings are valid and have been posted to the listener without error
 */
void InfraredReceiver::update() {
	post(read(1));
}

/*
 * Reads the level of the given receive channel.
 * @param channel - the number of the ADC channel to read
 * @return the 12-bit level read from the channel, indicating the amount of infrared light
 * hitting the receivers
 * */
unsigned int InfraredReceiver::read(int channel) {
	alt_up_de0_nano_adc_update(adc_dev);
	unsigned int level = (alt_up_de0_nano_adc_read(adc_dev, channel) >> INFRARED_RECEIVER_ADC_READ_UNUSED) - INFRARED_RECEIVER_ADC_READ_BASE;
	INFRAREDRECEIVER_LOG(printf("InfraredHandler [channel: %d, level: %u]\n", channel, level));
	return level;
}
