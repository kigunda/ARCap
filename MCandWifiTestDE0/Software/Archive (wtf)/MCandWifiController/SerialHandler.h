/*
 * SerialHandler.h
 *
 *  Created on: 2014-03-01
 *      Author: gongal
 */

#ifndef SERIALHANDLER_H_
#define SERIALHANDLER_H_

#define SERIAL_START_BYTE 0x80

#define SERIAL_DEVICE_TYPE 0x00
#define SERIAL_MOTOR2_BACKWARD 0x04 // motor 2 backward
#define SERIAL_MOTOR3_BACKWARD 0x06 //motor 3 backward
#define SERIAL_MOTOR2_FORWARD 0x05 // motor 2 forward
#define SERIAL_MOTOR3_FORWARD 0x07 // motor 3 forward
#define SERIAL_CONST_SPEED 0x5F
#define SERIAL_STOP_SPEED 0x00

class SerialHandler {
public:
	SerialHandler();
	virtual ~SerialHandler();
	void sendByteMC(char msg);
	void sendDataWifi(char * msg, int length);
};

#endif /* SERIALHANDLER_H_ */
