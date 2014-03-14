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


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "altera_up_avalon_rs232.h"
#include "includes.h"

//#define DEBUG
//#define WRITE_DEBUG
//#define READ_DEBUG

#if defined(WRITE_DEBUG) || defined(DEBUG)
#define WRITE_LOG(info) info
#else
#define WRITE_LOG(info)
#endif

#if defined(READ_DEBUG) || defined(DEBUG)
#define READ_LOG(info) info
#else
#define READ_LOG(info)
#endif

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

#define WRITE_FIFO_EMPTY	0x20
#define READ_FIFO_EMPTY		0x0

#define WIFI_READ_MAX_SIZE	1024
#define WIFI_READ_STOP_MARKER_MAX_SIZE	32

#define WIFI_HTTP_REQUEST_MAX_LENGTH	128

typedef struct WifiMessage {
	char content[WIFI_READ_MAX_SIZE];
	int length;
} WifiMessage;

alt_up_rs232_dev *wifi_dev;

/**
 * Indicates whether the string "whole", of length "wholeLength", ends with
 * the substring given by "end", of length "endLength".
 * @return true if the whole string is longer than the end string and the last
 * endLength characters of the whole string are the same as the end string.
 */
bool stringEndsWith(char *whole, int wholeLength, char *end, int endLength) {
	if (wholeLength < endLength) {
		return false;
	} else {
		return strncmp(whole + wholeLength - endLength, end, endLength) == 0;
	}
}

/*
 * Clears the given wifi message.
 * @param message - the message object to clear
 */
void wifiMessageClear(WifiMessage *message) {
	memset((void *)message, 0, sizeof(WifiMessage));
}

/*
 * Writes a message to the wifi UART device.
 * The message will be used to configure the Xbee wifi module, if the device is in configuration mode,
 * or it will be forwarded through TCP to the remote server given by DL (the destination IP address)
 * and DE (the destination IP port).
 * @param message - the message to write
 */
void wifiWrite(char *message) {
	int i = 0;
	alt_up_rs232_disable_read_interrupt(wifi_dev);
	char data = message[i];
	while (data != '\0') {
		// Check for write space.
		unsigned writeAvailable = alt_up_rs232_get_available_space_in_write_FIFO(wifi_dev);
		WRITE_LOG(printf("[WifiWrite] available: %u\n", writeAvailable));
		if (writeAvailable > WRITE_FIFO_EMPTY) {
			// If space, write the character.
			WRITE_LOG(printf("[WifiWrite] data: %c\n", data));
			alt_up_rs232_write_data(wifi_dev, data);
			data = message[++i];
			OSTimeDlyHMSM(0, 0, 0, 2);
		} else {
			// If no space, wait.
			WRITE_LOG(printf("[WifiWrite] waiting for space\n"));
			OSTimeDlyHMSM(0, 0, 0, 100);
		}
	}
	alt_up_rs232_enable_read_interrupt(wifi_dev);
}

/*
 * Listens on the wifi UART until a full message is received.
 * @param stop - the string marking the end of the message
 * @return the message that was received, which must be freed by the caller
 */
WifiMessage *wifiReadUntil(char *stop) {
	alt_u8 data, parity;
	unsigned readAvailable;
	int stopLength = strnlen(stop, WIFI_READ_STOP_MARKER_MAX_SIZE);
	READ_LOG(printf("[WifiRead] stop: %s\n", stop));
	// Create a new, empty message.
	WifiMessage *message = malloc(sizeof(*message));
	wifiMessageClear(message);
	// Read until the message buffer matches the stop string.
	while (!stringEndsWith(message->content, message->length, stop, stopLength)) {
		// Check for next character.
		readAvailable = alt_up_rs232_get_used_space_in_read_FIFO(wifi_dev);
		READ_LOG(printf("[WifiRead] available: %u\n", readAvailable));
		while (readAvailable > READ_FIFO_EMPTY) {
			// Read next character into the message buffer.
			alt_up_rs232_read_data(wifi_dev, &data, &parity);
			READ_LOG(printf("[WifiRead] data: %c\n", data));
			message->content[message->length] = (char)data;
			message->length++;
			// Check for next character.
			readAvailable = alt_up_rs232_get_used_space_in_read_FIFO(wifi_dev);
		}
	}
	// Return the message length;
	return message;
}

/*
 * Reads the latest configuration response from the Xbee wifi module.
 * @return the message object containing the response
 */
WifiMessage *wifiConfigStartRead() {
	WifiMessage *response = wifiReadUntil("\r");
	printf("%s\n", response->content);
	return response;
}

/*
 * Reads the latest configuration response from the Xbee wifi module.
 */
void wifiConfigRead() {
	WifiMessage *response = wifiConfigStartRead();
	free(response);
}

/**
 * Sends the sequence to enter configuration mode on the Xbee wifi module.
 */
