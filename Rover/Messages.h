/*
 * Command.h
 *
 *  Created on: 2014-04-03
 *      Author: Kenan Kigunda
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#define ROVER_ID		"r2"

#define MESSAGE_CONNECT_PREFIX	"+"
#define MESSAGE_CONTROL_PREFIX	':'
#define MESSAGE_STOP_SUFFIX		"\n"

#define MESSAGE_OK				":ok"
#define MESSAGE_ERR				":err"
#define MESSAGE_OK_LENGTH		3
#define MESSAGE_ERR_LENGTH		4

#define COMMAND_TYPE_INDEX	0

#define INFRARED_COMMAND		'i'
#define INFRARED_TYPE_INDEX		1
#define INFRARED_SHOOT			's'
#define INFRARED_HIT			'h'

#define	MOTOR_COMMAND			'm'
#define MOTOR_MOTOR_INDEX		1
#define MOTOR_LEFT 				'l'
#define MOTOR_RIGHT 			'r'
#define MOTOR_BOTH				'b'
#define MOTOR_DIRECTION_INDEX	2
#define MOTOR_FORWARD 			'f'
#define MOTOR_BACKWARD			'b'
#define MOTOR_SPEED_INDEX		3
#define MOTOR_SPEED_BASE		0

#define NETWORK_COMMAND			'n'
#define	NETWORK_STATUS_INDEX	1
#define NETWORK_CONNECT			'c'
#define NETWORK_DISCONNECT		'd'

#define INFRARED_COMMAND_SHOOT		"is"
#define INFRARED_COMMAND_HIT		"ih"
#define NETWORK_COMMAND_CONNECT		"nc"
#define NETWORK_COMMAND_DISCONNECT	"nd"

#endif /* COMMAND_H_ */
