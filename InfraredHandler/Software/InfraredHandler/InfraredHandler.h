/*
 * InfraredHandler.h
 *
 *  Created on: 2014-02-28
 *      Author: Kenan Kigunda
 */

#ifndef INFRAREDHANDLER_H_
#define INFRAREDHANDLER_H_

#include "altera_avalon_pio_regs.h"
#include "altera_up_avalon_de0_nano_adc.h"
#include "includes.h"

#include "DataSource.h"
#include "Status.h"

#define INFRARED_HANDLER_UPDATE_TIME_MILLIS			50
#define INFRARED_HANDLER_EMITTER_ON_TIME_SECONDS	1
#define INFRARED_HANDLER_EMITTER_OFF_TIME_SECONDS	1

class InfraredHandler: public DataSource {
public:
	InfraredHandler();
	virtual ~InfraredHandler();

	/*
	 * Initializes this handler.
	 * @return OK if there are no initialization errors
	 */
	Status init();

	/*
	 * Updates this handler.
	 * @return OK if the infrared readings are valid and have been posted to all listeners without error
	 */
	Status update();

	/**
	 * Sends a signal from the infrared emitters.
	 * @return OK if the signal was sent successfully
	 */
	Status send();

private:
	/* The analog-to-digital converter controller used to read from the infrared. */
	alt_up_de0_nano_adc_dev *adc_dev;

	/*
	 * Reads the level of the given receive channel.
	 * @param channel - the number of the ADC channel to read
	 * @return the 12-bit level read from the channel, indicating the amount of infrared light
	 * hitting the receivers
	 * */
	unsigned int read(int channel);

	/*
	 * Posts the given infrared level readings to all listener queues.
	 * @param level - the level read by the infrared receivers
	 * @return OK if the readings are posted to all listener queues without error
	 */
	Status onInfraredReceive(unsigned int level);

};

//#define INFRAREDHANDLER_DEBUG

#define INFRAREDHANDLER_SEND_DEBUG
#if defined(INFRAREDHANDLER_SEND_DEBUG) || defined(INFRAREDHANDLER_DEBUG) || defined(DEBUG)
#define INFRAREDHANDLER_SEND_LOG(info) info
#else
#define INFRAREDHANDLER_SEND_LOG(info)
#endif

//#define INFRAREDHANDLER_RECEIVE_DEBUG
#if defined(INFRAREDHANDLER_RECEIVE_DEBUG) || defined(INFRAREDHANDLER_DEBUG) || defined(DEBUG)
#define INFRAREDHANDLER_RECEIVE_LOG(info) info
#else
#define INFRAREDHANDLER_RECEIVE_LOG(info)
#endif

#endif /* INFRAREDHANDLER_H_ */
