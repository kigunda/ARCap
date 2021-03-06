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

#include "Status.h"

/* Network details */
#define SSID				"ARCap"
#define	SSID_KEY			"playthegame"
#define SERVER_ADDRESS		"192.168.0.100"		// reserved for server
#define SERVER_TCP_PORT		"2710" 				// port 10000 (0x2710)
#define SERVER_HTTP_PORT	"50"				// port 80 (0x50)
#define SERVER_HOSTNAME		"surface"

/* Test details */
#define HTTP_TEST_PAGE		"/arcap/infrared/hit.php"

/* Indicates how long callers will wait to use a busy connection. */
#define WIFI_DEFAULT_TIMEOUT				OS_TICKS_PER_SEC

/* Indicates how many times we will continue reading when we receive no data. */
#define	WIFI_READ_AVAILABLE_RETRIES			(1 << 18)

/* The LED is turned off when we're connected to the server. */
#define WIFI_LED_WAITING		0
#define WIFI_LED_CONNECTED		0
#define WIFI_LED_DISCONNECTED	1

/* Indicates how long we should wait before trying to connect to the server again on startup. */
#define WIFI_CONNECT_RETRY_TIME_SECONDS		1

typedef enum {
	WIFI_TCP,
	WIFI_HTTP
} WifiSetupType;

/**
 * Starts network communications.
 * This task configures the wifi module and calls WifiHandler::tcpConnect.
 * All other tasks must wait for the connection to be confirmed before executing.
 */
void wifi_handler_tcp_start_task(void *pdata);

/**
 * Tests network communications.
 * This test sends messages to a PHP echo server and prints the response.
 * Before running this, first start the echo server by visiting 192.168.0.100/arcap/socket.php
 * from a browser on the ARCap network.
 * To view output enable WIFIHANDLER_TCP_DEBUG in Debug.h.
 */
void wifi_handler_tcp_test_task(void *pdata);

/**
 * Test network communications.
 * This test sends a GET request to an HTTP server and prints the response.
 * Before running this, make sure the computer serving /arcap/infrared/hit.php
 * is connecting to the ARCap network.
 * To view output enable WIFIHANDLER_HTTP_DEBUG in Debug.h.
 */
void wifi_handler_http_test_task(void *pdata);

/**
 * The wifi handler provides a high-level view of the interfacing to the Xbee wifi module.
 */
class WifiHandler {
public:
	/**
	 * Creates a new wifi handler.
	 * @throw UARTOpenException if the handler cannot open the UART device used to talk to the wifi module
	 * @throw SemCreateException if the handler cannot create a semaphore used to control access to the wifi connection
	 */
	WifiHandler();

	/**
	 * Sets up the wifi module for TCP communication with the server.
	 * If the module is already configured for TCP, prints the status of the module.
	 * After confirming the module status, calls tcpConnect() to establish a connection to the server socket.
	 * @return true if the connection was established
	 * This method will also indicate the connection status by turning off the LED.
	 */
	bool tcpStart();

	/**
	 * Sends the given TCP message to the server.
	 * @param message - the message to send
	 * @param timeout - the number of ticks that the caller is willing to wait to use the connection
	 * @return true if the message was sent successfully, or false if the message could not be sent
	 * because the wifi connection is busy
	 */
	bool tcpSend(char *message, INT16U timeout);

	/**
	 * Sends the given TCP message to the server and waits for the confirmation MESSAGE_OK.
	 * The message will be resent until we receive MESSAGE_OK.
	 * @param message - the message to send
	 * @param timeout - the number of ticks that the caller is willing to wait to use the connection
	 * @return true if the message was sent successfully, or false if the message could not be sent
	 * because the wifi connection is busy
	 */
	bool tcpSendAndConfirm(char *message, INT16U timeout);

	/**
	 * Waits for the next TCP message from the server.
	 * The message is terminated by a newline '\n'.
	 * @return the message received, or NULL if no message could be read because the wifi connection is busy
	 */
	char *tcpReceive();

	/**
	 * Tests network communications.
	 * Sends a series of commands to the server, prints the responses,
	 * and closes the connection.
	 */
	void tcpSendTest();

	/**
	 * Test network communications from the server to the rover.
	 * Listens on the socket for new messages and prints them out.
	 */
	void tcpReceiveTest();

	/**
	 * Sets up the wifi module for HTTP communication with the server.
	 * If the module is already configured for HTTP, prints the status of the module.
	 * @return true if the connection was established
	 * This method will also indicate the connection status by turning off the LED.
	 */
	bool httpStart();

