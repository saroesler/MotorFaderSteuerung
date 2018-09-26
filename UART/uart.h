/*
 * uart.h
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
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

//Sendeerlaubnis
extern volatile uint8_t allowSending;

extern volatile char outMessage[100];
extern volatile char inMessage[20];

#define BAUD 9600

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.


/*
 * this defines are for the uart state wire. If the wire is high, the slaves are allowed to send.
 * They pull down the wire and send. After is, they pull the wire up again.
 */
#define UART_STATE_WIRE_MODE

#define UART_STATE_IN 3
#define UART_STATE_OUT 4
#define UART_STATE_DDR DDRD
#define UART_STATE_PORT PORTD
#define UART_STATE_PIN PIND
#define TXD 1

#define UART_STATE_WIRE_SET	 UART_STATE_PORT |=  (1<<UART_STATE_OUT)
#define UART_STATE_WIRE_FREE UART_STATE_PORT &=~ (1<<UART_STATE_OUT)
#define UART_STATE_WIRE_IS_FREE UART_STATE_PIN &= (1<<UART_STATE_IN)

#define ENABLE_UART_TX UCSRB |= (1<<TXEN) | (1<<TXCIE)
#define DISABLE_UART_TX UCSRB &=~( (1<<TXEN) | (1<<TXCIE))
#define TX_TO_INPUT		UART_STATE_DDR &= ~(1 << TXD)


void uart_init(void);
void sendMessage(void);
uint8_t reserveBus(void);
void freeBus(void);
char* splitMessage(char*, uint8_t*, uint8_t *);

#endif /* UART_H_ */
