/*
 * MotorHandler.h
 *
 *  Created on: 2014-03-03
 *      Author: Amshu Gongal
 */

#ifndef MOTORHANDLER_H_
#define MOTORHANDLER_H_

#include "altera_up_avalon_rs232.h"
#include "altera_up_avalon_rs232_regs.h"
#include <stdio.h>
using namespace std;
#include <string>
#include "Status.h"

//Commands from network handler
#define MOTOR_LEFT "left"
#define MOTOR_RIGHT "right"
#define MOTOR_FORWARD "forward"
#define MOTOR_BACKWARD "backward"


// Motor messages are LSB first
#define MOTOR_START_BYTE 		0x80
#define MOTOR_DEVICE_TYPE 		0x00
#define MOTOR_LEFT_CHAR 	 	0x02		// motor 1 backward
#define MOTOR_RIGHT_CHAR 	    0x03		// motor 2 backward
#define MOTOR_LEFT_FORWARD 		0x05		// motor 1 forward
#define MOTOR_RIGHT_FORWARD 	0x07		// motor 2 forward
#define MOTOR_CONST_SPEED 		0x5F
#define MOTOR_STOP_SPEED 		0x00
#define MOTOR_CHANGE_CONFIGURATION 0X02

class MotorHandler {
public:
	MotorHandler();
	virtual ~MotorHandler();

	/*
	 * Initializes this handler.
	 * @return OK if the handler was initialized without error
	 */
	Status init();
	void reset();
	void configure();
	void motorCommand(string motor, string motorDirection, string speed);

private:
	/* The device used to send serial commands to the motor controller. */
	alt_up_rs232_dev *motor_dev;

	char interpretMotor(string motor);
	char interpretDirection(string direction, int motor);

	/*
	 * Moves in the given direction at constant speed.
	 * @param direction - the motor and direction to move in, e.g. MOTOR_MOTOR1_FORWARD
	 * @param speed speed of the motor
	 * @param description - the string description of the movement, for debugging
	 */
	void move(char direction, char speed, const char *description);

	/*
	 * Sends a byte-length message to the motor controller.
	 * @param message - the message to send
	 * @param description - the string description of the message, for debugging
	 */
	void send(char message, const char *description);
};

#endif /* MOTORHANDLER_H_ */
