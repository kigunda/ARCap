/*
 * WifiHandler.cpp
 *
 *  Created on: 2014-03-13
 *      Author: Kenan Kigunda
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "altera_avalon_pio_regs.h"

#include "WifiHandler.h"

#define WIFI_ENABLE		1
#define WIFI_RESET		0

#define WRITE_FIFO_EMPTY	0x20
#define READ_FIFO_EMPTY		0x0

#define WIFI_READ_MAX_SIZE				1024
#define WIFI_READ_STOP_MARKER_MAX_SIZE	32
#define WIFI_HTTP_REQUEST_MAX_LENGTH	128

// TASKS

extern WifiHandler *wifi;

/**
 * Starts network communications.
 * This task configures the wifi module and calls WifiHandler::tcpConnect.
 * All other tasks must wait for the connection to be confirmed before executing.
 */
void wifi_handler_tcp_start_task(void *pdata) {
	TASK_LOG(printf("WifiHandler [task: start, status: start]\n"));
	// Start TCP communications.
	wifi->tcpStart();
	TASK_LOG(printf("WifiHandler [task: start, status: finish]\n"));
	// Delay forever.
	while (true) {
		OSTimeDlyHMSM(1, 0, 0, 0);
	}
}

/**
 * Tests network communications.
 * This test sends messages to a PHP echo server and prints the response.
 * Before running this, first start the echo server by visiting 192.168.0.100/arcap/socket.php
 * from a browser on the ARCap network.
 * To view output enable WIFIHANDLER_TCP_DEBUG in Debug.h.
 */
void wifi_handler_tcp_test_task(void *pdata) {
	TASK_LOG(printf("WifiHandler [task: socket test, status: start]\n"));
	// Start TCP communications.
	wifi->tcpStart();
	// Run the test.
	while (true) {
		wifi->tcpSendTest();
		wifi->tcpReceiveTest();
	}
}

/**
 * Test network communications.
 * This test sends a GET request to an HTTP server and prints the response.
 * Before running this, make sure the computer serving /arcap/infrared/hit.php
 * is connecting to the ARCap network.
 * To view output enable WIFIHANDLER_HTTP_DEBUG in Debug.h.
 */
void wifi_handler_http_test_task(void *pdata) {
	TASK_LOG(printf("WifiHandler [task: test, status: start]\n"));
	// Start HTTP communication.
	wifi->httpStart();
	while (true) {
		// Get the test page.
		char *response = wifi->httpGet(HTTP_TEST_PAGE);
		free(response);
		OSTimeDlyHMSM(0, 0, 10, 0);
	}
}

// CONSTRUCTION

/**
 * Creates a new wifi handler.
 * @throw UARTOpenException if the handler cannot open the UART device used to talk to the wifi module
 * @throw SemCreateException if the handler cannot create a semaphore used to control access to the wifi connection
 */
WifiHandler::WifiHandler() {
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_WIFI_RESET_N_BASE, WIFI_ENABLE);
	wifi_dev = alt_up_rs232_open_dev(UART_WIFI_NAME);
	wifi_lock = OSSemCreate(1);
	if (wifi_dev == NULL) {
		throw new UARTOpenException();
	} if (wifi_lock == NULL) {
		throw new SemCreateException();
	}
}

// SYNCHRONIZATION

/**
 * Waits to acquire the lock on the wifi connection.
 * Since we are using transparent mode, there can only be one connection at a time.
 * Methods calling write() or readUntil() must acquire this lock first.
 * @param timeout - the maximum number of ticks that the caller is willing to wait to acquire the lock
 * @return true if the lock was acquired
 */
bool WifiHandler::lockUpTo(INT16U timeout) {
	INT8U status;
	OSSemPend(wifi_lock, timeout, &status);
	return (status == OS_NO_ERR);
}

/**
 * Waits to acquire the lock on the wifi connection.
 * Since we are using transparent mode, there can only be one connection at a time.
 * Methods calling write() or readUntil() must acquire this lock first.
 * @return true if the lock was acquired
 */
