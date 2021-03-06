/*
 * NetworkReceiver.cpp
 *
 *  Created on: 2014-03-25
 *      Author: Kenan Kigunda
 */

#include "altera_avalon_pio_regs.h"

#include "NetworkReceiver.h"

// TASKS

extern NetworkReceiver *networkIn;

/**
 * Polls for server updates.
 */
void network_receiver_update_task(void* pdata) {
	TASK_LOG(printf("NetworkReceiver [task: update, status: start]\n"));
	while (true) {
		try {
			// Update the network receiver.
			networkIn->update();
		} catch (ARCapException &e) {
			// Log exceptions.
			NETWORKRECEIVER_LOG(printf("%s\n", e.what()));
		}
		// Wait.
		OSTimeDlyHMSM(0, 0, 0, NETWORK_RECEIVER_UPDATE_TIME_MILLIS);
	}
}

/**
 * Pings the server to confirm the continued connection between the rover and the server.
 */
void network_receiver_check_task(void* pdata) {
	TASK_LOG(printf("NetworkChecker [task: update, status: start]\n"));
	while (true) {
		try {
			// Ping the server.
			networkIn->ping();
		} catch (ARCapException &e) {
			// Log exceptions.
			NETWORKRECEIVER_LOG(printf("%s\n", e.what()));
		}
		// Wait.
		OSTimeDlyHMSM(0, 0, NETWORK_RECEIVER_CHECK_TIME_SECONDS, 0);
	}
}

// CONSTRUCTION

/**
 * Creates a new network receiver.
 * @param wifi - the wifi handler to use to receive messages from the server
 */
NetworkReceiver::NetworkReceiver(WifiHandler *wifiHandler) {
	wifi = wifiHandler;
}

// LISTENERS

/*
 * Adds a listener queue to this network receiver.
 * The queue will receive network events which start with the listener's key.
 * @param key the first character of network events intended for the listener
 * @param queue - the queue to which matching network events will be forwarded
 */
void NetworkReceiver::addListener(char key, OS_EVENT *queue) {
	keys.push_back(key);
	listeners.push_back(queue);
}

/**
 * Posts a network event to the matching listener.
 * @param event - the name and parameters of the event.
 * The event will be posted to the listener whose key matches the first character of the event.
 * @throw PostException if the event cannot be posted to the matching listener
 */
void NetworkReceiver::postMatching(const char *event) {
	char key = event[0];
	int index = 0;
	for (vector<char>::iterator it = keys.begin(); it != keys.end(); ++it) {
		// Look for a matching key.
		if (key == *it) {
			// Post to the matching listener.
			int status = OSQPost(listeners[index], (void *)event);
			if (status != OS_NO_ERR) {
				throw new QueuePostException();
			}
		}
		// Go the next key.
		index++;
	}
}

/**
 * Posts a network event to the all the listeners.
 * @param event - the name and parameters of the event.
 * @throw QueuePostException if the event cannot be posted to a listener
 */
void NetworkReceiver::postAll(const char *event) {
	for (vector<OS_EVENT *>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
		// Post to the listener.
		int status = OSQPost(*it, (void *)event);
		if (status != OS_NO_ERR) {
			throw new QueuePostException();
		}
	}
}

// UPDATES

/**
 * Updates this receiver. The receiver will check the wifi handler for new TCP messages
 * and forward them to the corresponding listeners.
 * @throw QueuePostException if a message cannot be posted to a listener
 */
void NetworkReceiver::update() {
	if (wifi->hasData()) {
		char *message = wifi->tcpReceive();
		if (message == NULL) {
			NETWORKRECEIVER_LOG(printf("[NetworkReceiver] error: wifi busy\n"));
		} else {
			NETWORKRECEIVER_LOG(printf("[NetworkReceiver] message: %s", message));
			postMatching(message);
		}
	}
}

/**
 * Pings the server to check if this rover is connected to the network
 * and forwards the connection status to the listeners.
 */
void NetworkReceiver::ping() {
	// Ping the server.
	wifi->tcpSend(MESSAGE_CONNECT_PREFIX ROVER_ID, WAIT_FOREVER);
	char *response = wifi->tcpReceive();
	if (response == NULL) {
		NETWORKRECEIVER_LOG(printf("[NetworkReceiver] error: wifi busy\n"));
	} else {
		if (response[0] == '\0') {
			// Server did not respond.
			// Indicate the failure on the LED.
			IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, NETWORK_LED_DISCONNECTED);
			// Post network disconnect to all listeners.
			postAll(NETWORK_COMMAND_DISCONNECT);
			NETWORKRECEIVER_LOG(printf("NetworkReceiver [status: disconnected]\n"));
		} else if (response[0] == MESSAGE_CONTROL_PREFIX) {
			// Server sent back a control command. Check for MESSAGE_OK.
			if (strncmp(response, MESSAGE_OK, MESSAGE_OK_LENGTH) == 0) {
				// Indicate success on the LED.
				IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, NETWORK_LED_CONNECTED);
				// Post network connect to all listeners.
				// postAll(NETWORK_COMMAND_CONNECT); // No listeners for connect event, save useless posts for now.
				NETWORKRECEIVER_LOG(printf("NetworkReceiver [status: connected]\n"));
			} else {
				NETWORKRECEIVER_LOG(printf("NetworkReceiver [status: unknown]\n"));
			}
		} else {
			// Server sent a message.
			// Indicate success on the LED.
			IOWR_ALTERA_AVALON_PIO_DATA(PIO_IR_EMITTER_BASE, NETWORK_LED_CONNECTED);
			// Post to matching listener.
			postMatching(response);
			NETWORKRECEIVER_LOG(printf("[NetworkReceiver] message: %s", response));
		}
	}
}
