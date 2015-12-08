/*
 * uart.c
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 */

#include "uartVar.h"
#include "uart.h"

ISR(USART_RXC_vect)
{
	uartFlag &=~ (1<<NEWRECEIVED);
	//numInMessage = 0;

	char data = UDR;

	inMessage[iInMessage] = data;
	iInMessage ++;

	if(data == '\0'){
		numInMessage = iInMessage;
		uartFlag |= (1 << NEWRECEIVED);
		//iInMessage = 0;
	}
}

ISR(USART_TXC_vect)
{
	//ein datenpacket senden
	if(iOutMessage < numOutMessage){
		uartFlag &=~ (1<<SENDET);
		//nächstes Byte senden
		UDR = outMessage[iOutMessage ++];
	// schluss senden
	} else if(iOutMessage == numOutMessage){
		//Ende senden
		iOutMessage ++;
		UDR = '\0';
		uartFlag |= (1<<SENDET);
	//Ende senden
	} else {
		uartFlag |= (1<<SENDET);
	}
}

void uart_init(void)
{
	UBRRH = 0x00;//UBRR_VAL >> 8;
	UBRRL = 95;//UBRR_VAL & 0xFF;

	UCSRB |= (1<<TXEN) | (1<<RXEN) | (1<<RXCIE)| (1<<TXCIE);  // UART TX einschalten
	//UCSRB |= (1<<RXEN);                        // UART RX einschalten & Interrupt on Receive
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // Asynchron 8N1
	sei();
}

//start senden
void sendMessage(void){
	if(numOutMessage > 0 && iOutMessage == 0){
		uartFlag &=~ (1<<SENDET);
		UDR = outMessage[iOutMessage];
		iOutMessage++;
	}
}
