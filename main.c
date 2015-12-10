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
	init_shift();
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

	uint16_t initref = 0;

	for(uint8_t i = 0; i < CHANNEL; i ++){
		initref |= (1<<i);
	}

	uint16_t initialzied = 0;
	uint8_t allowSending = 0;
	uint8_t newMessage = 0;
	while(1){


		/*
		 * Uart Empfangen
		 */

		if(uartFlag & (1 << NEWRECEIVED)){

			PORTB &= ~(1<<0);

			_delay_ms(10);

			switch(inMessage[0]){
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

						uint8_t num = atoi(subbuff);

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
				case 's':
					//Slaves können senden
					if(inMessage[1] == SLAVEID)
						allowSending = 1;
					else
						allowSending = 0;
					break;

			}
			inMessage[0] = '\0';
			iInMessage = 0;
			uartFlag &= ~(1 << NEWRECEIVED);
		}

		/*
		 * Steuerung der Fader
		 */
		if(initialzied == initref){
			if(timerReady){
				setTimer(CHANNEL);
				timerReady = 0;
			}
			workFader();
		}

		/*
		 * Fader auslesen und Änderungsstring bereitstellen
		 */
		char Buffer[4] = "";
		for(uint8_t i = 0; i < CHANNEL; i ++){
			if(adcData[i][NEWVALUEFLAG]){
				initialzied |= testFader(i);
				adcData[i][NEWVALUEFLAG] = 0;
			}
#ifdef SHOWALL
			if(adcData[i][CHANGED] == 1 && allowSending){
#else
			if(adcData[i][CHANGED] == 1 && fader[i].flag & (1<< CLEARDATA1) && allowSending){
#endif

				//Trennzeichen im String
				if(strlen(outMessage) > 0)
					strcat(outMessage, ";");
				itoa(i, Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer
				strcat(outMessage, ":"); 	//sendet :, wenn motor nicht an war
				itoa(adcData[i][ACT], Buffer, 10);
				strcat(outMessage, Buffer);	//geht immer

				numOutMessage = strlen(outMessage);

				adcData[i][CHANGED] = 0;

				newMessage = 1;
			}
		}

		if(newMessage && allowSending){
			sendMessage();

			while(!(uartFlag & (1<<SENDET)));
			outMessage[0] = '\0';
			newMessage = 0;
		}
	}
	return 0;
}

