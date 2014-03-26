/*
 * MotorHandler.cpp
 *
 *  Created on: 2014-03-03
 *      Author: Amshu Gongal
 */


#include <stdio.h>
#include <stdlib.h>

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
	motor_dev = alt_up_rs232_open_dev(UART_MOTOR_NAME);
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

//Command Interpreter
/*
 * Interprets the command from the network handler and sends move commands
 * @param motor The motor to move eg. Left or Right
 * @param motorDirection The direction of the given motor to move
 * @param speed The speed the motor is to move
 */
void MotorHandler::motorCommand(string motor, string motorDirection, string speed){
	char motorID = interpretMotor(motor);
	char directionByte = interpretDirection(motorDirection, int(motorID));
	int speedByte = strtol(speed.c_str(), NULL, 0);
	move(directionByte, speedByte, (motor.append(motorDirection)).c_str());
}


char MotorHandler::interpretMotor(string motor){
	if(motor.compare(MOTOR_LEFT) == 0){
		return MOTOR_LEFT_CHAR;
	}
	else{
		return MOTOR_RIGHT_CHAR;
	}
}

char MotorHandler::interpretDirection(string direction, int motor){
	if(direction.compare(MOTOR_FORWARD) == 0){
		return (char) (motor*2 + 1);
	}
	else{
		return (char) (motor*2);
	}
}

// MOTOR CONTROL

/*Moves the given motor in specified direction by Motor Direction with specified
 * Speed
 * @param MotorDirection Motor and direction to move
 * @param Speed the speed to move at
 * @param string description of motor movement
 */

void MotorHandler::move(char motorDirection, char speed,const char *description){
		send(MOTOR_START_BYTE, "start");
		send(MOTOR_DEVICE_TYPE, "device");
		send(motorDirection, description);
		send(speed, "speed");
		printf("\n");
}

// MOTOR CONTROL HELPERS
/*
 * Sends a byte-length message to the motor controller.
 * @param message - the message to send
 * @param description - the string description of the message, for debugging
 */
void MotorHandler::send(char message, const char *description) {
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
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_MOTOR_RESET_N_BASE, 0);
	OSTimeDlyHMSM(0, 0, 0,200 );
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_MOTOR_RESET_N_BASE, 1);
	OSTimeDlyHMSM(0, 0, 0, 200);
}
