/*
 * CommandTasks.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef COMMANDTASKS_H_
#define COMMANDTASKS_H_

#include "CommandHandler.h"

/* Monitors the infrared receive queue and responds to infrared receive events. */
void command_check_infrared_task(void* pdata);

#endif /* COMMANDTASKS_H_ */
