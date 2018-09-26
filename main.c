/*
 * main.c
 *
 *  Created on: 23.11.2015
 *      Author: sascha
 */
#include "main.h"

void init(){
	DDRB |= (1<<2);
	PORTB|= (1<<2);

	//Debug LED
	DDRB|= (1<< LED);
	PORTB|= (1<< LED);
}

void minisend(char* data, uint8_t i){
	for(uint8_t j = 0; i < i; j ++){
		while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
		{
		}
		UDR = *(data + j);
	}
}

int main(void){

	faderstruct fader[CHANNEL];
	initSPI();
	uart_init();
	init();
#if TYPE == MOTOR_FADER
	init_shift();
#endif
	initFader(fader);

	_delay_ms(1000);
	outMessage[0] = 'H';
	outMessage[1] = 'A';
	outMessage[2] = 'L';
	outMessage[3] = 'L';
	outMessage[4] = 'O';
	outMessage[5] = '\n';
	outMessage[5] = '\0';
	numOutMessage = 6;
	iOutMessage = 0;

	//Warten bis Bus frei ist
	while(!reserveBus());
	sendMessage();
	//warten bis Nachricht gesendet wurde
	while(numOutMessage != 0);
	//Bus freigeben
	freeBus();

	/*uint8_t mask = 0;
	uint8_t startValue = readFader(0, &mask);
	uint8_t timefactor = 0;

	if(startValue > 122){
		startMotor(0,RUECK);
		for(uint16_t i = 0; i < 100; i ++) _delay_ms(1);
		stopMotor(0);
		timefactor = 100/ (startValue - readFader(0, &mask));
	}
	else{
		startMotor(0,VOR);
		for(uint16_t i = 0; i < 100; i ++)_delay_ms(1);
		stopMotor(0);
		timefactor = 100/ (readFader(0, &mask) - startValue);
		startValue =  readFader(0, &mask);
	}

	startMotor(0,RUECK);
	for(uint32_t i = 0; i < (startValue * timefactor); i ++) _delay_ms(1);
	stopMotor(0);*/

	_delay_ms(1000);

	startMeassure();

	_delay_ms(1000);

#if TYPE == MOTOR_FADER
	uint16_t initref = 0;

	for(uint8_t i = 0; i < CHANNEL; i ++){
		initref |= (1<<i);
	}
#endif

	// initialzied zeigt an, ob stabile ADC-Werte vorliegen
	uint16_t initialzied = 0;
	uint8_t newMessage = 0;

	while(1){


		/*
		 * Uart Empfangen
		 */

		if(numInMessage != 0){

			//_delay_ms(10);
			/*for(uint8_t i = 0; inMessage[i]; i ++){
				outMessage[i] = inMessage[i];
			}

			outMessage[numInMessage-2] = '\0';
			numOutMessage = numInMessage-1;
			while(!reserveBus());
			sendMessage();

			while(numOutMessage != 0);
			freeBus();*/


			switch(inMessage[0]){

#if TYPE == MOTOR_FADER
				/*case 'i':
					startAllCalibration(fader);
					break;*/
				case 'm':
				{
#ifdef UART_INIT
					while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
					{}
					UDR = 'M';
#endif
					//allowSending = 0;

					//Fader auf Position setzen (move)

					//string durchsuchen ';' trennt befehle, ':' trennt fader nummer von fader wert

					//solange nachrichten da sind
					char *pt2 = inMessage + 1;
					while(pt2 != NULL){
						uint8_t channel = 0;
						uint8_t value = 0;
						pt2 = splitMessage(pt2, &channel, &value);
						if(pt2){
							gotoPosition(channel - STARTADDRESS, value, fader);
						}

					}
					break;
				}
#else
				//set fader max level
				case 'n':
				{
					LED_OFF;
					char *pt2 = inMessage + 1;
					while(pt2 != NULL){
						uint8_t channel = 0;
						uint8_t value = 0;
						pt2 = splitMessage(pt2, &channel, &value);
						if(pt2){
							fader[channel].minvalue = value;
						}
					}
					break;
				}
				//set fader min level
				case 'o':
				{
					LED_ON;
					char *pt2 = inMessage + 1;
						while(pt2 != NULL){
							uint8_t channel = 0;
							uint8_t value = 0;
							pt2 = splitMessage(pt2, &channel, &value);
							if(pt2){
								fader[channel].maxvalue = value;
							}
						}
						break;
					}
#endif
#ifndef UART_STATE_WIRE_MODE
				case 's':
					//Slaves können senden
					if(inMessage[1] == SLAVEID)
						allowSending = 1;
					else
						allowSending = 0;
					break;
#endif

			}
			inMessage[0] = '\0';
			numInMessage = 0;
		}

#if TYPE == MOTOR_FADER
		/*
		 * Steuerung der Motorfader
		 * erst wenn stabile ADC Daten von allen Fadern vorliegen
		 */
		if(initialzied == initref){
			// Wenn Timer abgelaufen ist
			/*if(timerReady){
				setTimer(CHANNEL, fader);
				timerReady = 0;
			}*/
			workFader(fader);
		}
#endif

		/*
		 * Fader auslesen und Änderungsstring bereitstellen
		 */
		char Buffer[33] = "";
		for(uint8_t i = 0; i < CHANNEL; i ++){
			//ADC hat neuen Messung durchgeführt
			if(adcValue[i][NEWVALUEFLAG]){
				initialzied |= testFader(i, fader);
				adcValue[i][NEWVALUEFLAG] = 0;

/*#ifdef DEBUG_INIT
				while (!(UCSRA & (1<<UDRE)))  // warten bis Senden moeglich
				{}
				UDR = 'L';
#endif*/
			}
#ifdef SHOWALL
#ifndef UART_STATE_WIRE_MODE
			if(fader[i].entprelledChange == 1 && allowSending && numOutMessage == 0){
#else
			//neuer Wert und Sendestring ist frei
			if(fader[i].entprelledChange == 1 && numOutMessage == 0){
#endif
#else
#ifndef UART_STATE_WIRE_MODE
			if(fader[i].entprelledChange == 1 && fader[i].flag & (1<< CLEARDATA1) && allowSending && numOutMessage == 0){
#else
			if((fader[i].valueflags & (1 << NEWSTABILVALUE)) &&
#if TYPE == MOTOR_FADER
					(fader[i].mode  == READY || fader[i].mode  == SLEEP) &&
#endif
					numOutMessage == 0){
#endif
#endif

				//adcData[i][MIN] = 0;
				//adcData[i][MAX] = 0;
				//Trennzeichen im String
				if(strlen(outMessage) > 0)
					strcat(outMessage, ";");
				itoa(i + STARTADDRESS, Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer
				strcat(outMessage, ":"); 	//sendet :, wenn motor nicht an war
				itoa(fader[i].entprellt, Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer
				strcat(outMessage, "|");	//geht immer
				itoa(fader[i].minvalue, Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer
				strcat(outMessage, "-");	//geht immer
				itoa(fader[i].maxvalue, Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer
				strcat(outMessage, "\n");	//geht immer

				numOutMessage = strlen(outMessage);

				fader[i].valueflags &= ~(1 << NEWSTABILVALUE);

				newMessage = 1;
			}
		}

#ifndef UART_STATE_WIRE_MODE
		if(newMessage && allowSending){{
#else
			//send, if there is a reserved wire for this slave
		if(newMessage){
			//try to block the bus, if success, send
			if(reserveBus()){
#endif
			sendMessage();

			while(numOutMessage != 0);
			newMessage = 0;
			freeBus();
		}}
	}
	return 0;
}

