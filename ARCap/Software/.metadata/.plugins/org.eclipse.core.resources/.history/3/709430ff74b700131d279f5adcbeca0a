/*
 * NetworkReceiver.cpp
 *
 *  Created on: 2014-03-25
 *      Author: Kenan Kigunda
 */

#include "NetworkReceiver.h"

// TASKS

extern NetworkReceiver *networkIn;

/* Polls for server updates. */
void network_receiver_update_task(void* pdata) {
	printf("NetworkReceiver [task: update, status: start]\n");
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
void NetworkReceiver::post(const char *event) {
	char key = event[0];
	int index = 0;
	int status;
	for (vector<char>::iterator it = keys.begin(); it != keys.end(); ++it) {
		// Look for a matching key.
		if (key == *it) {
			// Post to the matching listener.
			status = OSQPost(listeners[index], (void *)event);
			if (status != OS_NO_ERR) {
				throw new QueuePostException();
			}
		}
		// Go the next key.
		index++;
	}
}

// UPDATES

/**
 * Updates this receiver. The receiver will check the wifi handler for new TCP messages
 * and forward them to the corresponding listeners.
 * @throw PostException if a message cannot be posted to a listener
 */
void NetworkReceiver::update() {
	char *message = wifi->tcpReceive();
	if (message != NULL) {
		post(message);
		wifi->tcpSend(MESSAGE_OK, "\n");
	}
}
