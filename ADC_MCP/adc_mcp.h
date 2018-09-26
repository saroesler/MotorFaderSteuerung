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
#include "../type.h"

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define DD_MOSI 5
#define DD_SCK 3
#define DD_SS1 2
#define DD_SS2 1


/*
 * Array flags
 */
#define NEWVALUEFLAG 0	//get new value by the adc
#define ADCNEWVALUE 1	//the value, read by the adc last time

/*
#define ACT 0			//actual debounced value
#define OLD 1			//previous debounced value
#define CHANGED 2		//get new debounced value to work it
#define TEMPVALUE 5		//last value, without debounce (de:Entprellen)
#define COUNTER 6		//debounce counter (de:Entprellen)
#define ADCREAL 7		//unbereinigte Messwerte (ohne max- min Anpassung)
#define MIN 8		//unbereinigte Messwerte (ohne max- min Anpassung)
#define MAX 9		//unbereinigte Messwerte (ohne max- min Anpassung)*/


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

//extern uint8_t adcData[CHANNEL][10];
extern volatile uint8_t adcValue[CHANNEL][2];
//extern uint8_t dummyTest;

#endif /* ADC_MCP_H_ */
