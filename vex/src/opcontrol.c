/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */

#define LEFT_FRONT_MOTOR_PORT 4
#define LEFT_BACK_MOTOR_PORT 6
#define RIGHT_FRONT_MOTOR_PORT 3
#define RIGHT_BACK_MOTOR_PORT 5

#define INTAKE_DRUM_MOTOR_PORT 7
#define LAUNCHER_MOTOR_PORT 8

#define LIFT_MOTOR_PORT 9

#define EXTEND_MOTOR_PORT 2

void operatorControl() {
	lcdSetText(uart1, 2, "REDY: Op");
	int left = 0; // 4, 6
	int right = 0; // 3, 5
	bool intake_on = false;
	bool shooter_on = false;
	int lift_pwr = 0;
	int extend_pwr = 0;

	while (1) {
		lcdPrint(uart1, 1, "Batt: %1.3f V", (double)powerLevelMain() / 1000);
		if (joystickGetDigital(1, 6, JOY_DOWN)) {
			intake_on = true;
		} else if (joystickGetDigital(1, 6, JOY_UP)) {
			intake_on = false;
		}

		if (joystickGetDigital(1, 5, JOY_DOWN)) {
			shooter_on = true;
		} else if (joystickGetDigital(1, 5, JOY_UP)) {
			shooter_on = false;
		}

		if (joystickGetDigital(1, 8, JOY_UP)) {
			lift_pwr = -127;
		} else if (joystickGetDigital(1, 8, JOY_LEFT) || joystickGetDigital(1, 8, JOY_RIGHT)) {
			lift_pwr = 0;
		} else if (joystickGetDigital(1, 8, JOY_DOWN)) {
			lift_pwr = 127;
		}

		if (joystickGetDigital(1, 7, JOY_UP)) {
			extend_pwr = -127;
		} else if (joystickGetDigital(1, 7, JOY_LEFT) || joystickGetDigital(1, 7, JOY_RIGHT)) {
			extend_pwr = 0;
		} else if (joystickGetDigital(1, 7, JOY_DOWN)) {
			extend_pwr = 127;
		}

		right = joystickGetAnalog(1, 2);
		left = joystickGetAnalog(1, 3);

		motorSet(LEFT_FRONT_MOTOR_PORT, left);
		motorSet(LEFT_BACK_MOTOR_PORT, left);
		motorSet(RIGHT_FRONT_MOTOR_PORT, right);
		motorSet(RIGHT_BACK_MOTOR_PORT, right);
		if (intake_on) {
			motorSet(INTAKE_DRUM_MOTOR_PORT, -127);
		} else {
			motorSet(INTAKE_DRUM_MOTOR_PORT, 0);
		}
		if (shooter_on) {
			motorSet(LAUNCHER_MOTOR_PORT, -127);
		} else {
			motorSet(LAUNCHER_MOTOR_PORT, 0);
		}
		motorSet(LIFT_MOTOR_PORT, lift_pwr);
		motorSet(EXTEND_MOTOR_PORT, extend_pwr);

		delay (20);
	}

	while (1) { continue;	}
}
