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
OS_STK    command_check_infrared_task_stk		[TASK_STACKSIZE];
OS_STK    infrared_read_task_stk				[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define COMMAND_CHECK_INFRARED_TASK_PRIORITY		1
#define INFRARED_READ_TASK_PRIORITY      			2

#endif /* TASKPROPERTIES_H_ */
