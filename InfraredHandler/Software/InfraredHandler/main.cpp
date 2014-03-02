/*************************************************************************
 * ARCap
 * Kenan Kigunda, Amshu Gongal, Matt Pon
 * March 2014
 **************************************************************************/

using namespace std;
#include <stdbool.h>
#include <stdio.h>

#include "includes.h"

#include "Tasks.h"

/* Command handler. */
CommandHandler *command = new CommandHandler();
InfraredHandler *infrared = new InfraredHandler();

/* The main function creates the command and infrared tasks and starts multi-tasking */
int main(void) {

	// Register tasks.
	OSTaskCreateExt(infrared_read_task,
			NULL,
			&infrared_read_task_stk[TASK_STACKSIZE-1],
			INFRARED_READ_TASK_PRIORITY,
			INFRARED_READ_TASK_PRIORITY,
			infrared_read_task_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	OSTaskCreateExt(command_check_infrared_task,
			0,
			&command_check_infrared_task_stk[TASK_STACKSIZE-1],
			COMMAND_CHECK_INFRARED_TASK_PRIORITY,
			COMMAND_CHECK_INFRARED_TASK_PRIORITY,
			command_check_infrared_task_stk,
			TASK_STACKSIZE,
			0,
			0);


	// Initialize the handlers.
	if ((command->init() == OK) && (infrared->init() == OK)) {

		// Create the communications chain.
		infrared->addListener(command->onInfraredReceive());

		// Start.
		OSStart();
	}

	return 0;
}

/******************************************************************************
 *                                                                             *
 * License Agreement                                                           *
 *                                                                             *
 * Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
 * All rights reserved.                                                        *
 *                                                                             *
 * Permission is hereby granted, free of charge, to any person obtaining a     *
 * copy of this software and associated documentation files (the "Software"),  *
 * to deal in the Software without restriction, including without limitation   *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
 * and/or sell copies of the Software, and to permit persons to whom the       *
 * Software is furnished to do so, subject to the following conditions:        *
 *                                                                             *
 * The above copyright notice and this permission notice shall be included in  *
 * all copies or substantial portions of the Software.                         *
 *                                                                             *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
 * DEALINGS IN THE SOFTWARE.                                                   *
 *                                                                             *
 * This agreement shall be governed in all respects by the laws of the State   *
 * of California and by the laws of the United States of America.              *
 * Altera does not recommend, suggest or require that this reference design    *
 * file be used in conjunction or combination with any other product.          *
 ******************************************************************************/
