/*
 * SerialHandler.cpp
 *
 *  Created on: 2014-03-01
 *      Author: gongal
 */

#include "SerialHandler.h"
#include "altera_avalon_uart_regs.h"

SerialHandler::SerialHandler() {
	// TODO Auto-generated constructor stub

}

SerialHandler::~SerialHandler() {
	// TODO Auto-generated destructor stub
}

void SerialHandler::sendByte(char * msg){
	int i;
		for(i = 0; i<8; i++){
			IOWR_ALTERA_AVALON_UART_TXDATA(UART_MC_BASE, msg[i]);
		}
}

