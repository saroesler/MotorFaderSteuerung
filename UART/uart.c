/*
 * uart.c
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 */

#include "../main.h"
#include "uartVar.h"
#include "uart.h"

volatile uint8_t interrupt = 1;
volatile uint8_t puffercount = 0;

ISR(USART_RXC_vect)
{
	char data = UDR;

	//nur speichern, wenn Wartschlange frei ist
	if((data == '\0' || data == 0x0A || (data >= 0x20 && data <= 0x7d)) && (numInMessage[puffercount] == 0)){
		inMessage[puffercount][iInMessage] = data;
		iInMessage ++;

		if(data == '\0' || data == 0x0A){
			inMessage[puffercount][iInMessage] = '\0';
			numInMessage[puffercount] = iInMessage;
			iInMessage = 0;

			//suche nächsten freien Puffer
			uint8_t i = puffercount +1;
			while(i != puffercount){
				if(inMessage[i][0] == '\0')
					break;
				i ++;
				if(i >= NUMINPUFFER)
					i = 0;
			}
			puffercount = i;
		}
	} else {
		//suche nächsten freien Puffer
		uint8_t i = puffercount +1;
		while(i != puffercount){
			if(inMessage[i][0] == '\0')
				break;
			i ++;
			if(i >= NUMINPUFFER)
				i = 0;
		}
		puffercount = i;
	}
}

ISR(USART_TXC_vect)
{
	//ein datenpacket senden
	if(iOutMessage < numOutMessage){
		//nächstes Byte senden
		UDR = outMessage[iOutMessage ++];
	// schluss senden
	} else if(iOutMessage == numOutMessage){
		//Ende senden
		iOutMessage ++;
		UDR = '\0';
	//Ende senden
	} else {
		iOutMessage = 0;
		numOutMessage = 0;
		outMessage[0] = '\0';
	}
}

void uart_init(void)
{
	/*
	 * Initialize uart
	 */
	UBRRH = 0x00;//UBRR_VAL >> 8;
	UBRRL = 95;//UBRR_VAL & 0xFF;

	UCSRB |= (1<<RXEN) | (1<<RXCIE);  			// UART Receiver einschalten
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  	// Asynchron 8N1

	/*
	 * Initialize uart state wire
	 */
	UART_STATE_DDR |=  (1 << UART_STATE_OUT);
	UART_STATE_DDR &= ~(1 << UART_STATE_IN);

	for(uint8_t i = 0; i < NUMINPUFFER; i ++){
		inMessage[i][0] = '\0';
		numInMessage[i] = 0;
	}
#ifdef UART_DEBUGMODE
	ENABLE_UART_TX;				//enable uart transmitter
#endif
	sei();
}

//start senden
void sendMessage(void){
	numOutMessage = strlen(outMessage);
	if(numOutMessage > 0 && iOutMessage == 0){
		UDR = outMessage[iOutMessage];
		iOutMessage = 1;
	}
}

/*
 * block bus for other slaves
 * return 1 if success
 */
uint8_t reserveBus(void){
#ifndef UART_DEBUGMODE
	//check if there is not another slave sending
	if(UART_STATE_WIRE_IS_FREE){
		UART_STATE_WIRE_SET;		//block bus
		ENABLE_UART_TX;				//enable uart transmitter
		sei();

		//ready for sending
		return 1;
	} else {
		//another slave is sending right now, cant block bus
		return 0;
	}
#else
	return 1;
#endif
}

void freeBus(void){
#ifndef UART_DEBUGMODE
	DISABLE_UART_TX;		//shutdown uart transmitter
	TX_TO_INPUT;			//set tx-pin to high impedance
	UART_STATE_WIRE_FREE;
#endif
}

/*
 * The function splits a channel number and a value out of a message
 * It returns only one pair of value. It returns the first pair of value,
 * get after the begin pointer.
 * The function sets the start pointer to the first character of next pair of value.
 * If it is NULL, its the end of the string. Value and channel number are usable.
 * If there is an error it returns NULL.
 */
char* splitMessage(char* begin, uint8_t* channel, uint8_t* value, volatile char* message, volatile uint8_t* numinmessage){
	//':'-suchen - Trennzeichen zwischen Kanal und Wert
	char* posDoppelPunkt = strstr(begin, ":");

	//Bei Fehler abbrechen
	if(posDoppelPunkt == NULL){
		return 0;
	}

	//Kanalnummer ausschneiden
	char subbuff[5];
	strncpy( subbuff, begin, (posDoppelPunkt - begin) );
	subbuff[4] = '\0';
	*channel = atoi(subbuff) - STARTADDRESS;

	//Faderwert ausschneiden
	//Trennzeichen suchen
	char* endChar = strstr(posDoppelPunkt, ";");

	/*
	* nicht gefunden, dann geht der Wert bis zum Stringende
	* sonst geht der Wert bis zum ';'
	*/
	if(endChar == NULL){
		strncpy( subbuff, (posDoppelPunkt + 1), (message + *numinmessage - posDoppelPunkt -1) );
		//gib Zeichen nach Doppelpunkt zurück. Nächster splitMessage-Aufruf gibt Fehler zurück
		begin = posDoppelPunkt +1;
	} else {
		strncpy( subbuff, (posDoppelPunkt + 1), (endChar - posDoppelPunkt -1) );
		//Gibt Position des Trennzeichens zurück
		begin = endChar;
	}
	subbuff[4] = '\0';

	*value = (uint8_t) atoi(subbuff);

	return begin;
}