bool WifiHandler::lock() {
	return lockUpTo(WIFI_DEFAULT_TIMEOUT);
}

/**
 * Releases the lock on the wifi connection.
 * Must be called at the end of any method calling lock().
 */
void WifiHandler::unlock() {
	OSSemPost(wifi_lock);
}

// STARTUP

/**
 * Prints the current status of the wifi module.
 * The caller must call lock() before calling this method.
 */
void WifiHandler::status() {
	WIFIHANDLER_CONFIG_LOG(printf("WifiHandler [startup: show status]\n"));
	configEnter();       	// Enter command mode.
	configSend("ATVR");  	// Print firmware version.
	configSend("ATID");  	// Print the target SSID.
	configSend("ATEE");  	// Print the security type (0 = None, 1 = WPA, 2 = WPA2, 3 = WEP).
	configSend("ATAH");  	// Print the network type (0 = Joiner, 1 = Creator, 2 = Infrastructure).
	configSend("ATIP");  	// Print the IP protocol (0 = UDP, 1 = TCP).
	configSend("ATTM");		// Read the TCP timeout (hex, multiply by 100 ms).
	configSend("ATMA");  	// Print the IP addressing mode (0 = DHCP, 1 = Static).
	configSend("ATDL");  	// Print the destination IP address. (hex)
	configSend("ATDE");  	// Print the destination port number. (hex)
	waitForReady();
	configSend("ATMY");  	// Print the IP address.
	configSend("ATCN");  	// Exit command mode.
}

/**
 * Sets up the wifi module to connect to the network.
 * The connection settings are stored in non-volatile memory,
 * so this method only needs to be run once per Xbee module.
 * After that, use status() to ensure that the settings are correct.
 * The caller must call lock() before calling this method.
 * @param type
 * WIFI_TCP - used to send TCP messages to PHP socket at 192.168.0.100:10000; preferred for full duplex communication
 * WIFI_HTTP - used to send HTTP requests to HTTP server at 192.168.0.100:80; used in earlier testing
 */
void WifiHandler::setup(WifiSetupType type) {
	WIFIHANDLER_CONFIG_LOG(printf("WifiHandler [startup: do setup]\n"));
	configEnter();
	configSend("ATVR");             	// Print firmware version.
	configSend("ATID"SSID);        		// Set the target SSID.
	configSend("ATEE2");            	// Set the security type to WPA2.
	configSend("ATPK"SSID_KEY);  		// Set the security key.
	configSend("ATAH2");            	// Set the network type to Infrastructure.
	configSend("ATIP1");            	// Set the IP protocol to TCP.
	configSend("ATTMff");				// Set the TCP timeout to 25,500 ms (0xff * 100 ms).
	configSend("ATMA0");            	// Set the IP addressing mode to DHCP.
	configSend("ATDL"SERVER_ADDRESS);	// Set the destination IP address.
	setDestinationPort(type);			// Sets the destination port based on the setup type.
	waitForReady();
	configSend("ATMY");             	// Print the IP address.
	configSend("ATWR");             	// Write the settings to non-volatile memory.
	configSend("ATCN");             	// Exit command mode.
}

/**
 * Sets the destination port based on the setup type.
 * @param type
 * WIFI_TCP - targets the socket at port 10000
 * WIFI_HTTP - targets the server at port 80
 */
void WifiHandler::setDestinationPort(WifiSetupType type) {
	switch (type) {
	case WIFI_TCP: configSend("ATDE"SERVER_TCP_PORT); break;
	case WIFI_HTTP: configSend("ATDE"SERVER_HTTP_PORT); break;
	}
}

/**
 * Tests the UART communications between the board and the wifi module.
 * Sends the configuration command "+++" and expects the response "OK\r".
 */
void WifiHandler::testUart() {
	printf("WifiHandler [test: UART]\n");
	configEnter();
}

