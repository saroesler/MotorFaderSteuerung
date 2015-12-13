/*
 * adc_mcp.c
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 */

#include "adc_mcp.h"

//Gemessener Wert
static volatile uint16_t meassure = 0;

//neuer Wert
static uint8_t new = 0;

//status der Übertragung
static volatile uint8_t state = 0;

volatile uint8_t adcData[CHANNEL][7];
static volatile uint8_t oldData;
static volatile uint8_t counter;



void initSPI(){
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK) | (1 << DD_SS);
	PORT_SPI |= (1 << DD_SS);
	/* Enable SPI, Master, set clock rate fck/8, enable interrupt */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0) | (1 << SPI2X);

	for(uint8_t i = 0; i < CHANNEL; i ++){
		adcData[i][OLD] = 0;
		adcData[i][ACT] = 0;
		adcData[i][CHANGED] = 0;
		adcData[i][NEWVALUEFLAG] = 0;
		adcData[i][ADCNEWVALUE] = 0;
		adcData[i][TEMPVALUE] = 0;
		adcData[i][COUNTER] = 0;
	}

	sei();
}

//Start meassure
void startMeassure(){
	//enable interrupt
	SPCR |= (1 << SPIE);

	//CS Wählen
	PORTB &= ~(1<<2);

	//erstes Byte zum ADC senden
	uint8_t mask = 0b00000110;

	SPDR = mask;

	//Entprellen vorbereiten
	counter = 10;
	oldData = adcData[0][ACT];
	state = 1;
}

//interrupt, wenn fertig
ISR(SPI_STC_vect){
	uint8_t pin = state / 3;
	if((state % 3) == 0){
		//letzte Rückgabe auswerten
		PORTB |= (1<<2);

		meassure |= SPDR;

		uint8_t lastPin = CHANNEL - 1;
		if(pin){
			lastPin = pin -1;
		}
		adcData[lastPin][ADCNEWVALUE] = meassure >> 4;
		adcData[lastPin][NEWVALUEFLAG] = 1;

		//nächsten Wert speichern

		//SS Wählen
		PORTB &= ~(1<<2);

		//erstes Byte zum ADC senden
		uint8_t mask = 0b00000110;

		if(pin & (1 << 2))
			mask |= 1;

		SPDR = mask;
	}
	else if((state % 3) == 1){
		SPDR = pin << 6;
	}
	else{
		meassure = (uint16_t)SPDR << 8;
		meassure = meassure & (0x0F00);
		SPDR = 0;
	}
	state ++;
	if(state == (CHANNEL * 3))
		state = 0;
}

uint8_t newMessage(){
	return new;
}

uint8_t getMeassure(){
	new = 0;
	return meassure;
}

/**
 * read measure of the ADC
 * this waits until message is receiving
 */
uint16_t readMeassure(uint8_t pin){
	PORTB &= ~(1<<2);
	SPCR &= ~(1 << SPIE);

	uint8_t mask = 0b00000110;

	if(pin & (1 << 2))
		mask |= 1;

	SPDR = mask;

	while(!(SPSR & (1<<SPIF)));
	SPDR = pin << 6;

	while(!(SPSR & (1<<SPIF)));
	meassure = (uint16_t)SPDR << 8;
	meassure = meassure & (0x0F00);
	SPDR = 0;

	while(!(SPSR & (1<<SPIF)));
	meassure |= SPDR;

	PORTB |= (1<<2);
	return meassure;
}
