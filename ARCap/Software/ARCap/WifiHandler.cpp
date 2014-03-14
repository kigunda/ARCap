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

#include "WifiHandler.h"

#define WRITE_FIFO_EMPTY	0x20
#define READ_FIFO_EMPTY		0x0

#define WIFI_READ_MAX_SIZE	1024
#define WIFI_READ_STOP_MARKER_MAX_SIZE	32

#define WIFI_HTTP_REQUEST_MAX_LENGTH	128

typedef struct WifiMessage {
	char content[WIFI_READ_MAX_SIZE];
	int length;
} WifiMessage;

// ALLOCATION
WifiHandler::WifiHandler() {}
WifiHandler::~WifiHandler() {}

// INITIALIZATION

/*
 * Initializes this handler.
 * @return OK if there are no initialization errors
 */
Status WifiHandler::init() {
	// Open the wifi UART device.
	wifi_dev = alt_up_rs232_open_dev(UART_WIFI_NAME);
	lock = OSSemCreate(1);
	if ((wifi_dev == NULL) || (lock == NULL)) {
		return ERR_WIFI;
	} else {
//		// Print the wifi status.
//		status();
		return OK;
	}
}

// SERIAL COMMUNICATIONS

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
 * Prints the given wifi message.
 * @param message - the message object to print
 */
void wifiMessagePrint(WifiMessage *message) {
	printf("%s\n", message->content);
}

/*
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
			alt_up_rs232_write_data(wifi_dev, data);
			data = message[++i];
			OSTimeDlyHMSM(0, 0, 0, 2);
		} else {
			// If no space, wait.
			WIFIHANDLER_WRITE_LOG(printf("[WifiWrite] waiting for space\n"));
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
WifiMessage *WifiHandler::readUntil(char *stop) {
	alt_u8 data, parity;
	unsigned readAvailable;
	int stopLength = strnlen(stop, WIFI_READ_STOP_MARKER_MAX_SIZE);
	WIFIHANDLER_READ_LOG(printf("[WifiRead] stop: %s\n", stop));
	// Create a new, empty message.
	WifiMessage *message = (WifiMessage *)malloc(sizeof(*message));
	wifiMessageClear(message);
	// Read until the message buffer matches the stop string.
	while (!stringEndsWith(message->content, message->length, stop, stopLength)) {
		// Check for next character.
		readAvailable = alt_up_rs232_get_used_space_in_read_FIFO(wifi_dev);
		WIFIHANDLER_READ_LOG(printf("[WifiRead] available: %u\n", readAvailable));
		while (readAvailable > READ_FIFO_EMPTY) {
			// Read next character into the message buffer.
			alt_up_rs232_read_data(wifi_dev, &data, &parity);
			WIFIHANDLER_READ_LOG(printf("[WifiRead] data: %c\n", data));
			message->content[message->length] = (char)data;
			message->length++;
			// Check for next character.
			readAvailable = alt_up_rs232_get_used_space_in_read_FIFO(wifi_dev);
		} WIFIHANDLER_READ_LOG(printf("[WifiRead] length: %d\n", message->length));
	}
	// Return the message length;
	return message;
}

// CONFIGURATION

/*
 * Reads the latest configuration response from the Xbee wifi module.
 * @return the message object containing the response
 */
WifiMessage *WifiHandler::configReadStart() {
	WifiMessage *response = readUntil("\r");
	printf("%s\n", response->content);
	return response;
}

/*
 * Reads the latest configuration response from the Xbee wifi module.
 */
void WifiHandler::configRead() {
	WifiMessage *response = configReadStart();
	free(response);
}

/*
 * Sends a configuration command to the Xbee wifi module.
 * @param command - the command to send
 */
void WifiHandler::configSendStart(char *command) {
	write(command);
	write("\r");
	printf("%s > ", command);
}

/*
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
	printf("+++ > ");
	OSTimeDlyHMSM(0, 0, 2, 0);
	configRead();
}

/*
 * Indicates whether the wifi module has connected to the network.
 * @return true if the wifi module is connected
 */
