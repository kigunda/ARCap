/*
 * TaskProperties.h
 *
 *  Created on: 2014-03-01
 *      Author: kigunda
 */

#ifndef TASKPROPERTIES_H_
#define TASKPROPERTIES_H_

#include "includes.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       	2048
OS_STK    infrared_handler_update_task_stk				[TASK_STACKSIZE];
OS_STK    infrared_to_network_update_task_stk			[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define INFRARED_HANDLER_UPDATE_TASK_PRIORITY      			1
#define INFRARED_TO_NETWORK_UPDATE_TASK_PRIORITY			2

#endif /* TASKPROPERTIES_H_ */
