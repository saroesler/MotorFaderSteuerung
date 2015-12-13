/*
 * fader.c
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */

#include "fader.h"

faderstruct fader[CHANNEL];
volatile uint8_t numFaderRunning = 0;	//number of running fader
volatile uint8_t timerReady = 0;

uint8_t oldValue = 0;

/*
 * initialisert Fader
 * soll 100 Schritte fahren
 */
void initFader(void){
	for(uint8_t i = 0; i < CHANNEL; i ++){
		uint8_t mask = 0;
		uint8_t startValue = readFader(i, &mask);
		if(startValue > 122){
			fader[i].position = startValue - 100;
			fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen
		} else {
			fader[i].flag |= (1<<DIRECTION);	//vorlaufen
		}

		fader[i].flag |= (1<<INITIALIZE);
		fader[i].startposition = 0;
		fader[i].cycles = 65530;
		fader[i].factor = STARTFACTOR;
		fader[i].mode = WAITFORRUN;
	}

	numFaderRunning = 0;

	// Timer 0 konfigurieren
	TCCR1B = 0;
	OCR1A = 18432;
	TIMSK |= (1<<OCIE1A);

	// Global Interrupts aktivieren
	sei();
}

void workFader(){
	for(uint8_t i = 0; i < CHANNEL; i ++){

#ifdef DEBUG
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
		UDR = fader[i].position;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = adcData[i][ADCNEWVALUE];
#endif

		switch(fader[i].mode){
			case WAITFORTEST:
			case WAITFORRUN:
				//wenn noch ein fader arbeiten darf und dieser auch noch zyklen hat
				if(numFaderRunning < PARALLELFADER && fader[i].cycles > 0){
					//in Ausfürhmodus gehen
					fader[i].mode ++;
					fader[i].startposition = adcData[i][ADCNEWVALUE];
					setTimer(i);
					startMotor(i,fader[i].flag & (1<<DIRECTION));
					numFaderRunning ++;
				}
				break;
			case TESTEVALUATION:
				//wenn fader festgehalten wird
				if(((fader[i].flag & (1<<DIRECTION) )&& (adcData[i][ADCNEWVALUE] < (fader[i].startposition + 9)))
						|| ((!fader[i].flag & (1<<DIRECTION)) && (adcData[i][ADCNEWVALUE] > (fader[i].startposition - 9)))){
					fader[i].startposition = adcData[i][ADCNEWVALUE];
					fader[i].mode = SLEEP;
					fader[i].cycles = SLEEPCYCLES;

					setTimer(i);

				} else {
					//Wenn Motor zurückläuft
					if(!fader[i].flag & (1<<DIRECTION))
						fader[i].cycles = fader[i].factor * ((fader[i].startposition - 10) - fader[i].position);
					else
						fader[i].cycles = fader[i].factor * (fader[i].position - (fader[i].startposition + 10));
					fader[i].mode = WAITFORRUN;
				}
				break;
			case ENDCONTROLL:
				if(adcData[i][ADCNEWVALUE] == fader[i].position)
					fader[i].mode = READY;
				else {
					fader[i].startposition = adcData[i][ADCNEWVALUE];
					fader[i].mode = WAITFORRUN;

					if(adcData[i][ADCNEWVALUE] < fader[i].position){
						fader[i].flag |= (1<<DIRECTION);	//vorlaufen
						fader[i].cycles = fader[i].factor * (fader[i].position - fader[i].startposition) * 2;
					}
					else {
						fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen
						fader[i].cycles = fader[i].factor * (fader[i].startposition - fader[i].position) * 2;
					}
				}
				break;
			case RUN:
			case TESTIMPULS:
				if((fader[i].startposition != 0 && adcData[i][ADCNEWVALUE] == 0) || (fader[i].startposition != 255 && adcData[i][ADCNEWVALUE] == 255)){
					//fader anhalten, wenn ende erreicht
					STOPTIMER;
					fader[i].cycles = TCNT1;
					setTimer(CHANNEL);
				}
				break;
		}
	}
}

