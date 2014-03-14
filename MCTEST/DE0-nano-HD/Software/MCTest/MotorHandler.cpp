/*
 * MotorHandler.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Amshu Gongal
 */


#include <stdio.h>

#include "altera_avalon_pio_regs.h"
#include "includes.h"
#include "system.h"

#include "MotorHandler.h"

#define WRITE_FIFO_EMPTY 0x80
#define MOTOR_ENABLE 	1
#define MOTOR_RESET		0

// ALLOCATION
MotorHandler::MotorHandler() {}
MotorHandler::~MotorHandler() {}

// INITIALIZATION

/*
 * Initializes this handler.
 * @return OK if the handler was initialized without error
 */
Status MotorHandler::init() {
	// Enable the motor controller.
	reset();
	// Open the serial device to send commands to the motor controller.
	motor_dev = alt_up_rs232_open_dev(RS232_MOTOR_NAME);
	//printf("%d", motor_dev);
	if (motor_dev == NULL) {
		return ERR_MOTOR_OPEN;
	} else {
		return OK;
	}
}

void MotorHandler::configure(){
	send(MOTOR_START_BYTE, "start");
	send(MOTOR_CHANGE_CONFIGURATION, "change configuration");
	send(0X01, "2 MOTOR CONFIG");
	printf("\n");
}
// TESTING

void MotorHandler::test() {
	send(MOTOR_START_BYTE, "test start");
}

// MOTOR CONTROL

/*
 * Move rover forward by activating both motors.
 */
void MotorHandler::forward() {
	move(MOTOR_MOTOR1_FORWARD, "motor 1 forward");
	move(MOTOR_MOTOR2_FORWARD, "motor 2 forward");
}

/*
 * Move rover backward by activating both motor backwards.
 */
void MotorHandler::backward() {
	move(MOTOR_BOTH_BACKWARD, "both motors backward");
//	move(MOTOR_MOTOR2_BACKWARD, "motor 2 backward");
}

/*
 * Turn rover left.
 */
void MotorHandler::left() {
	move(MOTOR_MOTOR1_FORWARD, "motor 1 forward");
}

/*
 * Turn rover right.
 */
void MotorHandler::right() {
	move(MOTOR_MOTOR2_FORWARD, "motor 2 forward");
}

/*
 * Stop the rover.
 */
void MotorHandler::stop() {
	move(MOTOR_MOTOR1_FORWARD, MOTOR_STOP_SPEED, "motor 1 stop");
	move(MOTOR_MOTOR2_FORWARD, MOTOR_STOP_SPEED, "motor 2 stop");
}

// MOTOR CONTROL HELPERS

/*
 * Moves in the given direction at constant speed.
 * @param direction - the motor and direction to move in, e.g. MOTOR_MOTOR1_FORWARD
 * @param description - the string description of the movement, for debugging
 */
void MotorHandler::move(char direction, char *description) {
	move(direction, MOTOR_CONST_SPEED, description);
}

/*
 * Moves in the given direction and speed.
 * @param direction - the motor and direction to move in, e.g. MOTOR_MOTOR1_FORWARD
 * @param speed - the speed to move at, i.e. MOTOR_CONST_SPEED or MOTOR_STOP_SPEED
 * @param description - the string description of the movement, for debugging
 */
void MotorHandler::move(char direction, char speed, char *description) {
	send(MOTOR_START_BYTE, "start");
	send(MOTOR_DEVICE_TYPE, "device");
	send(direction, description);
	send(speed, "speed");
	printf("\n");
}

/*
 * Sends a byte-length message to the motor controller.
 * @param message - the message to send
 * @param description - the string description of the message, for debugging
 */
void MotorHandler::send(char message, char *description) {
	alt_u32 write_space;
	// Check for write space.
	write_space = alt_up_rs232_get_available_space_in_write_FIFO(motor_dev);
	if (write_space >= WRITE_FIFO_EMPTY) {
		// If space, write.
		int status = alt_up_rs232_write_data(motor_dev, message);
		printf("MotorHandler [message: 0x%02x, description: %s", message, description);
		// Log errors.
		if (status == OK) {
			printf("]\n");
		} else {
			printf(", error: cannot write]\n");
		}
	} else {
		// If no space, wait and retry.
		printf("MotorHandler [message: 0x%02x, description: %s, error: no write space; retrying]\n", message, description);
		OSTimeDlyHMSM(0, 0, 0, 10);
		send(message, description);
	}
}

void MotorHandler::reset(){
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_MOTOR_RST_BASE, 0);
	OSTimeDlyHMSM(0, 0, 0,200 );
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_MOTOR_RST_BASE, 1);
	OSTimeDlyHMSM(0, 0, 0, 200);
}
