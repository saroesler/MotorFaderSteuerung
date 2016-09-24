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
	initSPI();
	uart_init();
	init();
#if TYPE == MOTOR_FADER
	init_shift();
#endif
	initFader();

	_delay_ms(1000);
	outMessage[0] = 'H';
	outMessage[1] = 'A';
	outMessage[2] = 'L';
	outMessage[3] = 'L';
	outMessage[4] = 'O';
	outMessage[5] = '\0';
	numOutMessage = 5;
	iOutMessage = 0;
	sendMessage();

	//while(!(uartFlag & (1<<SENDET)));TODO

	_delay_ms(1000);
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

	uint16_t initialzied = 0;
	uint8_t newMessage = 0;
	while(1){


		/*
		 * Uart Empfangen
		 */

		if(numInMessage != 0){

			PORTB &= ~(1<<0);

			//_delay_ms(10);

			switch(inMessage[0]){

#if TYPE == MOTOR_FADER
				case 'm':
					allowSending = 0;

					//Fader auf Position setzen (move)

					//string durchsuchen ';' trennt befehle, ':' trennt fader nummer von fader wert

					//solange nachrichten da sind
					char* pt2 = inMessage + 1;
					while(pt2 != NULL){

						//':'-suchen
						char* pt1 = strstr(pt2, ":");

						//Bei Fehler abbrechen
						if(pt1 == NULL)
							break;

						//Fadernummer ausschneiden
						char subbuff[5];
						strncpy( subbuff, pt2, (pt1 - pt2) );
						subbuff[4] = '\0';

						uint8_t num = atoi(subbuff) - STARTADDRESS;

						//Faderwert ausschneiden
						pt2 = strstr(pt1, ";");

						if(pt2 == NULL){
							strncpy( subbuff, (pt1 + 1), (inMessage + numInMessage - pt1 -1) );
						} else {
							strncpy( subbuff, (pt1 + 1), (pt2 - pt1 -1) );
						}
						subbuff[4] = '\0';

						uint8_t value = atoi(subbuff);

						gotoPosition(num, value);
					}
					break;
#else
				//set fader max level
				case 'n':
					break;
				//set fader min level
				case 'o':
					break;
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
		 */
		if(initialzied == initref){
			if(timerReady){
				setTimer(CHANNEL);
				timerReady = 0;
			}
			workFader();
		}
#endif

		/*
		 * Fader auslesen und Änderungsstring bereitstellen
		 */
		char Buffer[4] = "";
		for(uint8_t i = 0; i < CHANNEL; i ++){
			//Initializes Motorfader
			if(adcData[i][NEWVALUEFLAG]){
				initialzied |= testFader(i);
				adcData[i][NEWVALUEFLAG] = 0;
			}
#ifdef SHOWALL
#ifndef UART_STATE_WIRE_MODE
			if(adcData[i][CHANGED] == 1 && allowSending && numOutMessage == 0){
#else
			if(adcData[i][CHANGED] == 1 && numOutMessage == 0){
#endif
#else
#ifndef UART_STATE_WIRE_MODE
			if(adcData[i][CHANGED] == 1 && fader[i].flag & (1<< CLEARDATA1) && allowSending && numOutMessage == 0){
#else
			if(adcData[i][CHANGED] == 1 && fader[i].flag & (1<< CLEARDATA1) && numOutMessage == 0){
#endif
#endif

				//Trennzeichen im String
				if(strlen(outMessage) > 0)
					strcat(outMessage, ";");
				itoa(i + STARTADDRESS, Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer
				strcat(outMessage, ":"); 	//sendet :, wenn motor nicht an war
				itoa(adcData[i][ACT], Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer

				numOutMessage = strlen(outMessage);

				adcData[i][CHANGED] = 0;

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

