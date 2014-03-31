/*
 * MotorHandler.cpp
 *
 *  Created on: 2014-03-24
 *      Author: Amshu Gongal
 */

#include "MotorHandler.h"

#define WRITE_FIFO_EMPTY 0x80
#define MOTOR_ENABLE 	1
#define MOTOR_RESET		0

#define MOTOR_MOTOR_INDEX		1
#define MOTOR_DIRECTION_INDEX	2
#define MOTOR_SPEED_INDEX		3
#define MOTOR_SPEED_BASE_AUTO	0

// TASKS

extern MotorHandler *motor;

/* Waits for motor commands and executes them as they arrive. */
void motor_handler_update_task(void *pdata) {
	TASK_LOG(printf("MotorHandler [task: update, status: start]\n"));
	while (true) {
		try {
			// Update the motor handler.
			motor->update();
		} catch (ARCapException &e) {
			// Log exceptions.
			MOTORHANDLER_LOG(printf("%s\n", e.what()));
		}
	}
}

/* @test Executes the motor handler test suite. */
void motor_handler_test_task(void *pdata) {
	TASK_LOG(printf("MotorHandler [task: test, status: start]\n"));
	while (true) {
		motor->test();
	}
}

// CONSTRUCTION

/**
 * Creates a new motor handler.
 * @throw UARTOpenException if the handler cannot open the UART device used to talk to the motors
 */
MotorHandler::MotorHandler() {
	// Enable the motor controller.
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_MOTOR_RESET_N_BASE, MOTOR_ENABLE);
	// Open the serial device to send commands to the motor controller.
	motor_dev = alt_up_rs232_open_dev(UART_MOTOR_NAME);
	if (motor_dev == NULL) {
		throw new UARTOpenException();
	}
}

// COMMANDS

/**
 * Gets the speed from the given motor command.
 * The speed is the number following the motor and direction characters.
 * @return the speed
 */
int MotorHandler::getSpeedFrom(char *command) {
	return strtol(command + MOTOR_SPEED_INDEX, NULL, MOTOR_SPEED_BASE_AUTO);
}

/**
 * Parses the given motor command.
 * @param command - of the form m$m$d$sss, e.g. mlf80
 * $m - the motor to move, from {l, r} for "left" and "right"
 * $d - the direction to move in, from {f, b} for "forward" and "back"
 * $sss - the speed to move at, between 0 and 127
 * The command must be dynamically allocated, and will be freed at the end of this method.
 */
void MotorHandler::parse(char *command) {
	MOTORHANDLER_LOG(printf("MotorHandler [command: %s]\n", command));
	if (command[MOTOR_MOTOR_INDEX] == MOTOR_BOTH) {
		if (command[MOTOR_DIRECTION_INDEX] == MOTOR_FORWARD) {
			move(MOTOR_RIGHT_FORWARD, getSpeedFrom(command), "motor right forward");
			move(MOTOR_LEFT_FORWARD, getSpeedFrom(command), "motor left forward");
		} else {
			move(MOTOR_LEFT_BACKWARD, getSpeedFrom(command), "motor left backward");
			move(MOTOR_RIGHT_BACKWARD, getSpeedFrom(command), "motor right backward");
		}
	} else if(command[MOTOR_MOTOR_INDEX] == MOTOR_LEFT) {
		if (command[MOTOR_DIRECTION_INDEX] == MOTOR_FORWARD) {
			move(MOTOR_LEFT_FORWARD, getSpeedFrom(command), "motor left forward");
		} else {
			move(MOTOR_LEFT_BACKWARD, getSpeedFrom(command), "motor left backward");
		}
	} else {
		if (command[MOTOR_DIRECTION_INDEX] == MOTOR_FORWARD) {
			move(MOTOR_RIGHT_FORWARD, getSpeedFrom(command), "motor right forward");
		} else {
			move(MOTOR_RIGHT_BACKWARD, getSpeedFrom(command), "motor right backward");
		}
	}
	free(command);
}

// MOTOR CONTROL

/**
 * Resets the motor controller.
 */
void MotorHandler::reset(){
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_MOTOR_RESET_N_BASE, MOTOR_RESET);
	OSTimeDlyHMSM(0, 0, 0, 200);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_MOTOR_RESET_N_BASE, MOTOR_ENABLE);
	OSTimeDlyHMSM(0, 0, 0, 200);
}

/**
 * Configures the motor controller.
 */
void MotorHandler::configure(){
	reset();
	send(MOTOR_START_BYTE, "start");
	send(MOTOR_CHANGE_CONFIGURATION, "change configuration");
	send(0X01, "2 MOTOR CONFIG");
	MOTORHANDLER_LOG(printf("\n"));
}

/**
 * Moves in the given direction at constant speed.
 * @param direction - the motor and direction to move in, e.g. MOTOR_MOTOR1_FORWARD
 * @param speed speed of the motor
 * @param description - the string description of the movement, for debugging
 */
void MotorHandler::move(char motorDirection, char speed, const char *description){
	send(MOTOR_START_BYTE, "start");
	send(MOTOR_DEVICE_TYPE, "device");
	send(motorDirection, description);
	send(speed, "speed");
	MOTORHANDLER_LOG(printf("\n"));
}

/**
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
		MOTORHANDLER_LOG(printf("MotorHandler [message: 0x%02x, description: %s]\n", message, description));
		// Log errors.
		if (status != OK) MOTORHANDLER_LOG(printf("MotorHandler [error: cannot write]\n"));
	} else {
		// If no space, wait and retry.
		MOTORHANDLER_LOG(printf("MotorHandler [message: 0x%02x, description: %s, error: no write space; retrying]\n", message, description));
		OSTimeDlyHMSM(0, 0, 0, 10);
		send(message, description);
	}
}

// TESTING

/**
 * Tests the motor controller.
 * Runs both motors forward, both backward, left forward and backward, right forward and backward.
 */
void MotorHandler::test() {
	OSTimeDlyHMSM(0, 0, 3, 0);
	// Go forward.
	parse("mlf120");
	parse("mrf120");
	OSTimeDlyHMSM(0, 0, 2, 0);
	// Stop.
	parse("mlf0");
	parse("mrf0");
	OSTimeDlyHMSM(0, 0, 5, 0);
	// Go backward.
	parse("mlb120");
	parse("mrb120");
	OSTimeDlyHMSM(0, 0, 2, 0);
	// Stop.
	parse("mlf0");
	parse("mrf0");
	OSTimeDlyHMSM(0, 0, 5, 0);
	// Turn left.
	parse("mrf120");
	OSTimeDlyHMSM(0, 0, 2, 0);
	// Stop.
	parse("mrf0");
	OSTimeDlyHMSM(0, 0, 5, 0);
	// Turn right.
	parse("mlf120");
	OSTimeDlyHMSM(0, 0, 2, 0);
	// Stop.
	parse("mlf0");
	OSTimeDlyHMSM(0, 0, 5, 0);
}
