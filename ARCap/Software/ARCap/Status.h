/*
 * Status.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef STATUS_H_
#define STATUS_H_

typedef enum Status {
	OK = 0,
	ERR_INFRARED,
	ERR_BRIDGE_CREATE,
	ERR_BRIDGE_READ,
	ERR_BRIDGE_MESSAGE_NOT_ACCEPTED
} Status;

#endif /* STATUS_H_ */
