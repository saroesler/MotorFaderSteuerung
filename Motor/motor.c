/*
 * motor.c
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */
#include "motor.h"

#if TYPE == MOTOR_FADER
static uint32_t shiftData = 0;
volatile static uint8_t changes = 0;

void startMotor(uint8_t num, uint8_t direction){
	setMotorStart(num, direction);

	shift(shiftData);
	changes = 0;
}

void stopMotor(uint8_t num){
	setMotorStop(num);

	shift(shiftData);
	changes = 0;
}

void pauseMotor(){
	shift(0);
}

void replayMotor(){
	changes = 0;
	shift(shiftData);
}

void stopAll(){
	shiftData = 0;
	changes = 0;
	shift(shiftData);
}

void setMotorStop(uint8_t num){
	shiftData &= ~(1 << (2*num));
	shiftData &= ~(1 << (2*num + 1));
	changes = 1;
}

void setMotorStart(uint8_t num, uint8_t direction){
	if(direction == VOR){
		shiftData |= (1 << (2*num));
		shiftData &= ~(1 << (2* num + 1));
	} else {
		shiftData &= ~(1 << (2*num));
		shiftData |= (1 << (2* num + 1));
	}
	changes = 1;
}

void applyMotorChanges(void){
	if(changes)
		shift(shiftData);
	changes = 0;
}
#endif
