/*
 * uart.h
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 */

#ifndef UART_H_
#define UART_H_

#include "../main.h"

//Zähler um zusendene Nachricht zu durchwandern
extern volatile uint8_t iOutMessage;
//Länge der zusendenen Nachricht
extern volatile uint8_t numOutMessage;

//Zähler um zusendene Nachricht zu durchwandern
extern volatile uint8_t iInMessage;
//Länge der zusendenen Nachricht
extern volatile uint8_t numInMessage;

extern volatile uint8_t interrupt;


extern volatile char outMessage[20];
extern volatile char inMessage[20];

#define BAUD 9600

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.

void uart_init(void);
void sendMessage(void);

#endif /* UART_H_ */
