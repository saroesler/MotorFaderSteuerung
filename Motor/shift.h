/*
 * shift.h
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */

#ifndef SHIFT_H_
#define SHIFT_H_

#include "../main.h"

#define SHIFT_DDR DDRC
#define SHIFT_PORT PORTC

#define SHIFT_DATA 2
#define SHIFT_PS 1
#define SHIFT_CLK 0

void shift();
void init_shift();

#endif /* SHIFT_H_ */
