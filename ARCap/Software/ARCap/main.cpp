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

#include "InfraredReceiver.h"
#include "InfraredSender.h"
#include "MotorHandler.h"
#include "NetworkSender.h"
#include "NetworkReceiver.h"

InfraredReceiver *infraredIn;
InfraredSender *infraredOut;
MotorHandler *motor;
WifiHandler *wifi;
NetworkSender *networkOut;
NetworkReceiver *networkIn;

/* The main function registers the infrared tasks and starts multi-tasking */
int main(void) {

	// WIFI TASKS
	OSTaskCreateExt(wifi_handler_tcp_start_task,
			0,
			&task1_stk[TASK_STACKSIZE - 1],
			TASK1_PRIORITY,
			TASK1_PRIORITY,
			task1_stk,
			TASK_STACKSIZE,
			0,
			0);
	OSTaskCreateExt(wifi_handler_tcp_update_task,
			0,
			&task2_stk[TASK_STACKSIZE - 1],
			TASK2_PRIORITY,
			TASK2_PRIORITY,
			task2_stk,
			TASK_STACKSIZE,
			0,
			0);

	// LISTENER TASKS
	OSTaskCreateExt(network_sender_update_task,
			0,
			&task3_stk[TASK_STACKSIZE-1],
			TASK3_PRIORITY,
			TASK3_PRIORITY,
			task3_stk,
			TASK_STACKSIZE,
			0,
			0);
	OSTaskCreateExt(infrared_sender_update_task,
			0,
			&task4_stk[TASK_STACKSIZE-1],
			TASK4_PRIORITY,
			TASK4_PRIORITY,
			task4_stk,
			TASK_STACKSIZE,
			0,
			0);
	OSTaskCreateExt(motor_handler_update_task,
			0,
			&task5_stk[TASK_STACKSIZE-1],
			TASK5_PRIORITY,
			TASK5_PRIORITY,
			task5_stk,
			TASK_STACKSIZE,
			0,
			0);

	// SOURCE TASKS
	OSTaskCreateExt(network_receiver_update_task,
			0,
			&task6_stk[TASK_STACKSIZE-1],
			TASK6_PRIORITY,
			TASK6_PRIORITY,
			task6_stk,
			TASK_STACKSIZE,
			0,
			0);
	OSTaskCreateExt(infrared_receiver_update_task,
			NULL,
			&task7_stk[TASK_STACKSIZE-1],
			TASK7_PRIORITY,
			TASK7_PRIORITY,
			task7_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	try {
		// Create handlers.
		infraredIn = new InfraredReceiver();
		infraredOut = new InfraredSender();
		motor = new MotorHandler();
		wifi = new WifiHandler();
		networkOut = new NetworkSender(wifi);
		networkIn = new NetworkReceiver(wifi);
		TASK_LOG(printf("Main [initialize, status: OK]\n"));

		// Create the communications chain.
		infraredIn->setListener(networkOut->listener());
		networkIn->addListener(INFRARED_SENDER_COMMAND, infraredOut->listener());
		networkIn->addListener(MOTOR_COMMAND, motor->listener());

		// Start.
		OSStart();

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