// TCP

/**
 * Sets up the wifi module for TCP communication with the server.
 * If the module is already configured for TCP, prints the status of the module.
 * After confirming the module status, calls tcpConnect() to establish a connection to the server socket.
 * @return true if the connection was established
 * This method will also indicate the connection status by turning off the LED.
 */
bool WifiHandler::tcpStart() {
	if (lock()) {
		// Setup or show the wifi status.
//		WIFIHANDLER_CONFIG_LOG(setup(WIFI_TCP));
		WIFIHANDLER_CONFIG_LOG(status());
		// Connect to the server.
		tcpConnect();
		unlock();
		return true;
	} else {
		WIFIHANDLER_TCP_LOG(printf("WifiHandler [task: start, error: failed to acquire lock]\n"));
		return false;
	}
}

/**
 * Connects to the server socket.
 * This method sends the rover identification command r(id), where (id) is ROVER_ID,
 * and waits until it receives MESSAGE_OK.
 * The caller must call lock() before calling this method.
 * @return true if connection was accepted
 */
bool WifiHandler::tcpConnect() {
	char *response = (char *)malloc(WIFI_READ_MAX_SIZE);
	bool retryCount = 0;
	// Loop until we receive MESSAGE_OK.
	do {
		// If retrying, wait.
		if (retryCount > 0) {
			IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, WIFI_LED_WAITING);
			OSTimeDlyHMSM(0, 0, WIFI_CONNECT_RETRY_TIME_SECONDS, 0);
		}
		// Indicate the connection status.
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, WIFI_LED_DISCONNECTED);
		// Send the connection message.
		WIFIHANDLER_TCP_LOG(printf("WifiHandler [connect, send: %s%s]\n", MESSAGE_CONNECT_PREFIX, ROVER_ID));
		write(MESSAGE_CONNECT_PREFIX ROVER_ID MESSAGE_STOP_SUFFIX);
		// Read the connection response.
		readIntoBufferUntil(response, MESSAGE_STOP_SUFFIX);
		WIFIHANDLER_TCP_LOG(printf("[WifiHandler] receive: %s%s", response, MESSAGE_STOP_SUFFIX));
		// Update the retry count.
		retryCount++;
	} while (strncmp(response, MESSAGE_OK, MESSAGE_OK_LENGTH) != 0);
	// Indicate the connection status.
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, WIFI_LED_CONNECTED);
	return true;
}

/**
 * Sends the given TCP message to the server.
 * @param message - the message to send
 * @param timeout - the number of ticks that the caller is willing to wait to use the connection
 * @return true if the message was sent successfully, or false if the message could not be sent
 * because the wifi connection is busy
 */
bool WifiHandler::tcpSend(char *message, INT16U timeout) {
	if (lockUpTo(timeout)) {
		WIFIHANDLER_TCP_LOG(printf("[WifiHandler] send: %s%s", message, MESSAGE_STOP_SUFFIX));
		write(message);
		write(MESSAGE_STOP_SUFFIX);
		unlock();
		return true;
	} else {
		WIFIHANDLER_TCP_LOG(printf("WifiHandler [error: failed to acquire lock]\n"));
		return false;
	}
}

/**
 * Sends the given TCP message to the server and waits for the confirmation MESSAGE_OK.
 * The message will be resent until we receive MESSAGE_OK.
 * @param message - the message to send
 * @param timeout - the number of ticks that the caller is willing to wait to use the connection
 * @return true if the message was sent successfully, or false if the message could not be sent
 * because the wifi connection is busy
 */
