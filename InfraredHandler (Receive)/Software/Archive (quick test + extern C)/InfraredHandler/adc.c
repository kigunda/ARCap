/*
 * adc.c
 *
 *  Created on: 2014-03-02
 *      Author: kigunda
 */

#include <stdio.h>
#include "includes.h"
#include "adc.h"
#include "altera_up_avalon_de0_nano_adc.h"

#define ADC_READ_OFFSET		4
#define ADC_READ_BASE		33554432

/* Read from the ADC. */
void task2(void* pdata) {
	// Open the ADC controller.
	alt_up_de0_nano_adc_dev *adc = alt_up_de0_nano_adc_open_dev(ADC_NAME);
	int channel = 1;
	while (1) {
		// Read from the ADC.
		alt_up_de0_nano_adc_update(adc);
		unsigned int level = (alt_up_de0_nano_adc_read(adc, channel) >> ADC_READ_OFFSET) - ADC_READ_BASE;
		printf("ADC [channel: %d, level: %u]\n", channel, level);
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
}
