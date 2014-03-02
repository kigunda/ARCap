/*
 * CommandTasks.cpp
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#include "CommandTasks.h"

extern CommandHandler *command;

/* Monitors the infrared receive queue and responds to infrared receive events. */
void command_check_infrared_task(void* pdata) {
	while (true) {
		command->checkInfraredReceive();
	}
}

