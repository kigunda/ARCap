/*
 * NetworkSender.h
 *
 *  Created on: 2014-03-25
 *      Author: Kenan Kigunda
 */

#ifndef NETWORKSENDER_H_
#define NETWORKSENDER_H_

#include "Listener.h"
#include "WifiHandler.h"

/* Waits for outgoing network commands and forwards them as they arrive. */
void network_sender_update_task(void *pdata);

/**
 * Listens for events from handlers on the rover and forwards them to the server
 * using a wifi handler.
 */
class NetworkSender: public Listener {
public:
	/**
	 * Creates a new network sender.
	 * @param wifi - the wifi handler to use to send messages to the server
	 */
	NetworkSender(WifiHandler *wifiHandler);

private:
	/* The wifi handler used by this sender to communicate with the server. */
	WifiHandler *wifi;

	/**
	 * Forwards the given network command to the server.
	 * @param command the command to forward
	 */
	void parse(char *command);
};

#if defined(NETWORKSENDER_DEBUG) || defined(DEBUG)
#define NETWORKSENDER_LOG(info) info
#else
#define NETWORKSENDER_LOG(info)
#endif

#endif /* NETWORKSENDER_H_ */
