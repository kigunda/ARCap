/*
 * InfraredHandler.h
 *
 *  Created on: 2014-02-28
 *      Author: kigunda
 */

#include "altera_up_avalon_de0_nano_adc.h"
#include "altera_up_avalon_de0_nano_adc_regs.h"

#ifndef INFRARED_HANDLER_H_
#define INFRARED_HANDLER_H_

#define INFRARED_HANDLER_OK 0
#define INFRARED_HANDLER_ERR -1

class InfraredHandler {
public:
	InfraredHandler();
	virtual ~InfraredHandler();
	int init();
private:
	alt_up_de0_nano_adc_dev *adc_dev;
	bool read(int channel);
};

#endif /* INFRAREDHANDLER_H_ */