bool WifiHandler::ready() {
	configSendStart("ATAI");
	WifiMessage *response = configReadStart();
	long code = strtol(response->content, NULL, 16);
	free(response);
	return code == 0;
}

/*
 * Blocks until the wifi module has connected to the network.
 */
void WifiHandler::waitForReady() {
	while (!ready()) {
		OSTimeDlyHMSM(0, 0, 0, 100);
	};
}

/*
 * Prints the current status of the wifi module.
 */
void WifiHandler::status() {
	printf("WifiHandler [startup: show status]\n");
	configEnter();       	// Enter command mode.
	configSend("ATVR");  	// Print firmware version.
	configSend("ATID");  	// Print the target SSID.
	configSend("ATEE");  	// Print the security type (0 = None, 1 = WPA, 2 = WPA2, 3 = WEP).
	configSend("ATAH");  	// Print the network type (0 = Joiner, 1 = Creator, 2 = Infrastructure).
	configSend("ATIP");  	// Print the IP protocol (0 = UDP, 1 = TCP).
	configSend("ATMA");  	// Print the IP addressing mode (0 = DHCP, 1 = Static).
	configSend("ATDL");  	// Print the destination IP address.
	configSend("ATDE");  	// Print the destination port number.
	waitForReady();
	configSend("ATMY");  	// Print the IP address.
	configSend("ATCN");  	// Exit command mode.
}

/*
 * Sets the wifi module up to connect to the network.
 */
void WifiHandler::setup() {
	printf("WifiHandler [startup: do setup]\n");
	configEnter();
	configSend("ATVR");             	// Print firmware version.
	configSend("ATIDARCap");        	// Set the target SSID to ARCAP.
	configSend("ATEE2");            	// Set the security type to WPA2.
	configSend("ATPKplaythegame");  	// Set the security key.
	configSend("ATAH2");            	// Set the network type to Infrastructure.
	configSend("ATIP1");            	// Set the IP protocol to TCP.
	configSend("ATMA0");            	// Set the IP addressing mode to DHCP.
	configSend("ATDL192.168.0.100");	// Set the destination IP address to ...100 (reserved for server).
	configSend("ATDE50");           	// Set the destination port to port 80 (0x50).
	waitForReady();
	configSend("ATMY");             	// Print the IP address.
	configSend("ATWR");             	// Write the settings to non-volatile memory.
	configSend("ATCN");             	// Exit command mode.
}

// HTTP

/*
 * Sends an HTTP GET request for the given URL.
 * @param url - the URL specifying the resource to request
 * @return the response to the request
 */
WifiMessage *WifiHandler::httpGet(char *url) {
	INT8U err;
	OSSemPend(lock, WIFI_HANDLER_LOCK_TIMEOUT_TICKS, &err);
	if (err == OS_NO_ERR) {
		// Send the GET request.
		char request[WIFI_HTTP_REQUEST_MAX_LENGTH];
		snprintf(request, WIFI_HTTP_REQUEST_MAX_LENGTH,
				"GET %s HTTP/1.1\r\n"
				"Host: surface\r\n"
				"Content-Length: 0\r\n"
				"\r\n", url);
		write(request);
		WIFIHANDLER_HTTP_LOG(printf("\n%s", request));
		WifiMessage *response = readUntil("</html>");
		OSSemPost(lock);
		return response;
	} else {
		// Return an empty message.
		return (WifiMessage *)malloc(sizeof(WifiMessage));
	}
}

/*
 * Blocks and waits for an HTTP request from remote clients.
 * @return the request
 */
WifiMessage *WifiHandler::httpConnect() {
	unsigned char err;
	OSSemPend(lock, WIFI_HANDLER_LOCK_TIMEOUT_TICKS, &err);
	if (err == OS_NO_ERR) {
		// Wait for an HTTP request.
		WifiMessage *response = readUntil("\r\n\r\n");
		OSSemPost(lock);
		return response;
	} else {
		// Return an empty message.
		return (WifiMessage *)malloc(sizeof(WifiMessage));
	}
}
