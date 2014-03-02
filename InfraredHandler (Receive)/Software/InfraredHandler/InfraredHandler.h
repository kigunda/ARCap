/*
 * InfraredHandler.h
 *
 *  Created on: 2014-02-28
 *      Author: Kenan Kigunda
 */

#ifndef INFRAREDHANDLER_H_
#define INFRAREDHANDLER_H_

#include "altera_up_avalon_de0_nano_adc.h"
#include "includes.h"

#include "DataSource.h"
#include "Status.h"

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

	// TASKS
	void registerInfraredReceiveTask();

private:
	alt_up_de0_nano_adc_dev *adc_dev;

	unsigned int read(int channel);
	Status onInfraredReceive(unsigned int level);
};

//#define INFRAREDHANDLER_DEBUG
#if defined(INFRAREDHANDLER_DEBUG) || defined(DEBUG)
#define INFRAREDHANDLER_LOG(x) x
#else
#define INFRAREDHANDLER_LOG(x)
#endif

#endif /* INFRAREDHANDLER_H_ */
