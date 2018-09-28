/*
 * fader.c
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */

#include "fader.h"

uint8_t debugVar = 0;
volatile static uint64_t clock;

//volatile faderstruct fader[CHANNEL];
#if TYPE == MOTOR_FADER
static uint8_t numFaderRunning = 0;	//number of running fader
volatile uint8_t timerReady = 0;
#endif

/*
 * initialisert Fader
 * soll 100 Schritte fahren
 */
void initFader(faderstruct * fader){
	//halte Motoren an
#if TYPE == MOTOR_FADER
	stopAll();
#endif

	for(uint8_t i = 0; i < CHANNEL; i ++){
		fader[i].maxvalue = 255;
		fader[i].minvalue = 0;
		fader[i].adcValue = 0;			//Benötigt für min-/ max-Berechnungen
		fader[i].bereinigt = 0;			//max-min- normiert aber nicht entprellt; für die Arbeit der Motorfader
		fader[i].entprellt = 0;			//Entprellter Werte - kann zum Senden verwendet werden
		fader[i].entprellCounter = 0;	//Zähler, um Wert zu entprellen
		fader[i].valueflags = 0;	//Entprellter Wert wurde geändert

#if TYPE == MOTOR_FADER
		fader[i].zielposition = 0;
		fader[i].startposition = 0;
		fader[i].lastposition = 0;
		fader[i].speed = INITCYCLES;
		fader[i].time = 0;
		fader[i].cycles = 0;
		fader[i].mode = READY;
		fader[i].errorcounter = 0;
		fader[i].innererrorcounter = 0;
		fader[i].motorflags = 0;
		fader[i].divider = 1;
	}


	numFaderRunning = 0;

	timer_init();
#else
	}//end for if ONLY_FADER
#endif
}

/*void initGlobalClock(void){
	clock = 0;

	//Start timer
	TCCR0 |= (1<<CS01) | (1<<CS00);

	// Global Interrupts aktivieren
	sei();
}*/

#if TYPE == MOTOR_FADER
/*void startCalibration(uint8_t i, uint8_t startValue, faderstruct * fader){
	/*
	 * Initialize Motorfader
	 * Die Fader werden kalibriert, indem sie zunächst 65430 Zyklen bewegt werden
	 * draus wird dann errechnet, wie viele Zyklen auf einen gefahrenen Wert kommen (fader[i].cycles)
	 *//*
	fader[i].startposition = startValue;
	if(fader[i].startposition > 122){
		//fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen
		fader[i].zielposition = 0;
	} else {
		//fader[i].flag |= (1<<DIRECTION);	//vorlaufen
		fader[i].zielposition = 255;
	}

	fader[i].flag |= (1<<INITIALIZE);
	fader[i].cycles = INITCYCLES;
	fader[i].factor = 0;
	fader[i].mode = WAITFORRUN;
	fader[i].errorcounter = 0;
}

/*
 * Startet die Kalibrierung aller Fader
 * Die Funktion nutzt fader[i].adcValue. Das setzt voraus, dass dort gültige Werte vorliegen
 * nicht zur Erstkalibierung geeignet
 *//*
void startAllCalibration(faderstruct * fader){
	for(uint8_t i = 0; i < CHANNEL; i ++){
		startCalibration(i, fader[i].adcValue, fader);
	}
}*/

uint8_t setFaderCycles(faderstruct* fader, uint16_t offset){
	uint8_t olddirection = fader->motorflags & (1 << DIRECTION);
	uint8_t dif = 0;
	if(fader->zielposition > fader->bereinigt){
		dif = fader->zielposition - fader->bereinigt;
		fader->motorflags |= (1 << DIRECTION);
	} else {
		dif = fader->bereinigt - fader->zielposition;
		fader->motorflags &= ~(1 << DIRECTION);
	}

	uint8_t directionchange = ((olddirection & (1 << DIRECTION) )^ (fader->motorflags & (1 << DIRECTION)));
	if(directionchange)
		fader->divider = fader->divider << 1;

	// geometrische Reihe: Konvergiert
	if(dif < 30){
		fader->cycles = (uint8_t)((dif * fader->speed + offset) /(fader->divider * fader->divider)) +1;
	} else {
		fader->cycles = dif * fader->speed + offset;
	}
	return directionchange;
}

