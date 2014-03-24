/*************************************************************************
 * ARCap
 * Kenan Kigunda, Amshu Gongal, Matt Pon
 * March 2014
 **************************************************************************/

using namespace std;
#include <stdbool.h>
#include <stdio.h>

#include "includes.h"

#include "TaskProperties.h"
#include "Tasks.h"

#include "InfraredReceiver.h"
#include "InfraredSender.h"

InfraredReceiver *infrared;
InfraredSender *infraredSender;

NetworkHandler *network = new NetworkHandler();
InfraredNetworkBridge *infraredToNetwork = new InfraredNetworkBridge(network);

/* The main function registers the infrared tasks and starts multi-tasking */
int main(void) {

	// Register tasks.
	//	OSTaskCreateExt(network_handler_update_task,
	//			0,
	//			&task1_stk[TASK_STACKSIZE - 1],
	//			TASK1_PRIORITY,
	//			TASK1_PRIORITY,
	//			task1_stk,
	//			TASK_STACKSIZE,
	//			0,
	//			0);
	OSTaskCreateExt(infrared_receiver_update_task,
			NULL,
			&task2_stk[TASK_STACKSIZE-1],
			TASK2_PRIORITY,
			TASK2_PRIORITY,
			task2_stk,
			TASK_STACKSIZE,
			NULL,
			0);
	OSTaskCreateExt(infrared_to_network_update_task,
			0,
			&task3_stk[TASK_STACKSIZE-1],
			TASK3_PRIORITY,
			TASK3_PRIORITY,
			task3_stk,
			TASK_STACKSIZE,
			0,
			0);
	OSTaskCreateExt(infrared_sender_test_task,
			0,
			&task4_stk[TASK_STACKSIZE-1],
			TASK4_PRIORITY,
			TASK4_PRIORITY,
			task4_stk,
			TASK_STACKSIZE,
			0,
			0);

	// Initialize handlers.
	try {
		infrared = new InfraredReceiver();
		infraredSender = new InfraredSender();

		// Initialize the handlers and bridges. (OLD)
		if (	(network->init() == OK) &&
				(infraredToNetwork->init() == OK)
		) {
			printf("Main [initialize, status: OK]\n");

			// Create the communications chain.
			infrared->setListener(infraredToNetwork->listener());

			// Start.
			OSStart();
		}

	} catch (ARCapException &e) {
		printf("%s", e.what());
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
