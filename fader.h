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
#include "type.h"

//mask defines
//#define NEWVALUE 0

//valueFlags
#define NEWVALUE		0
#define NEWSTABILVALUE	1

#if TYPE == MOTOR_FADER
/*
 *  faderstruct
 *  Struktur zur ansteuerung eines MotorFaders
 */
//mode defines
#define READY 0				//keine Aufgabe
//#define WAITFORTEST 1 		//wartet, dass Testimpuls ausgeführt wird
//#define TESTIMPULS	2		//Testimpuls wird ausgeführt
//#define TESTEVALUATION 3	//Auswertung des Testimpulses
#define WAITFORRUN	4		//wartet, dass Position angefahren wird
#define RUN			5				//Position wird angefahren
#define CONTROL		6		//Fader muss kontrolliert werden
#define SLEEP		7		//Fader muss kontrolliert werden


//direction flags
#define BACK 0		//Zurück fahren
#define FRONT 1		//vor fahren

//flag positions
#define DIRECTION 0
#define MOTORMOVE 1
//#define INITIALIZE 1
//#define FINDMAX 2
//#define FINDMIN 3

//error flags
//#define PUSHTOP		0//steiß oben an
//#define PUSHBOTTOM	1//steiß unten an


#define MAXRUNNINGERRORS 80		//Anzahl an Fehlanläufen für eine Position, nach dem der Fader neu kalibiert werden soll
#define MAXINNERRUNNINGERRORS 10		//Anzahl an Fehlanläufen für eine Position, nach dem der Fader neu kalibiert werden soll
//#define TESTZYKLEN_FAKTOR 1.1	//Faktor, der bei der Berechnung der Testzyklen eine Rolle spielt
#define STARTSTOPZYKLEN		120	//Zyklen, die benötigt werden, um Fader zu starten / zu stoppen
#define INITCYCLES 100
#define SLEEPCYCLES 230400	//zyklen, die der Fader schläft
#define NEWVALUEPERCENTAGE	20

//#define TESTMOEGLICH	( (((fader[i].zielposition - fader[i].bereinigt) > 15) &&( fader[i].zielposition > fader[i].bereinigt)) || ((( fader[i].bereinigt - fader[i].zielposition) > 15) &&( fader[i].zielposition < fader[i].bereinigt)))
#endif


//Flags, die verhindern, dass Motoränderungen zurückgesendet werden.
//#define CLEARDATAWORK 6		//gesetzt,motor steht und Änderungen übersprungen werden
//#define CLEARDATASEND 7		//gesetzt, wenn Daten Änderung nicht von Motor hervorgerufen.

typedef struct{
#if TYPE == MOTOR_FADER
	uint8_t zielposition;
	//uint8_t returnzielposition;	//position, die nach der Kalibrierung angefahren werden soll
	uint8_t startposition;

	// Berechnung der Fadergeschwindigkeit
	uint8_t lastposition;
	uint16_t time;
	float speed;

	uint16_t divider;
	uint32_t cycles;	//Zyklen für diesen Vorgang
	uint8_t mode;
	uint8_t errorcounter;
	uint8_t innererrorcounter;
	uint8_t motorflags;
	//uint8_t errorflag;
#endif
	uint8_t maxvalue;
	uint8_t minvalue;
	uint8_t adcValue;		//Benötigt für min-/ max-Berechnungen
	uint8_t bereinigt;		//max-min- normiert aber nicht entprellt; für die Arbeit der Motorfader
	uint8_t entprellt;		//Entprellter Werte - kann zum Senden verwendet werden
	uint8_t entprellCounter;//Zähler, um Wert zu entprellen
	uint8_t valueflags;//Entprellter Wert wurde geändert
} faderstruct;

//extern volatile faderstruct fader[CHANNEL];

/*#if TYPE == MOTOR_FADER
//extern volatile uint8_t numFaderRunning;	//number of running fader
//volatile extern uint8_t timerReady;
#endif*/

void initFader(faderstruct * fader);
//void startCalibration(uint8_t i, uint8_t startValue, faderstruct * fader);
//void startAllCalibration(faderstruct * fader);
uint8_t readFader(uint8_t pin);
uint16_t testFader(uint8_t i, faderstruct * fader);
//void setTimer(uint8_t startnum, faderstruct * fader);

#if TYPE == MOTOR_FADER
void workFader(faderstruct * fader);
void gotoPosition(uint8_t i, uint8_t pos, faderstruct * fader);
#include "timer.h"
#endif

#endif /* FADER_H_ */
