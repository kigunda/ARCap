/*************************************************************************
 * ARCap
 * Amshu Gongal, Kenan Kigunda
 * February 18, 2014
 **************************************************************************
 * Description:                                                           *
 * Prototype testing for infrared and mc.
 **************************************************************************/

#include <stdio.h>
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "alt_types.h"
#include "altera_up_avalon_rs232.h"
#include "altera_up_avalon_rs232_regs.h"

#include "MotorHandler.h"
#include "Status.h"

/* Task stacks */
#define   TASK_STACKSIZE       2048
OS_STK    ir_task_stk[TASK_STACKSIZE];
OS_STK    mc_task_stk[TASK_STACKSIZE];
/* Task priorities */
#define IR_TASK_PRIORITY	2
#define MC_TASK_PRIORITY	1


#define SW_READ 1
#define SW_WRITE 2
#define WRITE_FIFO_EMPTY 0x80
#define READ_FIFO_EMPTY 0x0
OS_EVENT *SWQ;
OS_EVENT *RS232Q;
INT8U err;


/* Error status */
#define OK 0

/* Message queues */
#define WAIT_FOREVER 0

// ==== INFRARED ====

/* Infrared message queue */
#define IR_MAX_MESSAGES 4
int ir_messages[IR_MAX_MESSAGES];
OS_EVENT *ir_queue;

/* Infrared messages */
// Together these avoid passing 0 to the queue (which is not allowed since queue messages cannot be null)
// and invert pushbutton input (since the pushbutton returns 0 when it is pushed down).
#define IR_QUEUE_SEND_BASE	1		// Use: IR_QUEUE_SEND_BASE + IORD
#define IR_QUEUE_RECEIVE_BASE 2		// Use: IR_QUEUE_RECEIVE_BASE - OSQPend


MotorHandler *motor= new MotorHandler();


// ==== mc

#define mc_GUARD_TIME 1

void mc_task(void *pdata)
{
	printf("mc task\n");
	printf("MotorHandler [init");
		if (motor->init() == OK) {
			printf("]\n\n");
		} else {
			printf( ", error]\n");
		}
		while(1){
			motor->motorCommand("left", "forward", "120");
			motor->motorCommand("right","forward", "120");
			OSTimeDlyHMSM(0, 0, 5, 0);
			motor->motorCommand("left", "forward", "0");
			motor->motorCommand("right","forward", "0");
			OSTimeDlyHMSM(0, 0, 5, 0);
			motor->motorCommand("left", "backward", "120");
			motor->motorCommand("right","backward", "120");
			OSTimeDlyHMSM(0, 0, 5, 0);
			motor->motorCommand("left", "forward", "0");
			motor->motorCommand("right","forward", "0");
			OSTimeDlyHMSM(0, 0, 5, 0);
			motor->motorCommand("right","forward", "120");
			OSTimeDlyHMSM(0, 0, 5, 0);
			motor->motorCommand("right","forward", "0");
			OSTimeDlyHMSM(0, 0, 5, 0);
			motor->motorCommand("left","forward", "120");
			OSTimeDlyHMSM(0, 0, 5, 0);
			motor->motorCommand("left","forward", "0");
			OSTimeDlyHMSM(0, 0, 5, 0);

		}
}



// ==== GENERAL

void queue_init() {
	ir_queue = OSQCreate((void**)&ir_messages, IR_MAX_MESSAGES);
}

/* The main function creates the LCD task, registers the edge counter polling interrupt,
 * and starts the OS. */
int main(void)
{
	int status = OK;
	// Initialize components.
	queue_init();
	// Create the mc task.
	OSTaskCreateExt(mc_task,
			NULL,
			&mc_task_stk[TASK_STACKSIZE - 1],
			MC_TASK_PRIORITY,
			MC_TASK_PRIORITY,
			mc_task_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	if (status == OK) {
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
