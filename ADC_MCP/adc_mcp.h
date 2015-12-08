/*
 * adc_mcp.h
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 *
 *  Auswerten der AD-Wandler MCP3208 und MCP 3204
 */

#ifndef ADC_MCP_H_
#define ADC_MCP_H_

#include "../main.h"

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define DD_MOSI 5
#define DD_SCK 3
#define DD_SS 2

/*
 * Array flags
 */
#define ACT 0
#define OLD 1
#define CHANGED 2
#define NEWVALUEFLAG 3
#define ADCNEWVALUE 4
#define TEMPVALUE 5
#define COUNTER 6

void initSPI(void);

/*
 * read measure of ADC by an interrupt
 */
void startMeassure();
uint8_t newMessage(void);
uint8_t getValue(void);

/**
 * read measure of the ADC
 * this waits until message is receiving
 */
uint16_t readMeassure(uint8_t pin);

extern volatile uint8_t adcData[CHANNEL][7];

#endif /* ADC_MCP_H_ */
