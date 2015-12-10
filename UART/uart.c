/*
 * uart.c
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 */

#include "uartVar.h"
#include "uart.h"

volatile uint8_t interrupt = 1;

ISR(USART_RXC_vect)
{
	uartFlag &=~ (1<<NEWRECEIVED);

	char data = UDR;

	if(data == '\0' || (data >= 0x20 && data <= 0x7d)){
		inMessage[iInMessage] = data;
		iInMessage ++;

		if(data == '\0'){
			numInMessage = iInMessage;
			uartFlag |= (1 << NEWRECEIVED);
		}
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
	//Ende senden
	} else {
		iOutMessage = 0;
		uartFlag |= (1<<SENDET);
		outMessage[0] = '\0';
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
