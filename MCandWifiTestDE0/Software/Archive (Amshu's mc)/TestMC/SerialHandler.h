/*
 * SerialHandler.h
 *
 *  Created on: 2014-03-01
 *      Author: gongal
 */

#ifndef SERIALHANDLER_H_
#define SERIALHANDLER_H_

#define SERIAL_START_BYTE "10000000"

#define SERIAL_DEVICE_TYPE "00000000"
#define SERIAL_MOTOR2_BACKWARD "00000100" // motor 2 backward
#define SERIAL_MOTOR3_BACKWARD "00000110" //motor 3 backward
#define SERIAL_MOTOR2_FORWARD "00000101" // motor 2 forward
#define SERIAL_MOTOR3_FORWARD "00000111" // motor 3 forward
#define SERIAL_CONST_SPEED "01011111"
#define SERIAL_STOP_SPEED "00000000"

class SerialHandler {
public:
	SerialHandler();
	virtual ~SerialHandler();
	void sendByteMC(char * msg);
	void sendDataWifi(char * msg, int length);
};

#endif /* SERIALHANDLER_H_ */