ISR(TIMER1_COMPA_vect){
	STOPTIMER;
	//setTimer, kein Startkanal
	//setTimer(CHANNEL);
	timerReady = 1;
}
/*
 * Setzt den Timer neu und berechet neue Zyklen
 */
void setTimer(uint8_t startnum){
	//stop timer
	STOPTIMER;

	cli();
	uint16_t counterState = 0;

	//Wenn timer abgelaufen
	if(timerReady)
		counterState = OCR1A;
	//wenn timer angehalten
	else
		counterState = TCNT1;

	sei();

	//Zyklen erneuern
	for(uint8_t i = 0; i < CHANNEL; i ++){

#ifdef DEBUG3
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
		UDR = fader[i].position;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = adcData[i][ADCNEWVALUE];
#endif

		if(fader[i].cycles > 0 && (fader[i].mode == SLEEP || fader[i].mode == RUN || fader[i].mode == TESTIMPULS) && i != startnum){
			fader[i].cycles -= counterState;

			//Nächsten schritt einleiten, wenn fertig
			if(fader[i].cycles == 0){

				outMessage[0] = '\0';
				switch(fader[i].mode){
					case SLEEP:
						fader[i].startposition = adcData[i][ADCNEWVALUE];
						fader[i].mode = WAITFORTEST;

						if(adcData[i][ACT] < fader[i].position){
							fader[i].flag |= (1<<DIRECTION);	//vorlaufen
							fader[i].cycles = fader[i].factor * 10;
						}
						else {
							fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen
							fader[i].cycles = fader[i].factor * 10;
						}
						break;
					case RUN:
						stopMotor(i);
						numFaderRunning --;


						//Zum Beginn wird der Faktor berechnet
						if(fader[i].flag & (1<<INITIALIZE)){

							//Erfolg der Kallibirierung testen
							if(adcData[i][ADCNEWVALUE] == 0 || adcData[i][ADCNEWVALUE] == 255){
								//Kallibrierung stieß an 0, noch einmal starten
								if(adcData[i][ADCNEWVALUE])
									fader[i].flag &= ~(1<<DIRECTION);	//vorlaufen
								else
									fader[i].flag |= (1<<DIRECTION);	//vorlaufen


								fader[i].startposition = adcData[i][ADCNEWVALUE];

								fader[i].flag |= (1<<INITIALIZE);
								fader[i].cycles = 65530;
								fader[i].factor = STARTFACTOR;
								fader[i].mode = WAITFORRUN;
							} else {

								//Faktor zum Fahren berechnen
								fader[i].factor = (65530.0) / (adcData[i][ADCNEWVALUE] - fader[i].startposition);
								if( fader[i].factor < 0)
									fader[i].factor *= -1;

								//Nächsten Schritt vorbereiten
								fader[i].startposition = adcData[i][ADCNEWVALUE];
								fader[i].position = 0;
								fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen

								if(adcData[i][ADCNEWVALUE] > 10){
									//Testlauf
									fader[i].cycles = fader[i].factor * 10;
									fader[i].mode = WAITFORTEST;
								} else {
									//Direkt anfahren
									fader[i].cycles = fader[i].factor * adcData[i][ADCNEWVALUE];
									fader[i].mode = WAITFORRUN;
								}

								fader[i].flag&=~(1<<INITIALIZE);

							}
						} else if(fader[i].flag & (1<<FINDMAX)){

						} else if(fader[i].flag & (1<<FINDMIN)){

						} else {
							//normales fahren
							fader[i].mode = ENDCONTROLL;
						}
						break;
					case TESTIMPULS:
						stopMotor(i);
						numFaderRunning --;

						fader[i].startposition = adcData[i][ADCNEWVALUE];
						fader[i].mode = WAITFORRUN;

						if(adcData[i][ACT] < fader[i].position){
							fader[i].flag |= (1<<DIRECTION);	//vorlaufen
							fader[i].cycles = fader[i].factor * (fader[i].position - fader[i].startposition);
						}
						else {
							fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen
							fader[i].cycles = fader[i].factor * (fader[i].startposition - fader[i].position);
						}
						break;

				}
			}
		}
	}


	//kleinsten Wert ermitteln
	uint16_t shorts = 0xffff;
	uint8_t timerActive = 0;
	for(uint8_t i = 0; i < CHANNEL; i ++){
		if(fader[i].cycles < shorts && fader[i].cycles > 0){
			shorts = fader[i].cycles;
		}
		if(fader[i].cycles > 0)
			timerActive = 1;
	}

	//start timer
	if(timerActive){
		OCR1A = shorts;
		TCNT1 = 0;
		STARTTIMER;
	}
}

