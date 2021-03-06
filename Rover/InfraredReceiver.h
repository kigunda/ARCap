/*
 * InfraredReceiver.h
 *
 *  Created on: 2014-03-23
 *      Author: Kenan Kigunda
 */

#ifndef INFRAREDRECEIVER_H_
#define INFRAREDRECEIVER_H_

#include "altera_up_avalon_de0_nano_adc.h"
#include "altera_up_avalon_de0_nano_adc_regs.h"
#include "Status.h"

#define INFRARED_RECEIVER_HIT_THRESHOLD			25

#define INFRARED_RECEIVER_NO_HIT_UPDATE_TIME_SECONDS	0
#define INFRARED_RECEIVER_NO_HIT_UPDATE_TIME_MILLIS		200
#define INFRARED_RECEIVER_HIT_UPDATE_TIME_SECONDS		4
#define INFRARED_RECEIVER_HIT_UPDATE_TIME_MILLIS		0

/* Polls the infrared receivers. */
void infrared_receiver_update_task(void* pdata);

/**
 * Reads input from the infrared receivers through the analog-to-digital converter
 * (ADC) and posts infrared hit events to a listener.
 */
class InfraredReceiver {
public:
	/* Marks an infrared hit event. */
	static const char *HIT;

	/**
	 * Creates a new infrared receiver.
	 * @throw ADCOpenException if the receiver cannot connect to the analog-to-digital converter
	 */
	InfraredReceiver();

	/**
	 * Sets the listener to which the infrared readings will be posted.
	 * @param queue - the queue to set as the listener
	 */
	void setListener(OS_EVENT *queue);

	/**
	 * Updates this receiver. The receiver will read the ADC and post the readings to the listener.
	 * @return true if the receiver triggered an infrared hit event
	 * @throw PostException if the reading cannot be posted to the listener
	 */
	bool update();

private:
	/* The analog-to-digital converter control device used to read from the infrared receivers. */
	alt_up_de0_nano_adc_dev *adc_dev;

	/* The listener queue to which infrared receive events will be posted. */
	OS_EVENT *listener;

	/**
	 * Reads the level of the given receive channel.
	 * @param channel - the number of the ADC channel to read
	 * @return the 12-bit level read from the channel, indicating the amount of infrared light
	 * hitting the receivers
	 * */
	unsigned int read(int channel);

	/**
	 * Checks the given infrared reading level against the infrared hit threshold.
	 * If the level exceeds the threshold, a hit event will be posted to the listener.
	 * @param level - the level read by the infrared receivers
	 * @return true if the level exceeds the threshhold
	 * @throw PostException if the hit event cannot be posted to the listener
	 */
	bool check(unsigned int level);

	/**
	 * Posts an infrared receive event to the listener.
	 * @param event - the name of the event, which must start with 'i' for infrared
	 * @throw PostException if the event cannot be posted to the listener
	 */
	void post(const char *event);

};

class ADCOpenException : ARCapException {
public:
	virtual const char *what() const throw() {
		return "Failed to open connection to analog-to-digital converter.";
	}
};

#if defined(INFRAREDRECEIVER_DEBUG) || defined(DEBUG)
#define INFRAREDRECEIVER_LOG(info) info
#else
#define INFRAREDRECEIVER_LOG(info)
#endif

#endif /* INFRAREDRECEIVER_H_ */
