/*
 * shift.h
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */

#ifndef SHIFT_H_
#define SHIFT_H_

#include "../main.h"

#if TYPE == MOTOR_FADER
#define SHIFT_DDR DDRC
#define SHIFT_PORT PORTC

#define SHIFT_DATA 3
#define SHIFT_PS 5
#define SHIFT_CLK 4

void shift();
void init_shift();
#endif
#endif /* SHIFT_H_ */
