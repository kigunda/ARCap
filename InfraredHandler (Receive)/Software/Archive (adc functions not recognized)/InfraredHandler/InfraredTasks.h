/*
 * InfraredTasks.h
 *
 *  Created on: 2014-03-01
 *      Author: kigunda
 */

#ifndef INFRAREDTASKS_H_
#define INFRAREDTASKS_H_

#include "InfraredHandler.h"

/* Polls the infrared handler for infrared updates. */
void infrared_read_task(void* pdata);

#endif /* INFRAREDTASKS_H_ */