void workFader(faderstruct * fader){
	// Änderung vom Timer abholen
	numFaderRunning -= timerActionHappend(fader);

	for(uint8_t i = 0; i < CHANNEL; i ++){
		switch(fader[i].mode){
			case RUN:

#ifdef DEBUG_INIT
				while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
				{}
				UDR = 'R';
#endif
				//auf gültigen Wert warten
				if(!(fader[i].valueflags & (1 << NEWVALUE)))
					break;
/*#ifdef DEBUG_INIT
				while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
				{}
				UDR = 'r';
#endif*/

				fader[i].valueflags &= ~(1 << NEWVALUE);

				//Fader ist fertig
				// Fader kommt nicht weiter. Wird er festgehalten?
				if(fader[i].bereinigt == fader[i].zielposition
						|| fader[i].innererrorcounter > MAXINNERRUNNINGERRORS){
					stopMotor(i);
					numFaderRunning --;
					fader[i].cycles = 0;
					fader[i].divider = 1;
					//fader[i].innererrorcounter = 0;
					if(fader[i].bereinigt == fader[i].zielposition){
						fader[i].mode = CONTROL;//TODO
						fader[i].valueflags |= (1 << NEWSTABILVALUE);
					} else {
						fader[i].mode = CONTROL;
					}
#ifdef DEBUG_INIT
					while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
					{}
					UDR = 'f';
#endif

					break;
				}

				//Fader muss noch weiterlaufen
				if(setFaderCycles(&fader[i], 0)){
					// Richtung ggf anpassen
					setMotorStart(i, fader[i].motorflags & (1 << DIRECTION));
				}
				setCycles(i, fader[i].cycles);
				fader[i].motorflags |= (1 << MOTORMOVE);
				break;
			case WAITFORRUN:
#ifdef DEBUG_INIT
					while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
					{}
					UDR = 'W';
#endif
				//aktivieren
				if(numFaderRunning < PARALLELFADER){
					setMotorStart(i, fader[i].motorflags & (1 << DIRECTION));
					setCycles(i, fader[i].cycles);
					fader[i].mode = RUN;
					numFaderRunning ++;
					fader[i].motorflags |= (1 << MOTORMOVE);
#ifdef DEBUG_INIT
					while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
					{}
					UDR = 'S';
#endif
				}
				break;

			case CONTROL:
#ifdef DEBUG_INIT
				while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
				{}
				UDR = 'C';
#endif
				//auf gültigen Wert warten
				if(!(fader[i].valueflags & (1 << NEWVALUE)))
					break;
				fader[i].valueflags &= ~(1 << NEWVALUE);

				//fader ist fertig
				// Fader kommt nicht weiter. Wird er festgehalten?
				// Anhalten und wieder als Handfader betreiben
				if(fader[i].bereinigt == fader[i].zielposition
						|| fader[i].errorcounter > MAXRUNNINGERRORS){
					fader[i].mode = READY;
					fader[i].errorcounter = 0;
					fader[i].innererrorcounter = 0;
#ifdef DEBUG_INIT
					while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
					{}
					UDR = 'F';
#endif
					break;
				}

				//fader kam nicht vorran
				// kurz warten
				if(fader[i].innererrorcounter > MAXINNERRUNNINGERRORS){
					fader[i].innererrorcounter = 0;
					fader[i].mode = SLEEP;
					fader[i].errorcounter ++;
					fader[i].cycles = SLEEPCYCLES;
					setCycles(i, fader[i].cycles);

#ifdef DEBUG_INIT
					while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
					{}
					UDR = 'p';
#endif
					break;
				}

				// Fader soll weiter laufen. Neue Zyklen berechnen und weiter
				setFaderCycles(&fader[i], STARTSTOPZYKLEN);
				fader[i].mode = WAITFORRUN;

				break;
		}
	}
	timerUpdate();
	applyMotorChanges();
}

