/*
 * main.h
 *
 *  Created on: 22.11.2015
 *      Author: sascha
 */

#ifndef MAIN_H_
#define MAIN_H_

#define ENTPRELLLEVEL 10
#define PARALLELFADER 2


#define LED 0

#include <avr/io.h>
#include "type.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "fader.h"
#include "UART/uart.h"

#if TYPE == MOTOR_FADER
#include "Motor/motor.h"
#endif

#define SLAVEID '1' //Name of this slave

/*
 * SHOWALL ist definiert, wenn alle Änderungen gesendet werden sollen.
 * Wenn SHOWALL nicht definiert ist, werden Änderungen, die durch den
 * Motor entstehen unterdrückt
 */
//#define SHOWALL

/*
 * Wenn DEBUG definiert ist, werden DEBUG-Inhalte über den UART gesendet
 */
//#define DEBUG
//#define DEBUG2
//#define DEBUG3
//#define DEBUG_INIT

//UART auf DEBUG schalten: (nur ein Slave im Bus)
#define UART_DEBUGMODE

#define LED_OFF PORTB &= ~(1<< LED);
#define LED_ON  PORTB |=  (1<< LED);


#endif /* MAIN_H_ */
