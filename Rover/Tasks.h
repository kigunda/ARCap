/*
 * TaskProperties.h
 *
 *  Created on: 2014-03-01
 *      Author: Kenan Kigunda
 */

#ifndef TASKPROPERTIES_H_
#define TASKPROPERTIES_H_

#include "includes.h"

/* Definition of Task Stacks */
#define	TASK_STACKSIZE      2048
OS_STK task1_stk			[TASK_STACKSIZE];
OS_STK task2_stk			[TASK_STACKSIZE];
OS_STK task3_stk			[TASK_STACKSIZE];
OS_STK task4_stk			[TASK_STACKSIZE];
OS_STK task5_stk			[TASK_STACKSIZE];
OS_STK task6_stk			[TASK_STACKSIZE];
OS_STK task7_stk			[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK1_PRIORITY		1
#define TASK2_PRIORITY      2
#define TASK3_PRIORITY		3
#define TASK4_PRIORITY		4
#define TASK5_PRIORITY		5
#define TASK6_PRIORITY		6
#define TASK7_PRIORITY		7

#endif /* TASKPROPERTIES_H_ */
