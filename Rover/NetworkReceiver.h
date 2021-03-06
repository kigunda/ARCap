/*
 * NetworkReceiver.h
 *
 *  Created on: 2014-03-25
 *      Author: Kenan Kigunda
 */

#ifndef NETWORKRECEIVER_H_
#define NETWORKRECEIVER_H_

#include <vector>

#include "Status.h"
#include "WifiHandler.h"

#define NETWORK_RECEIVER_UPDATE_TIME_MILLIS		500
#define NETWORK_RECEIVER_CHECK_TIME_SECONDS		15

/* The LED is turned off when we're connected to the server. */
#define NETWORK_LED_CONNECTED		0
#define NETWORK_LED_DISCONNECTED	1

/**
 * Polls for server updates.
 */
void network_receiver_update_task(void* pdata);

/**
 * Pings the server to confirm the continued connection between the rover and the server.
 */
void network_receiver_check_task(void *pdata);

/**
 * Listens for messages from the server using a wifi handler
 * and forwards them to handlers that execute the corresponding effects on the rover.
 */
class NetworkReceiver {
public:
	/**
	 * Creates a new network receiver.
	 * @param wifi - the wifi handler to use to receive messages from the server
	 */
	NetworkReceiver(WifiHandler *wifiHandler);

	/*
	 * Adds a listener queue to this network receiver.
	 * The queue will receive network events which start with the listener's key.
	 * @param key the first character of network events intended for the listener
	 * @param queue - the queue to which matching network events will be forwarded
	 */
	void addListener(char key, OS_EVENT *queue);

	/**
	 * Updates this receiver. The receiver will check the wifi handler for new TCP messages
	 * and forward them to the corresponding listeners.
	 * @throw PostException if a message cannot be posted to a listener
	 */
	void update();

	/**
	 * Pings the server to check if this rover is connected to the network
	 * and forwards the connection status to the listeners.
	 */
	void ping();

private:
	/* The wifi handler used by this receiver to communicate with the server. */
	WifiHandler *wifi;

	/* The keys used to identify listeners. */
	vector<char> keys;

	/* The queues used to send messages to listeners. */
	vector<OS_EVENT *> listeners;

	/**
	 * Posts a network event to the matching listener.
	 * @param event - the name and parameters of the event.
	 * The event will be posted to the listener whose key matches the first character of the event.
	 * @throw QueuePostException if the event cannot be posted to the matching listener
	 */
	void postMatching(const char *event);

	/**
	 * Posts a network event to the all the listeners.
	 * @param event - the name and parameters of the event.
	 * @throw QueuePostException if the event cannot be posted to a listener
	 */
	void postAll(const char *event);

};

#if defined(NETWORKRECEIVER_DEBUG) || defined(DEBUG)
#define NETWORKRECEIVER_LOG(info) info
#else
#define NETWORKRECEIVER_LOG(info)
#endif

#endif /* NETWORKRECEIVER_H_ */