void gotoPosition(uint8_t i, uint8_t pos){

	//wenn schon an position
	if(pos == adcData[i][ADCNEWVALUE])
		return;

	fader[i].startposition = adcData[i][ADCNEWVALUE];
	fader[i].position = pos;

	fader[i].factor = 100;

	if(pos < fader[i].startposition)
		fader[i].flag &= ~(1<<DIRECTION);	//zurücklaufen
	else
		fader[i].flag |= (1<<DIRECTION);	//vorwärts

	//Betrag ist kleiner 10
	if( (((pos - adcData[i][ADCNEWVALUE]) > 10) &&( pos > adcData[i][ADCNEWVALUE]))
			|| ((( adcData[i][ADCNEWVALUE] - pos) > 10) &&( pos < adcData[i][ADCNEWVALUE]))){
		//Testlauf
		fader[i].cycles = fader[i].factor * 10;
		fader[i].mode = WAITFORTEST;
	} else {
		//Direkt anfahren
		fader[i].cycles = fader[i].factor * (pos - adcData[i][ADCNEWVALUE]);
		fader[i].mode = WAITFORRUN;
	}

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
		UDR = fader[i].position;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = adcData[i][ADCNEWVALUE];
#endif
}

uint8_t readFader(uint8_t pin, uint8_t* mask){
	uint8_t counter = 10;
	uint16_t value = 0;

	//loaler alter Wert
	uint8_t thisOldvalue = oldValue;
	*mask &= ~(1<<NEWVALUE);

	do{
		value = readMeassure(0);
		value = value >> 4;
		if(thisOldvalue == value)
			counter ++;
		else{
			counter = 0;
		}
		thisOldvalue = value;
	}while(counter < 10);

	if(oldValue != thisOldvalue)
		*mask |= (1<<NEWVALUE);

	oldValue = thisOldvalue;

	return (uint8_t) value;
}

uint16_t testFader(uint8_t i){
	if(fader[i].mode != READY){
		//daten sperren
		fader[i].flag &= ~((1 << CLEARDATA0) | (1 << CLEARDATA1));
	}
	uint16_t initialized = 0;
	//Neuer Wert
	if(adcData[i][ADCNEWVALUE] != adcData[i][OLD]){
		if(adcData[i][ADCNEWVALUE] == adcData[i][TEMPVALUE]){
			adcData[i][COUNTER] ++;
		} else {
			adcData[i][COUNTER] = 0;
		}

		adcData[i][TEMPVALUE] = adcData[i][ADCNEWVALUE];

		//neuer Wert
		if(adcData[i][COUNTER] == ENTPRELLLEVEL){
			adcData[i][OLD] = adcData[i][ACT];
			adcData[i][ACT] = adcData[i][ADCNEWVALUE];
			adcData[i][CHANGED] = 1;
			adcData[i][NEWVALUEFLAG] = 0;
			adcData[i][COUNTER] = 0;
			initialized |= (1 << i);

			//Motor steht, Daten sind OK
			if(fader[i].mode == READY && (fader[i].flag & (1 << CLEARDATA0)))
				fader[i].flag |= (1 << CLEARDATA1);
			//Motor steht, letzte Änderung
			else if(fader[i].mode == READY )
				fader[i].flag |= (1 << CLEARDATA0);
		}

	}
	else {
		if(adcData[i][ADCNEWVALUE] == 0)
			initialized |= (1 << i);
		adcData[i][COUNTER] = 0;
	}

	return initialized;
}
