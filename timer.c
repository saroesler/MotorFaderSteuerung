
#include "timer.h"

#if TYPE == MOTOR_FADER
#define TIMERFLAG_UPDATE	0
#define TIMERFLAG_COMPLETE	1

#define TIMERFLAG_GLOBCOMPLETE	0
#define TIMERFLAG_READY			1


static volatile uint32_t cycles[CHANNEL];
static volatile uint8_t flags[CHANNEL];
static volatile uint8_t globflag;
static volatile uint8_t timerReady;

/*
 * Wenn der Timer abläuft, setTimer über main starten und
 * alle Motoren pausieren lassen
 */
ISR(TIMER1_COMPA_vect){
	STOPTIMER;
	//pauseMotor();
	//setTimer, kein Startkanal
	globflag |= (1 << TIMERFLAG_READY);
	timerUpdate();
}

void timer_init(void){
	for(uint8_t i = 0; i < CHANNEL; i ++){
		cycles[i] = 0;
		flags[i] = 0;
	}

	globflag = 0;
	timerReady = 0;

	// Timer 0 konfigurieren
	TCCR1B = 0;
	OCR1A = 18432;
	TIMSK |= (1<<OCIE1A);

	// Global Interrupts aktivieren
	sei();
}

void setCycles(uint8_t num, uint32_t mycycles){
	//eintragen und vormerken
	cycles[num] = mycycles;
	flags[num] |= (1 << TIMERFLAG_UPDATE);
}

/*
 * Setzt den Timer neu und berechnet neue Zyklen
 */
void timerUpdate(void){
	//stop timer
	STOPTIMER;
	//cli();
	uint16_t counterState = 0;

	/*
	 * Zeit ermittelt, die der Timer lief
	 */
	//Wenn Timer abgelaufen
	if(globflag & (1 << TIMERFLAG_READY))
		counterState = OCR1A;
	//wenn timer angehalten
	else
		counterState = TCNT1;

	globflag &= ~(1 << TIMERFLAG_READY);

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
		UDR = fader[i].zielposition;

		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{}
		UDR = fader[i].adcValue;
#endif

		/*
		 * Setze die Zyklen von allen laufenden Fadern herab, so sie nicht gerade neu aufgenommen wurden
		 */
		if(cycles[i] > 0 && !(flags[i] & (1 << TIMERFLAG_UPDATE))){

			if(cycles[i] > counterState){
				cycles[i] -= counterState;
			} else {
				cycles[i] = 0;
				// Motor spekulativ anhalten, wenn er bisher lief
				// Falls der Motor bisher stand (sleepmode) ist das auch egal
				setMotorStop(i);

				/*if(fader[i].mode == RUN){

					numFaderRunning --;
				}

				//Fader immer erneut kontrollieren, wenn Aktion abgeschlossen ist
				fader[i].mode = CONTROL;*/

				//Ablauf vermerken
				flags[i] |= (1 << TIMERFLAG_COMPLETE);
				globflag |= (1 << TIMERFLAG_GLOBCOMPLETE);
#ifdef DEBUG_INIT
	while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
	{}
	UDR = '0';
#endif


				/*switch(fader[i].mode){
					/*
					 * SLEEP wird gesetzt, wenn der Fader blockiert ist
					 * nachdem die Zyklen heruntergezählt sind, wird erneut getestet, ob der Fader bewegbar ist.
					 *//*
					case SLEEP:
						fader[i].cycles = fader[i].factor * 15 * TESTZYKLEN_FAKTOR + STARTSTOPZYKLEN;	//Neue Testzyklen
						break;

						/*
						 * Der Motor hat sich die vorgesehene Zeit bewegt, nun folgt die Kontrolle, ob die
						 * Position erreicht wurde. Beim ersten Vorgang wird der Faktor berechnet.
						 *//*
					case TESTIMPULS:
					case RUN:
						stopMotor(i);
						numFaderRunning --;

						fader[i].mode ++;
						break;
				}*/
			}
		}
		//Änderung annehmen
		flags[i] &=~ (1 << TIMERFLAG_UPDATE);
	}

	applyMotorChanges();


	//kleinsten Wert ermitteln
	uint32_t shorts = 0xffff;
	uint8_t timerActive = 0;
	for(uint8_t i = 0; i < CHANNEL; i ++){
		if(cycles[i] < shorts && cycles[i] > 0){
			shorts = cycles[i];
			timerActive = 1;
		}
	}

	//start timer mit neuen Werten und lasse Motor wieder fahren
	if(timerActive){
		OCR1A = (uint16_t) shorts;
		TCNT1 = 0;
		STARTTIMER;

		//replayMotor();
	}
}

uint8_t timerActionHappend(faderstruct* fader){
	if(!(globflag & (1 << TIMERFLAG_GLOBCOMPLETE))){
		//Keine Änderungen
		return 0;
	}

	globflag &= ~(1 << TIMERFLAG_GLOBCOMPLETE);
	uint8_t freemotors = 0;

	// Es gab Änderungen
	for(uint8_t i = 0; i < CHANNEL; i ++){
		if(!(flags[i] & (1 << TIMERFLAG_COMPLETE)))
			//Kanal hat nicht ausgelöst -> nichts zu tun
			continue;

		flags[i] &=~ (1 << TIMERFLAG_COMPLETE);
		if(fader[i].mode == SLEEP || fader[i].mode == RUN){

			// freie Motoren zählen
			if(fader[i].mode == RUN)
				freemotors ++;

			fader[i].mode = CONTROL;
		}

	}
	return freemotors;
}
#endif
