/*
 * motor.c
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */
#include "motor.h"

static uint32_t shiftData = 0;

void startMotor(uint8_t num, uint8_t direction){
	if(direction == VOR){
		shiftData |= (1 << (2*num));
		shiftData &= ~(1 << (2* num + 1));
	} else {
		shiftData &= ~(1 << (2*num));
		shiftData |= (1 << (2* num + 1));
	}

	shift(shiftData);
}

void stopMotor(uint8_t num){
	shiftData &= ~(1 << (2*num));
	shiftData &= ~(1 << (2*num + 1));

	shift(shiftData);
}

void stopAll(){
	shiftData = 0;
	shift(shiftData);
}

