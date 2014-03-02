/*
 * SerialHandler.h
 *
 *  Created on: 2014-03-01
 *      Author: gongal
 */

#ifndef SERIALHANDLER_H_
#define SERIALHANDLER_H_


class SerialHandler {
public:
	SerialHandler();
	virtual ~SerialHandler();
	void sendByte(char * msg);
	//TODO implement read byte
};

#endif /* SERIALHANDLER_H_ */
