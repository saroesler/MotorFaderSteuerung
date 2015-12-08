/*
 * motor.h
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "../main.h"
#include "shift.h"

#define VOR FRONT
#define RUECK BACK

void startMotor(uint8_t num, uint8_t direction);
void stopMotor(uint8_t num);
void stopAll();

#endif /* MOTOR_H_ */
