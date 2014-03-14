/*
 * MotorHandler.h
 *
 *  Created on: 2014-03-03
 *      Author: gongal
 */

#ifndef MOTORHANDLER_H_
#define MOTORHANDLER_H_
#define MOTOR_START_BYTE 0x80

#define MOTOR_DEVICE_TYPE 0x00
#define MOTOR_MOTOR2_BACKWARD 0x04 // motor 2 backward
#define MOTOR_MOTOR3_BACKWARD 0x06 //motor 3 backward
#define MOTOR_MOTOR2_FORWARD 0x05 // motor 2 forward
#define MOTOR_MOTOR3_FORWARD 0x07 // motor 3 forward
#define MOTOR_CONST_SPEED 0x5F
#define MOTOR_STOP_SPEED 0x00

class MotorHandler {
public:
	MotorHandler();
	virtual ~MotorHandler();
	void sendByteMC(char msg);
	void mc_forward();
	void mc_backward();
	void mc_right();
	void mc_left();
	void mc_stop();
};

#endif /* MOTORHANDLER_H_ */
