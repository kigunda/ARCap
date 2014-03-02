/*
 * CommandHandler.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_

#include "includes.h"

#include "Status.h"

#define COMMAND_INFRARED_RECEIVE_SIZE 4

class CommandHandler {
public:
	CommandHandler();
	virtual ~CommandHandler();

	/*
	 * Initializes this handler.
	 * @return OK if there are no initialization errors
	 */
	Status init();

	// INFRARED

	/*
	 * Gets the queue used to accept infrared receive events.
	 * @return the infrared receive queue
	 */
	OS_EVENT *onInfraredReceive();

	/*
	 * Checks the infrared receive queue for new readings.
	 * @return OK when a new reading is received and processed without error
	 */
	Status checkInfraredReceive();

private:
	int infraredReceiveArray[COMMAND_INFRARED_RECEIVE_SIZE];
	OS_EVENT *infraredReceiveQueue;

	Status createInfraredReceive();
	Status processInfraredReceive(unsigned int level);
};

#endif /* COMMANDHANDLER_H_ */
