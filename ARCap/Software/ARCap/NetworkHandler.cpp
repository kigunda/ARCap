/*
 * NetworkHandler.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda, Amshu Gongal
 */

using namespace std;
#include <iostream>

#include "NetworkHandler.h"

// ALLOCATION
NetworkHandler::NetworkHandler() {}
NetworkHandler::~NetworkHandler() {}

// INITIALIZATION

/*
 * Initializes this data source.
 * @return OK if there are no initialization errors
 */
Status NetworkHandler::init() {
	wifi = new WifiHandler();
	return wifi->init();
}

/*
 * Connects to the network.
 * @return OK if the connection was successful
 */
Status NetworkHandler::connect() {
	wifi->status();
	return OK;
}

// OUTGOING

/*
 * Sends a network command to the remote server.
 * @param command - the command to send
 * @param parameters - the parameters specifying further command details,
 * in the format param1=value1&param2=value2&...
 */
Status NetworkHandler::send(NetworkCommand command, string parameters) {
	NETWORKHANDLER_SEND_LOG(cout << "NetworkHandler [direction: out, command: " <<  (int)command << ", parameters: ?" << parameters << "]\n");
	if (command == NETWORK_INFRARED_HIT) {
		char *url = "/arcap/infrared/hit.php";
		WifiMessage *response = wifi->httpGet(url);
		NETWORKHANDLER_SUMMARY_LOG(printf("GET %s RETURNED\n", url));
		NETWORKHANDLER_SEND_LOG(wifiMessagePrint(response));
		free(response);
		return OK;
	} else {
		NETWORKHANDLER_SEND_LOG(printf("NetworkHandler [error: command not accepted]\n"));
		return ERR_NETWORK_COMMAND_NOT_ACCEPTED;
	}
}

// INCOMING

/*
 * Updates this data source.
 * @return OK if all incoming network messages have been read and posted to all listeners without error
 */
Status NetworkHandler::update() {
	char *url = "/arcap/infrared/hit.php";
	WifiMessage *response = wifi->httpGet(url);
	NETWORKHANDLER_SUMMARY_LOG(printf("GET %s RETURNED\n", url));
	NETWORKHANDLER_SEND_LOG(wifiMessagePrint(response));
	free(response);
	return OK;
}
