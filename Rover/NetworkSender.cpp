/*
 * NetworkSender.cpp
 *
 *  Created on: 2014-03-25
 *      Author: Kenan Kigunda
 */

#include "NetworkSender.h"

// TASKS

extern NetworkSender *networkOut;

/* Waits for outgoing network commands and forwards them as they arrive. */
void network_sender_update_task(void *pdata) {
	TASK_LOG(printf("NetworkSender [task: update, status: start]\n"));
	while (true) {
		try {
			// Update the network sender.
			networkOut->update();
		} catch (ARCapException &e) {
			// Log exceptions.
			NETWORKSENDER_LOG(printf("%s\n", e.what()));
		}
	}
}

// CONSTRUCTION

/**
 * Creates a new network sender.
 * @param wifi - the wifi handler to use to send messages to the server
 */
NetworkSender::NetworkSender(WifiHandler *wifiHandler) {
	wifi = wifiHandler;
}

// COMMANDS

/**
 * Forwards the given network command to the server.
 * @param command the command to forward
 */
void NetworkSender::parse(char *command) {
	NETWORKSENDER_LOG(printf("NetworkSender [command: %s]\n", command));
	wifi->tcpSendAndConfirm(command, WAIT_FOREVER);
}
