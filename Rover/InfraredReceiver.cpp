/*
 * InfraredReceiver.cpp
 *
 *  Created on: 2014-03-23
 *      Author: Kenan Kigunda
 */

#include "InfraredReceiver.h"

#define INFRARED_RECEIVER_ADC_READ_UNUSED 	4
#define INFRARED_RECEIVER_ADC_READ_BASE		(1 << 25)

const char *InfraredReceiver::HIT = INFRARED_COMMAND_HIT;

// TASKS

extern InfraredReceiver *infraredIn;

/* Polls the infrared receivers. */
void infrared_receiver_update_task(void* pdata) {
	TASK_LOG(printf("InfraredReceiver [task: update, status: start]\n"));
	while (true) {
		bool hit = false;
		try {
			// Update the infrared receiver.
			hit = infraredIn->update();
		} catch (ARCapException &e) {
			// Log exceptions.
			INFRAREDRECEIVER_LOG(printf("%s\n", e.what()));
		}
		// Wait.
		if (hit) {
			OSTimeDlyHMSM(0, 0, INFRARED_RECEIVER_HIT_UPDATE_TIME_SECONDS, INFRARED_RECEIVER_HIT_UPDATE_TIME_MILLIS);
		} else {
			OSTimeDlyHMSM(0, 0, INFRARED_RECEIVER_NO_HIT_UPDATE_TIME_SECONDS, INFRARED_RECEIVER_NO_HIT_UPDATE_TIME_MILLIS);
		}
	}
}

// CONSTRUCTION

/**
 * Creates a new infrared receiver.
 * @throw ADCOpenException if the receiver cannot connect to the analog-to-digital converter
 */
InfraredReceiver::InfraredReceiver() {
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

/**
 * Checks the given infrared reading level against the infrared hit threshold.
 * If the level exceeds the threshold, a hit event will be posted to the listener.
 * @param level - the level read by the infrared receivers
 * @throw PostException if the hit event cannot be posted to the listener
 */
bool InfraredReceiver::check(unsigned int level) {
	if (level > INFRARED_RECEIVER_HIT_THRESHOLD) {
		INFRAREDRECEIVER_LOG(printf("InfraredReceiver [event: HIT]\n"));
		post(HIT);
		return true;
	} else {
		return false;
	}
}

/**
 * Posts an infrared receive event to the listener.
 * @param event - the name of the event, which must start with 'i' for infrared
 * @throw PostException if the event cannot be posted to the listener
 */
void InfraredReceiver::post(const char *event) {
	INT8U status;
	status = OSQPost(listener, (void *)event);
	if (status != OS_NO_ERR) {
		throw new QueuePostException();
	}
}

// UPDATES

/**
 * Updates this receiver. The receiver will read the ADC and post the readings to the listener.
 * @throw PostException if the reading cannot be posted to the listener
 */
bool InfraredReceiver::update() {
	return check(read(1));
}

// RECEIVERS

/*
 * Reads the level of the given receive channel.
 * @param channel - the number of the ADC channel to read
 * @return the 12-bit level read from the channel, indicating the amount of infrared light
 * hitting the receivers
 * */
unsigned int InfraredReceiver::read(int channel) {
	alt_up_de0_nano_adc_update(adc_dev);
	unsigned int level = (alt_up_de0_nano_adc_read(adc_dev, channel) >> INFRARED_RECEIVER_ADC_READ_UNUSED) - INFRARED_RECEIVER_ADC_READ_BASE;
	INFRAREDRECEIVER_LOG(printf("InfraredReceiver [channel: %d, level: %u]\n", channel, level));
	return level;
}