void gotoPosition(uint8_t i, uint8_t pos, faderstruct * fader){
#ifdef DEBUG_INIT
	while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
	{}
	UDR = 'G';
#endif
	/*
	 * Berechne neue Position
	 * Wenn nicht der volle Wertebereicht genutzt wird (maxvalue != 255 oder minvlaue != 0),
	 * dann können einige Positionen nicht erreicht werden. Es wird dann ein neuer Wert berechnet
	 * Dieser ist die nächstkleinere erreichbare Position
	 */

	//Offset an minvalue kann igoriert werden
	/*double tmp =  255.0 / (fader[i].maxvalue - fader[i].minvalue);
	uint8_t temp = pos / tmp;
	pos = (temp) * tmp;*/

	//wenn schon an position
	if(pos == fader[i].bereinigt)
		return;

	//prüfen ob Fader noch kalibriert wurd
	/*if(fader[i].flag & ((1<<INITIALIZE) | (1<<FINDMAX) | (1<<FINDMIN)))
		return;*/

#ifdef DEBUG_INIT
	while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
	{}
	UDR = 'P';
#endif

	fader[i].startposition = fader[i].bereinigt;
	fader[i].lastposition = fader[i].bereinigt;
	fader[i].zielposition = pos;

	fader[i].divider = 1;

	//fader[i].factor = 100;

	/*if(pos < fader[i].startposition)
		fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen
	else
		fader[i].flag |= (1<<DIRECTION);	//vorwärts*/

	//Betrag ist größer 15
	/*if(TESTMOEGLICH){
		//Testlauf
		fader[i].cycles = fader[i].factor * 15 * TESTZYKLEN_FAKTOR + STARTSTOPZYKLEN;
		fader[i].mode = WAITFORTEST;

#ifdef DEBUG_INIT
		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = 'T';
#endif
	} else {*/

	if(fader[i].mode == RUN){
		//Motor kurz anhalten
		stopMotor(i);
		setCycles(i, 0);	// Timer interrupt verhindern
		numFaderRunning --;	// Motor austragen
	}

	//Direkt anfahren
	setFaderCycles(&fader[i], STARTSTOPZYKLEN);
	fader[i].mode = WAITFORRUN;
	//}

#ifdef DEBUG2
		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{
		}
		UDR = fader[i].mode;
		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = fader[i].cycles;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = fader[i].factor;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = fader[i].startposition;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = fader[i].zielposition;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = adcData[i][ADCNEWVALUE];
#endif
}
#endif //end MOTOR_FADER

uint8_t readFader(uint8_t pin){
	uint8_t counter = 0;
	uint16_t value = 0;

	do{
		uint16_t newvalue = readMeassure(0);
		newvalue = newvalue >> 4;
		if(newvalue == value)
			counter ++;
		else{
			counter = 0;
		}
		value = newvalue;
	}while(counter < ENTPRELLLEVEL);

	return (uint8_t) value;
}

