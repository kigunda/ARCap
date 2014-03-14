/*
 * WifiHandler.h
 *
 *  Created on: 2014-03-13
 *      Author: Kenan Kigunda
 */

#ifndef WIFIHANDLER_H_
#define WIFIHANDLER_H_

#include "altera_up_avalon_rs232.h"
#include "includes.h"

#include "Debug.h"
#include "Status.h"

/*
 * Indicates how long callers will wait to use a busy connection.
 */
#define WIFI_HANDLER_LOCK_TIMEOUT_TICKS	100

struct WifiMessage;
void wifiMessagePrint(WifiMessage *message);

/**
 * The wifi handler provides a high-level view of the interfacing to the Xbee wifi module.
 */
class WifiHandler {
public:
	WifiHandler();
	virtual ~WifiHandler();

	/*
	 * Initializes this handler.
	 * @return OK if there are no initialization errors
	 */
	Status init();

	/*
	 * Sets the wifi module up to connect to the network.
	 * The connection settings are stored in non-volatile memory,
	 * so this method only needs to be run once per Xbee module.
	 * After that, use status() to ensure that the settings are correct.
	 */
	void setup();

	/*
	 * Prints the current status of the wifi module.
	 */
	void status();

	/*
	 * Sends an HTTP GET request for the given URL.
	 * @param url - the URL specifying the resource to request
	 * @return the response to the request
	 */
	WifiMessage *httpGet(char *url);

	/*
	 * Blocks and waits for an HTTP request from remote clients.
	 * @return the request
	 */
	WifiMessage *httpConnect();

private:
	/* The UART device used to write and read data to and from the Xbee wifi module. */
	alt_up_rs232_dev *wifi_dev;

	/* The semaphore used to ensure that multiple requests are not made simultaneously to the same connection. */
	OS_EVENT *lock;

	/*
	 * Writes a message to the wifi UART device.
	 * The message will be used to configure the Xbee wifi module, if the device is in configuration mode,
	 * or it will be forwarded through TCP to the remote server given by DL (the destination IP address)
	 * and DE (the destination IP port).
	 * @param message - the message to write
	 */
	void write(char *message);

	/*
	 * Listens on the wifi UART until a full message is received.
	 * @param stop - the string marking the end of the message
	 * @return the message that was received, which must be freed by the caller
	 */
	WifiMessage *readUntil(char *stop);

	/*
	 * Reads the latest configuration response from the Xbee wifi module.
	 * @return the message object containing the response
	 */
	WifiMessage *configReadStart();

	/*
	 * Reads the latest configuration response from the Xbee wifi module.
	 */
	void configRead();

	/*
	 * Sends a configuration command to the Xbee wifi module.
	 * @param command - the command to send
	 */
	void configSendStart(char *command);

	/*
	 * Sends a configuration command to the Xbee wifi module,
	 * then waits for and prints the response.
	 * @param command - the command to send
	 */
	void configSend(char *command);

	/**
	 * Sends the sequence to enter configuration mode on the Xbee wifi module.
	 */
	void configEnter();

	/*
	 * Indicates whether the wifi module has connected to the network.
	 * @return true if the wifi module is connected
	 */
	bool ready();

	/*
	 * Blocks until the wifi module has connected to the network.
	 */
	void waitForReady();

};

#if defined(WIFIHANDLER_WRITE_DEBUG) || defined(WIFIHANDLER_DEBUG) || defined(DEBUG)
#define WIFIHANDLER_WRITE_LOG(info) info
#else
#define WIFIHANDLER_WRITE_LOG(info)
#endif

#if defined(WIFIHANDLER_READ_DEBUG) || defined(WIFIHANDLER_DEBUG) || defined(DEBUG)
#define WIFIHANDLER_READ_LOG(info) info
#else
#define WIFIHANDLER_READ_LOG(info)
#endif

#if defined(WIFIHANDLER_HTTP_DEBUG) || defined(WIFIHANDLER_DEBUG) || defined(DEBUG)
#define WIFIHANDLER_HTTP_LOG(info) info
#else
#define WIFIHANDLER_HTTP_LOG(info)
#endif

#endif /* WIFIHANDLER_H_ */
