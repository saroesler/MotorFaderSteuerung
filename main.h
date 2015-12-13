/*
 * main.h
 *
 *  Created on: 22.11.2015
 *      Author: sascha
 */

#ifndef MAIN_H_
#define MAIN_H_

#define CHANNEL 2
#define ENTPRELLLEVEL 10
#define PARALLELFADER 4


#define LED 0

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "fader.h"
#include "UART/uart.h"
#include "Motor/motor.h"

#define SLAVEID '1' //Name of this slave

/*
 * SHOWALL ist definiert, wenn alle Änderungen gesendet werden sollen.
 * Wenn SHOWALL nicht definiert ist, werden Änderungen, die durch den
 * Motor entstehen unterdrückt
 */
#define SHOWALL

/*
 * Wenn DEBUG definiert ist, werden DEBUG-Inhalte über den UART gesendet
 */
//#define DEBUG
//#define DEBUG2
//#define DEBUG3

#endif /* MAIN_H_ */