uint16_t testFader(uint8_t i, faderstruct * fader){
/*#if TYPE == MOTOR_FADER
	if(fader[i].mode != READY){
		//daten sperren
		fader[i].flag &= ~(1 << CLEARDATASEND);
	}
	if(fader[i].mode == RUN || fader[i].mode == WAITFORRUN || fader[i].mode == TESTIMPULS || fader[i].mode == WAITFORTEST){
		//daten sperren
		fader[i].flag &= ~(1 << CLEARDATAWORK);
	}
#endif*/
	uint16_t initialized = 0;

	fader[i].adcValue = adcValue[i][ADCNEWVALUE];	//Rohdaten zwischenspeichern (für Max-Min-Messungen)
	adcValue[i][NEWVALUEFLAG] = 0;

	/*
	 * Messwert bereinigen
	 * Maximalwert und Offset herausrechnen
	 */

	/*double tmp =  255.0 / (fader[i].maxvalue - fader[i].minvalue);
	uint8_t bereinigt = (fader[i].adcValue - fader[i].minvalue) * tmp;

	//unterlauf
	if(fader[i].adcValue < fader[i].minvalue)
		bereinigt = 0;
	//overflow
	if(fader[i].adcValue > fader[i].maxvalue)
		bereinigt = 255;*/

	uint8_t bereinigt = fader[i].adcValue;
	//Wert weicht vom letzten entprellten Wert ab
	if(bereinigt  != fader[i].entprellt ){
		//Wert weicht auch von letzter Messung ab
		if(fader[i].bereinigt == bereinigt){
			fader[i].entprellCounter ++;
		} else {
			fader[i].entprellCounter = 0;
		}

		//Entprellen
		if(fader[i].entprellCounter == ENTPRELLLEVEL){
			fader[i].entprellt = fader[i].bereinigt;
			fader[i].valueflags |= (1 << NEWSTABILVALUE);
			fader[i].entprellCounter = 0;

			initialized |= (1 << i);

#if TYPE == MOTOR_FADER
			//Motor steht, letzte Änderung
			//if(fader[i].mode == ENDCONTROLL )
			//	fader[i].flag |= (1 << CLEARDATAWORK);
#endif
		}

	}
	//Wert ist gleich entprellten Wert
	else {
		fader[i].entprellCounter = 0;
		if(fader[i].bereinigt == 0)
			initialized |= (1 << i);
/*#if TYPE == MOTOR_FADER
		//Motor steht, Daten sind OK
		if(!(fader[i].mode == RUN || fader[i].mode == WAITFORRUN || fader[i].mode == TESTIMPULS || fader[i].mode == WAITFORTEST))
			fader[i].flag |= (1 << CLEARDATAWORK);
		if(fader[i].mode == READY && (fader[i].flag & (1 << CLEARDATAWORK)))
			fader[i].flag |= (1 << CLEARDATASEND);
#endif*/
	}

	// Neue Messung
	fader[i].valueflags |= (1 << NEWVALUE);

	if(fader[i].mode == RUN){
		fader[i].time +=24;		// Wenn Fader mit 9600Hz gelesen wird, dann sind das 24 Timer Takte
	}

	if(bereinigt != fader[i].bereinigt){
#if TYPE == MOTOR_FADER

		//Geschwindigkeit berechnen, wenn der Motor an ist
		if(fader[i].mode == RUN){
			float s = fader[i].bereinigt;
			s -= fader[i].lastposition;
			if(s < 0)
				s *= -1;
			float newspeed = fader[i].time / s;
			fader[i].time = 0;
			fader[i].speed = fader[i].speed * (1- NEWVALUEPERCENTAGE) + NEWVALUEPERCENTAGE * newspeed;

		}
		// Wert hat sich geändert. Speichern für Motorfader
		fader[i].lastposition = fader[i].bereinigt;
		fader[i].innererrorcounter = 0;
		fader[i].motorflags &= ~(1 << MOTORMOVE);
#endif
		fader[i].bereinigt = bereinigt;
	} else {
#if TYPE == MOTOR_FADER
		//Keine Änderung. Wichtig für Motorfader!
		//Fehler, wenn Motor bewegt wurde, aber keine Änderung vorliegt
		if(fader[i].motorflags & (1 << MOTORMOVE))
			fader[i].innererrorcounter ++;
		fader[i].motorflags &= ~(1 << MOTORMOVE);
#endif
	}

	return initialized;
}
