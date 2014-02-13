/*************************************************************************
 * Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
 * All rights reserved. All use of this software and documentation is     *
 * subject to the License Agreement located at the end of this file below.*
 **************************************************************************
 * Description:                                                           *
 * The following is a simple hello world program running MicroC/OS-II.The *
 * purpose of the design is to be a very simple application that just     *
 * demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
 * for issues such as checking system call return codes. etc.             *
 *                                                                        *
 * Requirements:                                                          *
 *   -Supported Example Hardware Platforms                                *
 *     Standard                                                           *
 *     Full Featured                                                      *
 *     Low Cost                                                           *
 *   -Supported Development Boards                                        *
 *     Nios II Development Board, Stratix II Edition                      *
 *     Nios Development Board, Stratix Professional Edition               *
 *     Nios Development Board, Stratix Edition                            *
 *     Nios Development Board, Cyclone Edition                            *
 *   -System Library Settings                                             *
 *     RTOS Type - MicroC/OS-II                                           *
 *     Periodic System Timer                                              *
 *   -Know Issues                                                         *
 *     If this design is run on the ISS, terminal output will take several*
 *     minutes per iteration.                                             *
 **************************************************************************/


#include <stdio.h>
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "alt_types.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    ir_task_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define IR_TASK_PRIORITY	1

/* Message queue */
#define MAX_MESSAGES 4
#define WAIT_FOREVER 0
int messages[MAX_MESSAGES];
OS_EVENT *queue;

/* Messages */
// Together these avoid passing 0 to the queue (which is not allowed since queue messages cannot be null)
// and invert pushbutton input (since the pushbutton returns 0 when it is pushed down).
#define IR_QUEUE_SEND_BASE	1		// Use: IR_QUEUE_SEND_BASE + IORD
#define IR_QUEUE_RECEIVE_BASE 2		// Use: IR_QUEUE_RECEIVE_BASE - OSQPend

/* Error status */
#define OK 0

/* Interrupt service routine triggered whenever the status of the IR emitter pushbutton changes. */
static void isr_on_ir_pushbutton(void * context) {
	// Read the state of the pushbutton and post it to the queue.
	int message = IR_QUEUE_SEND_BASE + IORD_ALTERA_AVALON_PIO_DATA(PIO_KEY_LEFT_BASE);
	OSQPost(queue, (void*)message);
	// Mask to mark the end of the ISR.
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_KEY_LEFT_BASE, PIO_KEY_LEFT_BIT_CLEARING_EDGE_REGISTER);
}

/* Controllers the IR emitter based on the value of the pushbutton. */
void ir_task(void* pdata)
{
	INT8U err;
	while (1)
	{
		// Read the value from the queue.
		int status = IR_QUEUE_RECEIVE_BASE - (int)OSQPend(queue, WAIT_FOREVER, &err);
		if (err == OS_NO_ERR) {
			// Print the result and send it to the emitter.
			printf("IR: %d\n", status);
			IOWR_ALTERA_AVALON_PIO_DATA(IR_EMITTER_BASE, status);
		}
	}
}

void queue_init() {
	queue = OSQCreate((void**)&messages, MAX_MESSAGES);
}

/* The main function creates the LCD task, registers the edge counter polling interrupt,
 * and starts the OS. */
int main(void)
{
	int status;

	// Initialize components.
	queue_init();

	// Create the IR task.
	OSTaskCreateExt(ir_task,
			NULL,
			(void *)&ir_task_stk[TASK_STACKSIZE - 1],
			IR_TASK_PRIORITY,
			IR_TASK_PRIORITY,
			ir_task_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	// Register the IR pushbutton interrupt.
	status = alt_ic_isr_register(PIO_KEY_LEFT_IRQ_INTERRUPT_CONTROLLER_ID,
				PIO_KEY_LEFT_IRQ,
				isr_on_ir_pushbutton,
				NULL,
				NULL);

	// Enable key interrupts.
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_KEY_LEFT_BASE, PIO_KEY_LEFT_CAPTURE);

	// Start.
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
