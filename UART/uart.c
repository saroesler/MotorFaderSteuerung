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
	char data = UDR;

	if((data == '\0' || (data >= 0x20 && data <= 0x7d)) && (numInMessage == 0)){
		inMessage[iInMessage] = data;
		iInMessage ++;

		if(data == '\0'){
			numInMessage = iInMessage;
			iInMessage = 0;
		}
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

	UCSRB |= (1<<RXEN) | (1<<RXCIE);  // UART einschalten
	//UCSRB |= (1<<RXEN);                        // UART RX einschalten & Interrupt on Receive
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // Asynchron 8N1

	/*
	 * Initialize uart state wire
	 */
	UART_STATE_DDR |=  (1 << UART_STATE_OUT);
	UART_STATE_DDR &= ~(1 << UART_STATE_IN);
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
	//check if there is not another slave sending
	if(UART_STATE_WIRE_IS_FREE){
		UART_STATE_WIRE_SET;		//block bus
		ENABLE_UART_TX;				//enable uart transmitter

		//ready for sending
		return 1;
	} else {
		//another slave is sending right now, cant block bus
		return 0;
	}
}

void freeBus(void){
	DISABLE_UART_TX;		//shutdown uart transmitter
	TX_TO_INPUT;			//set tx-pin to high impedance
	UART_STATE_WIRE_FREE;
}