bool WifiHandler::tcpSendAndConfirm(char *message, INT16U timeout) {
	if (lockUpTo(timeout)) {
		char *response = (char *)malloc(WIFI_READ_MAX_SIZE);
		do {
			WIFIHANDLER_TCP_LOG(printf("[WifiHandler] send: %s%s", message, MESSAGE_STOP_SUFFIX));
			WIFIHANDLER_TCP_LOG(printf("[WifiHandler] expect: %s%s", MESSAGE_OK, MESSAGE_STOP_SUFFIX));
			write(message);
			write(MESSAGE_STOP_SUFFIX);
			readIntoBufferUntil(response, MESSAGE_STOP_SUFFIX);
			WIFIHANDLER_TCP_LOG(printf("[WifiHandler] receive: %s%s", response, MESSAGE_STOP_SUFFIX));
		} while (strncmp(response, MESSAGE_OK, MESSAGE_OK_LENGTH) != 0);
		unlock();
		return true;
	} else {
		WIFIHANDLER_TCP_LOG(printf("WifiHandler [error: failed to acquire lock]\n"));
		return false;
	}
}

/**
 * Waits for the next TCP message from the server.
 * The message is terminated by a newline '\n'.
 * @return the message received, or NULL if no message could be read because the wifi connection is busy
 */
char *WifiHandler::tcpReceive() {
	if (lock()) {
		char *message = readUntil(MESSAGE_STOP_SUFFIX);
		WIFIHANDLER_TCP_LOG(printf("[WifiHandler] receive: %s", message));
		unlock();
		return message;
	} else {
		WIFIHANDLER_TCP_LOG(printf("WifiHandler [error: failed to acquire lock]\n"));
		return NULL;
	}
}

/**
 * Tests network communications from the rover to the server.
 * Sends a message to the PHP socket server, then prints the response.
 */
void WifiHandler::tcpSendTest() {
	static int count = 1;
	// Send six commands.
	if (count <= 6) {
		if (count < 6) {
			// First five commands are infrared hit.
			wifi->tcpSendAndConfirm("ih", WIFI_DEFAULT_TIMEOUT);
		} else {
			// Last command is quit.
			wifi->tcpSend(":quit", WIFI_DEFAULT_TIMEOUT);
		} count++;
	}
	// Wait for 6 seconds.
	OSTimeDlyHMSM(0, 0, 6, 0);
}

/**
 * Test network communications from the server to the rover.
 * Listens on the socket for new messages and prints them out.
 */
void WifiHandler::tcpReceiveTest() {
	WIFIHANDLER_TCP_LOG(printf("[WifiHandler] receive: %s", tcpReceive()));
	OSTimeDlyHMSM(0, 0, 0, 100);
}

// HTTP

/**
 * Sets up the wifi module for HTTP communication with the server.
 * If the module is already configured for HTTP, prints the status of the module.
 * @return true if the connection was established
 * This method will also indicate the connection status by turning off the LED.
 */
bool WifiHandler::httpStart() {
	if (lock()) {
		// Setup for HTTP or show the wifi status.
		WIFIHANDLER_CONFIG_LOG(setup(WIFI_HTTP));
		//	WIFIHANDLER_CONFIG_LOG(status());
		// Indicate the connection on the LED.
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, WIFI_LED_CONNECTED);
		unlock();
		return true;
	} else {
		WIFIHANDLER_HTTP_LOG(printf("WifiHandler [task: start, error: failed to acquire lock]\n"));
		return false;
	}
}

/**
 * Sends an HTTP GET request for the given URL.
 * @param url - the URL specifying the resource to request
 * @return the response to the request
 */
char *WifiHandler::httpGet(char *url) {
	if (lock()) {
		// Send the GET request.
		char request[WIFI_HTTP_REQUEST_MAX_LENGTH];
		snprintf(request, WIFI_HTTP_REQUEST_MAX_LENGTH,
				"GET %s HTTP/1.1\r\n"
				"Host: %s\r\n"
				"Content-Length: 0\r\n"
				"\r\n", url, SERVER_HOSTNAME);
		write(request);
		WIFIHANDLER_HTTP_LOG(printf("\n%s", request));
		char *response = readUntil("</html>");
		WIFIHANDLER_HTTP_LOG(printf("%s", response));
		unlock();
		return response;
	} else {
		WIFIHANDLER_HTTP_LOG(printf("WifiHandler [error: failed to acquire lock]\n"));
		// Return an empty message.
		char *response = (char *)malloc(sizeof(char));
		response[0] = '\0';
		return response;
	}
}

