/*
 * NetworkHandler.h
 *
 *  Created on: 2014-03-03
 *      Author: Kenan Kigunda
 */

#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_

using namespace std;
#include <string>

#include "DataSource.h"
#include "Debug.h"
#include "NetworkCommand.h"
#include "Status.h"

#define NETWORK_HANDLER_UPDATE_TIME_MILLIS			50

/*
 * The network handler manages communications between the rovers and the remote players.
 * It provides a high-level view of the interfacing of the wifi module.
 */
class NetworkHandler: public DataSource {
public:
	NetworkHandler();
	virtual ~NetworkHandler();

	/*
	 * Initializes this data source.
	 * @return OK if there are no initialization errors
	 */
	Status init();

	/*
	 * Updates this data source.
	 * @return OK if all incoming network messages have been read and posted to all listeners without error
	 */
	Status update();

	/*
	 * Sends a network command to the remote server.
	 * @param command - the command to send
	 * @param parameters - the parameters specifying further command details,
	 * in the format param1=value1&param2=value2&...
	 */
	Status send(NetworkCommand command, string parameters);
};

#if defined(NETWORKHANDLER_SEND_DEBUG) || defined(NETWORKHANDLER_DEBUG) || defined(DEBUG)
#define NETWORKHANDLER_SEND_LOG(info) info
#else
#define NETWORKHANDLER_SEND_LOG(info)
#endif

#if defined(NETWORKHANDLER_RECEIVE_DEBUG) || defined(NETWORKHANDLER_DEBUG) || defined(DEBUG)
#define NETWORKHANDLER_RECEIVE_LOG(info) info
#else
#define NETWORKHANDLER_RECEIVE_LOG(info)
#endif

#endif /* NETWORKHANDLER_H_ */