	/**
	 * Sends an HTTP GET request for the given URL.
	 * @param url - the URL specifying the resource to request
	 * @return the response to the request
	 */
	char *httpGet(char *url);

	/**
	 * Indicates whether there is data to read.
	 * @return true - if there is data in the read buffer
	 */
	bool hasData();

	/**
	 * Tests the UART communications between the board and the wifi module.
	 * Sends the configuration command "+++" and expects the response "OK\r".
	 */
	void testUart();

private:
	/* The UART device used to write and read data to and from the Xbee wifi module. */
	alt_up_rs232_dev *wifi_dev;

	/* The semaphore used to ensure that multiple requests are not made simultaneously to the same connection. */
	OS_EVENT *wifi_lock;

	/* The semaphore used to ensure that we have a connection to the server before starting
	 * a task that might make a wifi request. */
	OS_EVENT *require_lock;

	/**
	 * Sets up the wifi module to connect to the network.
	 * The connection settings are stored in non-volatile memory,
	 * so this method only needs to be run once per Xbee module.
	 * After that, use status() to ensure that the settings are correct.
	 * @param type
	 * WIFI_TCP - used to send TCP messages to PHP socket at 192.168.0.100:10000; preferred for full duplex communication
	 * WIFI_HTTP - used to send HTTP requests to HTTP server at 192.168.0.100:80; used in earlier testing
	 */
	void setup(WifiSetupType type);

	/**
	 * Prints the current status of the wifi module.
	 */
	void status();

	/**
	 * Connects to the server socket.
	 * This method sends the rover identification command r(id), where (id) is ROVER_ID,
	 * and waits until it receives the response "#ok".
	 * @return true if connection was accepted
	 */
	bool tcpConnect();

	/**
	 * Waits to acquire the lock on the wifi connection.
	 * Since we are using transparent mode, there can only be one connection at a time.
	 * Methods calling write() or readUntil() must acquire this lock first.
	 * @param timeout - the maximum number of ticks that the caller is willing to wait to acquire the lock
	 * @return true if the lock was acquired
	 */
	bool lockUpTo(INT16U timeout);

	/**
	 * Waits to acquire the lock on the wifi connection.
	 * Since we are using transparent mode, there can only be one connection at a time.
	 * Methods calling write() or readUntil() must acquire this lock first.
	 * @return true if the lock was acquired
	 */
	bool lock();

	/**
	 * Releases the lock on the wifi connection.
	 * Must be called at the end of any method calling lock().
	 */
	void unlock();

	/**
	 * Sets the destination port based on the setup type.
	 * @param type
	 * WIFI_TCP - targets the socket at port 10000
	 * WIFI_HTTP - targets the server at port 80
	 */
	void setDestinationPort(WifiSetupType type);

	/*
	 * Writes a message to the wifi UART device.
	 * The message will be used to configure the Xbee wifi module, if the device is in configuration mode,
	 * or it will be forwarded through TCP to the remote server given by DL (the destination IP address)
	 * and DE (the destination IP port).
	 * @param message - the message to write
	 */
	void write(char *message);

	/**
	 * Listens on the wifi UART until a full message is received.
	 * @param message - the buffer in which the message will be placed
	 * @param stop - the string marking the end of the message
	 */
	void readIntoBufferUntil(char *message, char *stop);

	/*
	 * Listens on the wifi UART until a full message is received.
	 * @param stop - the string marking the end of the message
	 * @return the message that was received, which must be freed by the caller
	 */
	char *readUntil(char *stop);

	/*
	 * Reads the latest configuration response from the Xbee wifi module.
	 * @return the message object containing the response
	 */
	char *configReadStart();

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

#if defined(WIFIHANDLER_CONFIG_DEBUG) || defined(WIFIHANDLER_DEBUG) || defined(DEBUG)
#define WIFIHANDLER_CONFIG_LOG(info) info
#else
#define WIFIHANDLER_CONFIG_LOG(info)
#endif

#if defined(WIFIHANDLER_TCP_DEBUG) || defined(WIFIHANDLER_DEBUG) || defined(DEBUG)
#define WIFIHANDLER_TCP_LOG(info) info
#else
#define WIFIHANDLER_TCP_LOG(info)
#endif

#if defined(WIFIHANDLER_HTTP_DEBUG) || defined(WIFIHANDLER_DEBUG) || defined(DEBUG)
#define WIFIHANDLER_HTTP_LOG(info) info
#else
#define WIFIHANDLER_HTTP_LOG(info)
#endif

#endif /* WIFIHANDLER_H_ */