// SERIAL COMMUNICATIONS >>> WRITING

/**
 * Writes a message to the wifi UART device.
 * The message will be used to configure the Xbee wifi module, if the device is in configuration mode,
 * or it will be forwarded through TCP to the remote server given by DL (the destination IP address)
 * and DE (the destination IP port).
 * @param message - the message to write
 */
void WifiHandler::write(char *message) {
	int i = 0;
	alt_up_rs232_disable_read_interrupt(wifi_dev);
	char data = message[i];
	while (data != '\0') {
		// Check for write space.
		unsigned writeAvailable = alt_up_rs232_get_available_space_in_write_FIFO(wifi_dev);
		WIFIHANDLER_WRITE_LOG(printf("[WifiWrite] available: %u\n", writeAvailable));
		if (writeAvailable > WRITE_FIFO_EMPTY) {
			// If space, write the character.
			WIFIHANDLER_WRITE_LOG(printf("[WifiWrite] data: %c\n", data));
			int status = alt_up_rs232_write_data(wifi_dev, data);
			// Log errors.
			if (status != OK) WIFIHANDLER_WRITE_LOG(printf("[WifiWrite] error: cannot write\n"));
			// Go to next character.
			data = message[++i];
			//			OSTimeDlyHMSM(0, 0, 0, 2); // TODO Check wifi write delay.
		} else {
			// If no space, wait.
			WIFIHANDLER_WRITE_LOG(printf("[WifiWrite] waiting for space\n"));
			//			OSTimeDlyHMSM(0, 0, 0, 100);
		}
	}
	alt_up_rs232_enable_read_interrupt(wifi_dev);
}

// SERIAL COMMUNICATIONS >>> READING

/**
 * Indicates whether the string "whole", of length "wholeLength", ends with
 * the substring given by "end", of length "endLength".
 * @return true if the whole string is longer than the end string and the last
 * endLength characters of the whole string are the same as the end string.
 */
static bool stringEndsWith(char *whole, int wholeLength, char *end, int endLength) {
	if (wholeLength < endLength) {
		return false;
	} else {
		return strncmp(whole + wholeLength - endLength, end, endLength) == 0;
	}
}


/**
 * Indicates whether a message of the given length can fit in the read buffer.
 * @param messageLength - the length to test against the buffer size
 * @return true if the message length, plus one for the terminating null byte,
 * is less than the size of the buffer.
 */
static bool inRange(int messageLength) {
	bool inRange = messageLength < WIFI_READ_MAX_SIZE - 1;
	WIFIHANDLER_READ_LOG(if (!inRange) printf("[WifiRead] error: out of buffer space\n"));
	return inRange;
}

/**
 * Indicates whether the given number of retries is under the maximum number of retries.
 * @return true if there is room for more retries
 */
static bool inRetryRange(int retryCount) {
	bool inRetryRange = retryCount < WIFI_READ_AVAILABLE_RETRIES;
	WIFIHANDLER_READ_LOG(if (!inRetryRange) printf("[WifiRead] error: too many retries\n"));
	return inRetryRange;
}

/**
 * Indicates whether there is data to read.
 * @return true if there is data in the read buffer
 */
bool WifiHandler::hasData() {
	return alt_up_rs232_get_used_space_in_read_FIFO(wifi_dev) > READ_FIFO_EMPTY;
}

/**
 * Listens on the wifi UART until a full message is received.
 * @param message - the buffer in which the message will be placed
 * @param stop - the string marking the end of the message
 */
