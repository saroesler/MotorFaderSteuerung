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

#if TYPE == MOTOR_FADER

#define VOR FRONT
#define RUECK BACK

void startMotor(uint8_t num, uint8_t direction);
void stopMotor(uint8_t num);
void pauseMotor();
void replayMotor();
void stopAll();
void setMotorStop(uint8_t num);
void setMotorStart(uint8_t num, uint8_t direction);
void applyMotorChanges(void);
#endif

#endif /* MOTOR_H_ */
