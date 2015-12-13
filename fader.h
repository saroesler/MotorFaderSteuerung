/*
 * fader.h
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */

#ifndef FADER_H_
#define FADER_H_

#include "ADC_MCP/adc_mcp.h"
#include "main.h"

//mask defines
#define NEWVALUE 0


/*
 *  faderstruct
 *  Struktur zur ansteuerung eines Faders
 */
//mode defines
#define READY 0				//keine Aufgabe
#define WAITFORTEST 1 		//wartet, dass Testimpuls ausgeführt wird
#define TESTIMPULS	2		//Testimpuls wird ausgeführt
#define TESTEVALUATION 3	//Auswertung des Testimpulses
#define WAITFORRUN	4		//wartet, dass Position angefahren wird
#define RUN	5				//Position wird angefahren
#define ENDCONTROLL	6		//Fader muss kontrolliert werden
#define SLEEP		7		//Fader muss kontrolliert werden

//direction flags
#define BACK 0		//Zurück fahren
#define FRONT 1		//vor fahren

//flag positions
#define DIRECTION 0
#define INITIALIZE 1
#define FINDMAX 2
#define FINDMIN 3

//Flags, die verhindern, dass Motoränderungen zurückgesendet werden.
#define CLEARDATA0 6		//gesetzt,motor steht und Änderungen übersprungen werden
#define CLEARDATA1 7		//gesetzt, wenn Daten Änderung nicht von Motor hervorgerufen.


#define STARTFACTOR 10000.0	//start faktor
#define SLEEPCYCLES 147456000	//zyklen, die der Fader schläft

typedef struct{
	uint8_t position;
	uint8_t startposition;
	float factor;
	uint32_t cycles;	//Zyklen für diesen Vorgang
	uint8_t mode;
	uint8_t flag;
	uint8_t maxvalue;
	uint8_t minvalue;
	uint8_t errorcounter;
} faderstruct;

#define STARTTIMER TCCR1B |= (1<<CS11) | (1<<CS10)
#define STOPTIMER TCCR1B &= ~((1<<CS11) | (1<<CS10))

extern faderstruct fader[CHANNEL];
extern volatile uint8_t numFaderRunning;	//number of running fader
extern volatile uint8_t timerReady;

void initFader();
uint8_t readFader(uint8_t pin, uint8_t* mask);
uint16_t testFader(uint8_t i);
void workFader();
void gotoPosition(uint8_t i, uint8_t pos);
void setTimer(uint8_t startnum);

#endif /* FADER_H_ */
