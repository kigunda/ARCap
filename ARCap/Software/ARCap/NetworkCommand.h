/*
 * NetworkCommand.h
 *
 *  Created on: 2014-03-03
 *      Author: kigunda
 */

#ifndef NETWORKCOMMAND_H_
#define NETWORKCOMMAND_H_

/*
 * An enumeration over the commands that can be sent from and to the remote server.
 */
typedef enum NetworkCommand {
	NETWORK_COMMAND_SHOOT, /* Indicates an infrared "shoot" event. */
	NETWORK_INFRARED_HIT, 	/* Indicates an infrared "hit" event. */
	NETWORK_MOTOR			/* Indicates a motor command. */
} NetworkCommand;

#endif /* NETWORKCOMMAND_H_ */