void wifiConfigEnter() {
	OSTimeDlyHMSM(0, 0, 1, 0);
	wifiWrite("+++");
	printf("+++ > ");
	OSTimeDlyHMSM(0, 0, 2, 0);
	wifiConfigRead();
}

/*
 * Sends a configuration command to the Xbee wifi module.
 * @param command - the command to send
 */
void wifiConfigStartSend(char *command) {
	wifiWrite(command);
	wifiWrite("\r");
	printf("%s > ", command);
}

/*
 * Sends a configuration command to the Xbee wifi module,
 * then waits for and prints the response.
 * @param command - the command to send
 */
void wifiConfigSend(char *command) {
	wifiConfigStartSend(command);
	wifiConfigRead();
}

/*
 * Indicates whether the wifi module has connected to the network.
 * @return true if the wifi module is connected
 */
bool isWifiReady() {
	wifiConfigStartSend("ATAI");
	WifiMessage *response = wifiConfigStartRead();
	long code = strtol(response->content, NULL, 16);
	free(response);
	return code == 0;
}

/*
 * Blocks until the wifi module has connected to the network.
 */
void waitForWifiReady() {
	while (!isWifiReady()) {
		OSTimeDlyHMSM(0, 0, 0, 100);
	};
}

/*
 * Prints the current status of the wifi module.
 */
void wifiStatus() {
	printf("[WifiStatus]\n");
	wifiConfigEnter();       	// Enter command mode.
	wifiConfigSend("ATVR");  	// Print firmware version.
	wifiConfigSend("ATID");  	// Print the target SSID.
	wifiConfigSend("ATEE");  	// Print the security type (0 = None, 1 = WPA, 2 = WPA2, 3 = WEP).
	wifiConfigSend("ATAH");  	// Print the network type (0 = Joiner, 1 = Creator, 2 = Infrastructure).
	wifiConfigSend("ATIP");  	// Print the IP protocol (0 = UDP, 1 = TCP).
	wifiConfigSend("ATMA");  	// Print the IP addressing mode (0 = DHCP, 1 = Static).
	wifiConfigSend("ATDL");  	// Print the destination IP address.
	wifiConfigSend("ATDE");  	// Print the destination port number.
	waitForWifiReady();
	wifiConfigSend("ATMY");  	// Print the IP address.
	wifiConfigSend("ATCN");  	// Exit command mode.
}

/*
 * Sets the wifi module up to connect to the network.
 */
void wifiConnect() {
	wifiConfigEnter();
	wifiConfigSend("ATVR");             	// Print firmware version.
	wifiConfigSend("ATIDARCap");        	// Set the target SSID to ARCAP.
	wifiConfigSend("ATEE2");            	// Set the security type to WPA2.
	wifiConfigSend("ATPKplaythegame");  	// Set the security key.
	wifiConfigSend("ATAH2");            	// Set the network type to Infrastructure.
	wifiConfigSend("ATIP1");            	// Set the IP protocol to TCP.
	wifiConfigSend("ATMA0");            	// Set the IP addressing mode to DHCP.
	wifiConfigSend("ATDL192.168.0.100");	// Set the destination IP address to ...100 (reserved for server).
	wifiConfigSend("ATDE50");           	// Set the destination port to port 80 (0x50).
	waitForWifiReady();
	wifiConfigSend("ATMY");             	// Print the IP address.
	wifiConfigSend("ATWR");             	// Write the settings to non-volatile memory.
	wifiConfigSend("ATCN");             	// Exit command mode.
}

void wifiHttpGet(char *url) {
	char request[WIFI_HTTP_REQUEST_MAX_LENGTH];
	snprintf(request, WIFI_HTTP_REQUEST_MAX_LENGTH,
			"GET %s HTTP/1.1\r\n"
			"Host: surface\r\n"
			"Content-Length: 0\r\n"
			"\r\n", url);
	wifiWrite(request);
	printf("\n%s", request);
}

void wifiHttpRead() {
	WifiMessage *response = wifiReadUntil("</html>");
	printf("%s\n", response->content);
	free(response);
}

/* Communicates with the Xbee wifi module. */
void wifi_task(void *pdata) {
	OSTimeDlyHMSM(0, 0, 2, 0);
	wifiStatus();
	while (true) {
		wifiHttpGet("/arcap/infrared/hit.php");
		wifiHttpRead();
		OSTimeDlyHMSM(0, 0, 10, 0);
	}
}

/* The main function creates tasks and starts multi-tasking */
int main(void)
{

	// Register tasks.
	OSTaskCreateExt(wifi_task,
			NULL,
			(void *)&task1_stk[TASK_STACKSIZE-1],
			TASK1_PRIORITY,
			TASK1_PRIORITY,
			task1_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	// Open wifi UART device.
	wifi_dev = alt_up_rs232_open_dev(UART_WIFI_NAME);
	if (wifi_dev != NULL) {
		printf("Opened wifi UART device\n");
		// Start.
		OSStart();
	} else {
		printf("Could not open wifi UART device\n");
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
