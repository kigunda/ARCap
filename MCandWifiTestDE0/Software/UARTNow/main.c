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


#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "altera_avalon_uart.h"
#include "altera_avalon_uart_regs.h"
#include "includes.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

// GENERAL UART

void sendToggle() {
	printf("Sending 0x%2x\n", 0x55);
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_MC_BASE, 0x55);
}

// UART TO WIFI

#define WIFI_GUARD_TIME_SECONDS 1
#define WIFI_READ_BUFFER_SIZE	1024

typedef struct {
	char buffer[WIFI_READ_BUFFER_SIZE];
	int mark;
} WifiMessage;

WifiMessage networkIn;

#define WAIT_FOREVER 0

#define WIFI_QUEUE_BUFFER_SIZE 4
int wifiQueueBuffer[WIFI_QUEUE_BUFFER_SIZE];
OS_EVENT *wifiQueue;

void wifiMessageAppend(WifiMessage *message, char c) {
	if (message->mark < WIFI_READ_BUFFER_SIZE) {
		message->buffer[message->mark] = c;
		message->mark++;
	}
}

void wifiMessagePrint(WifiMessage *message) {
	printf("%s\n", message->buffer);
}

void wifiMessageClear(WifiMessage *message) {
	memset(&message, 0, sizeof(WifiMessage));
}

void uart_isr(void *context, alt_u32 id) {
//	WifiMessage *message = context;
//	alt_u32 status = IORD_ALTERA_AVALON_UART_STATUS(UART_MC_BASE);
//	if (status & ALTERA_AVALON_UART_STATUS_RRDY_MSK) {
//		wifiMessageAppend(message, (char)IORD_ALTERA_AVALON_UART_RXDATA(UART_MC_BASE));
//		status = IORD_ALTERA_AVALON_UART_STATUS(UART_MC_BASE);
//		OSQPost(wifiQueue, (void *)message);
//	} IOWR_ALTERA_AVALON_UART_STATUS(UART_MC_BASE, 0);
}

char readFromUart() {
	return IORD_ALTERA_AVALON_UART_RXDATA(UART_MC_BASE);
}

void writeToUart(char *data, int length) {
	int i;
	for (i = 0; i < length; i++) {
		IOWR_ALTERA_AVALON_UART_TXDATA(UART_MC_BASE, data[i]);
		OSTimeDlyHMSM(0, 0, 0, 2);
	}
}

void waitXbeeGuardTime() {
	OSTimeDlyHMSM(0, 0, WIFI_GUARD_TIME_SECONDS, 0);
}

void sendXbeeConfigureStart() {
	char *message = "+++";
	int length = 3;
	waitXbeeGuardTime();
	writeToUart(message, length);
	waitXbeeGuardTime();
	printf("%s > ", message);
}

void uart_write_task(void* pdata) {
	printf("UART Write Task started\n");
	while (true) {
		sendXbeeConfigureStart();
		OSTimeDlyHMSM(0, 0, 10, 0);
	}
}

void uart_read_task(void *pdata) {
	printf("UART Read Task started\n");
	INT8U err;
	while (true) {
		WifiMessage *message = OSQPend(wifiQueue, WAIT_FOREVER, &err);
		if (err == OS_NO_ERR) {
			wifiMessagePrint(message);
			wifiMessageClear(message);
		}
	}
}

// MAIN

/* The main function creates two task and starts multi-tasking */
int main(void)
{
	OSTaskCreateExt(uart_write_task,
			NULL,
			(void *)&task1_stk[TASK_STACKSIZE-1],
			TASK1_PRIORITY,
			TASK1_PRIORITY,
			task1_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	OSTaskCreateExt(uart_read_task,
			NULL,
			(void *)&task2_stk[TASK_STACKSIZE-1],
			TASK2_PRIORITY,
			TASK2_PRIORITY,
			task2_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	wifiQueue = OSQCreate((void**)wifiQueueBuffer, WIFI_QUEUE_BUFFER_SIZE);

	WifiMessage wifiMessage;
	memset(&wifiMessage, 0, sizeof(wifiMessage));
	alt_irq_register(UART_MC_IRQ_INTERRUPT_CONTROLLER_ID, (void *)&wifiMessage, uart_isr);

	printf("Main started\n");

	OSStart();
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