void WifiHandler::readIntoBufferUntil(char *message, char *stop) {
	alt_u8 data, parity;
	unsigned readAvailable;
	// Get the stop marker length.
	int stopLength = strnlen(stop, WIFI_READ_STOP_MARKER_MAX_SIZE);
	WIFIHANDLER_READ_LOG(printf("[WifiRead] stop: %s\n", stop));
	// Start with an empty message.
	int messageLength = 0;
	// Count the number of retries when there is no data available.
	int retryCount = 0;
	// Read until the message buffer matches the stop string, or we run out of space, or there are to many retries.
	while (inRange(messageLength) && inRetryRange(retryCount) &&
			!stringEndsWith(message, messageLength, stop, stopLength)) {
		// Check for next character.
		readAvailable = alt_up_rs232_get_used_space_in_read_FIFO(wifi_dev);
		WIFIHANDLER_READ_LOG(printf("[WifiRead] available: %u\n", readAvailable));
		// Is there data?
		if (readAvailable <= READ_FIFO_EMPTY) {
			// No data; retry.
			retryCount++;
		} else {
			// Reset the retry count.
			retryCount = 0;
			// Get the new data.
			while (readAvailable > READ_FIFO_EMPTY) {
				// Read next character.
				int status = alt_up_rs232_read_data(wifi_dev, &data, &parity);
				// Log errors.
				if (status != OK) WIFIHANDLER_READ_LOG(printf("[WifiRead] error: cannot read\n"));
				// Add character to the message buffer.
				WIFIHANDLER_READ_LOG(printf("[WifiRead] data: %c\n", data));
				message[messageLength] = (char)data;
				messageLength++;
				// Check for next character.
				readAvailable = alt_up_rs232_get_used_space_in_read_FIFO(wifi_dev);
			} WIFIHANDLER_READ_LOG(printf("[WifiRead] length: %d\n", messageLength));
		}
	}
	// Add the null character.
	message[messageLength] = '\0';
}

/**
 * Listens on the wifi UART until a full message is received.
 * @param stop - the string marking the end of the message
 * @return the message that was received, which must be freed by the caller
 */
char *WifiHandler::readUntil(char *stop) {
	char *message = (char *)malloc(WIFI_READ_MAX_SIZE);
	readIntoBufferUntil(message, stop);
	return message;
}

// CONFIGURATION

/**
 * Reads the latest configuration response from the Xbee wifi module.
 * @return the message object containing the response
 */
char *WifiHandler::configReadStart() {
	char *response = readUntil("\r");
	WIFIHANDLER_CONFIG_LOG(printf("%s\n", response));
	return response;
}

/**
 * Reads the latest configuration response from the Xbee wifi module.
 */
void WifiHandler::configRead() {
	char *response = configReadStart();
	free(response);
}

/**
 * Sends a configuration command to the Xbee wifi module.
 * @param command - the command to send
 */
void WifiHandler::configSendStart(char *command) {
	write(command);
	write("\r");
	WIFIHANDLER_CONFIG_LOG(printf("%s > ", command));
}

/**
 * Sends a configuration command to the Xbee wifi module,
 * then waits for and prints the response.
 * @param command - the command to send
 */
void WifiHandler::configSend(char *command) {
	configSendStart(command);
	configRead();
}

/**
 * Sends the sequence to enter configuration mode on the Xbee wifi module.
 */
void WifiHandler::configEnter() {
	OSTimeDlyHMSM(0, 0, 1, 0);
	write("+++");
	WIFIHANDLER_CONFIG_LOG(printf("+++ > "));
	OSTimeDlyHMSM(0, 0, 2, 0);
	configRead();
}

/**
 * Indicates whether the wifi module has connected to the network.
 * @return true if the wifi module is connected
 */
bool WifiHandler::ready() {
	configSendStart("ATAI");
	char *response = configReadStart();
	long code = strtol(response, NULL, 16);
	free(response);
	return code == 0;
}

/**
 * Blocks until the wifi module has connected to the network.
 */
void WifiHandler::waitForReady() {
	while (!ready()) {
		OSTimeDlyHMSM(0, 0, 0, 100);
	};
}
