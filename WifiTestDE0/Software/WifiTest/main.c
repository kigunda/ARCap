/*************************************************************************
 * ARCap
 * Amshu Gongal, Kenan Kigunda
 * February 18, 2014
 **************************************************************************
 * Description:                                                           *
 * Prototype testing for infrared and wifi.
 **************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_uart_regs.h"
#include "sys/alt_irq.h"
#include "alt_types.h"

/* Task stacks */
#define   TASK_STACKSIZE       2048
OS_STK    ir_task_stk[TASK_STACKSIZE];
OS_STK    wifi_task_stk[TASK_STACKSIZE];

/* Task priorities */
#define IR_TASK_PRIORITY	1
#define WIFI_TASK_PRIORITY	2

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

/* Interrupt service routine triggered whenever the status of the IR emitter pushbutton changes. */
static void isr_on_ir_pushbutton(void * context) {
	// Read the state of the pushbutton and post it to the queue.
	//printf("Pressed\n");
	int message = IR_QUEUE_SEND_BASE + IORD_ALTERA_AVALON_PIO_DATA(PIO_KEY_LEFT_BASE);
	OSQPost(ir_queue, (void*)message);
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
		int status = IR_QUEUE_RECEIVE_BASE - (int)OSQPend(ir_queue, WAIT_FOREVER, &err);
		if (err == OS_NO_ERR) {
			// Print the result and send it to the emitter.
			printf("IR: %d\n", status);
			IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, status);
		}
	}
}

// ==== WIFI

#define WIFI_GUARD_TIME 1

void wifi_wait() {
	OSTimeDlyHMSM(0, 0, WIFI_GUARD_TIME, 0);
}

void wifi_write(char *message, int length) {
	int i;
	for (i = 0; i < length; i++) {
		IOWR_ALTERA_AVALON_UART_TXDATA(UART_WIFI_BASE, message[i]);
	}
}

void wifi_read() {
	char c = IORD_ALTERA_AVALON_UART_RXDATA(UART_WIFI_BASE);
	printf("Wifi: %c\n", c);
}

void wifi_task(void *pdata)
{
	printf("Started wifi task\n");
	wifi_wait();
	wifi_write("x", 1);
	wifi_wait();
	wifi_write("+++", 3);
	while (true) {
		wifi_wait();
		wifi_read();
	}
}

// WIFI FILE STUFF

#define READ_BUFFER_SIZE 2

FILE *wifi;

void wifi_write_task(void *pdata) {
	printf("Started wifi write task");
	fputs("+++", wifi);
}

void wifi_read_task(void *pdata) {
	printf("Started wifi read task");
	char buffer[READ_BUFFER_SIZE];
	fgets(buffer, READ_BUFFER_SIZE, wifi);
	printf("%s", buffer);
}

// ==== GENERAL

void queue_init() {
	ir_queue = OSQCreate((void**)&ir_messages, IR_MAX_MESSAGES);
}

/* The main function creates the LCD task, registers the edge counter polling interrupt,
 * and starts the OS. */
int main(void)
{
	int status;

	// Initialize components.
	queue_init();

	// Create tasks.
	OSTaskCreateExt(wifi_write_task,
			NULL,
			(void *)&ir_task_stk[TASK_STACKSIZE - 1],
			IR_TASK_PRIORITY,
			IR_TASK_PRIORITY,
			ir_task_stk,
			TASK_STACKSIZE,
			NULL,
			0);
	OSTaskCreateExt(wifi_read_task,
			NULL,
			(void *)&wifi_task_stk[TASK_STACKSIZE - 1],
			WIFI_TASK_PRIORITY,
			WIFI_TASK_PRIORITY,
			wifi_task_stk,
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

	// Open wifi.
	wifi = fopen(UART_WIFI_NAME, "rw");

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
