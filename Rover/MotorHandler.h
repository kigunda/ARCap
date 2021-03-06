/*
 * MotorHandler.h
 *
 *  Created on: 2014-03-24
 *      Author: Amshu Gongal
 */

#ifndef MOTORHANDLER_H_
#define MOTORHANDLER_H_

#include "altera_up_avalon_rs232.h"
#include "altera_avalon_pio_regs.h"
#include "Listener.h"

#define MOTOR_START_BYTE 			0x80
#define MOTOR_DEVICE_TYPE 			0x00
#define MOTOR_LEFT_BACKWARD 	 	0x04		// motor 1 backward
#define MOTOR_RIGHT_BACKWARD 	    0x06		// motor 2 backward
#define MOTOR_LEFT_FORWARD 			0x05		// motor 1 forward
#define MOTOR_RIGHT_FORWARD 		0x07		// motor 2 forward
#define MOTOR_CONST_SPEED 			0x5F
#define MOTOR_STOP_SPEED 			0x00
#define MOTOR_CHANGE_CONFIGURATION 	0X02

/* Waits for motor commands and executes them as they arrive. */
void motor_handler_update_task(void *pdata);

/* @test Executes the motor handler test suite. */
void motor_handler_test_task(void *pdata);

/**
 * Listens for commands on a queue and uses them to control the motor.
 * The command must specify:
 * (a) which motor to run {l(eft), r(ight)},
 * (b) the direction to run the motor in {(f)orward, (b)ackward}
 * (c) the speed to run the motor at [0 to 127].
 */
class MotorHandler: public Listener {
public:
	/**
	 * Creates a new motor handler.
	 * @throw UARTOpenException if the handler cannot open the UART device used to talk to the motors
	 */
	MotorHandler();

	/**
	 * Sets the motor direction and speed according to the given command.
	 * @param command - of the form m$m$d$sss, e.g. mlf80
	 * $m - the motor to move, from {l, r} for "left" and "right"
	 * $d - the direction to move in, from {f, b} for "forward" and "back"
	 * $sss - the speed to move at, between 0 and 127
	 */
	void set(char *command);

	/**
	 * Tests the motor controller.
	 */
	void test();

private:
	/* The device used to send serial commands to the motor controller. */
	alt_up_rs232_dev *motor_dev;

	/**
	 * Resets the motor controller.
	 */
	void reset();

	/**
	 * Configures the motor controller.
	 */
	void configure();

	/**
	 * Parses the given command.
	 * The command can be either a motor command or a network command.
	 */
	void parse(char *command);

	/**
	 * Parses the given motor command.
	 * @param command - of the form m$m$d$sss, e.g. mlf80
	 * $m - the motor to move, from {l, r} for "left" and "right"
	 * $d - the direction to move in, from {f, b} for "forward" and "back"
	 * $sss - the speed to move at, between 0 and 127
	 * The command must be dynamically allocated, and will be freed at the end of this method.
	 */
	void parseMotorCommand(char *command);

	/**
	 * Parses the given network status command.
	 * The motor handler will stop the motors if the status is NETWORK_DISCONNECT.
	 */
	void parseNetworkCommand(char *command);

	/**
	 * Gets the speed from the given motor command.
	 * The speed is the number following the motor and direction characters.
	 * @return the speed
	 */
	int getSpeedFrom(char *command);

	/**
	 * Moves in the given direction at constant speed.
	 * @param direction - the motor and direction to move in, e.g. MOTOR_MOTOR1_FORWARD
	 * @param speed speed of the motor
	 * @param description - the string description of the movement, for debugging
	 */
	void move(char direction, char speed, const char *description);

	/**
	 * Sends a byte-length message to the motor controller.
	 * @param message - the message to send
	 * @param description - the string description of the message, for debugging
	 */
	void send(char message, const char *description);

};

#if defined(MOTORHANDLER_DEBUG) || defined(DEBUG)
#define MOTORHANDLER_LOG(info) info
#else
#define MOTORHANDLER_LOG(info)
#endif

#endif /* MOTORHANDLER_H_ */
