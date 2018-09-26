/*
 * adc_mcp.c
 *
 *  Created on: 19.11.2015
 *      Author: sascha
 */

#include "adc_mcp.h"

//Gemessener Wert
static volatile uint16_t meassure = 0;

//status der Übertragung
static volatile uint8_t state = 0;

//uint8_t adcData[CHANNEL][10];
volatile uint8_t adcValue[CHANNEL][2];
//uint8_t dummyTest = 0;



void initSPI(){
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK) | (1 << DD_SS1) | (1 << DD_SS2);
	PORT_SPI |= (1 << DD_SS1) | (1 << DD_SS2);
	/* Enable SPI, Master, set clock rate fck/8, enable interrupt */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0) | (1 << SPI2X);

	//dummyTest = 0;

	for(uint8_t i = 0; i < CHANNEL; i ++){
		adcValue[i][NEWVALUEFLAG] = 0;
		adcValue[i][ADCNEWVALUE] = 0;

		//adcData[i][OLD] = 0;
		//adcData[i][ACT] = 0;
		//adcData[i][CHANGED] = 0;
		//adcData[i][TEMPVALUE] = 0;
		//adcData[i][COUNTER] = 0;
		//adcData[i][ADCREAL] = 0;
		//adcData[i][MIN] = 0;
		//adcData[i][MAX] = 0;
	}

	sei();
}

//Start meassure
void startMeassure(){
	//enable interrupt
	SPCR |= (1 << SPIE);

	//CS Wählen
#if TYPE == ONLY_FADER
	PORTB |= (1<<DD_SS2);
#endif
	PORTB &= ~(1<<DD_SS1);

	//erstes Byte zum ADC senden
	uint8_t mask = 0b00000110;

	SPDR = mask;

	state = 1;
}

//interrupt, wenn fertig
ISR(SPI_STC_vect){
	uint8_t pin = state / 3;
	if((state % 3) == 0){
		//first package

		//deselect chip
		PORTB |= (1<<2);
#if TYPE == ONLY_FADER
		PORTB |= (1<<1);
#endif

		//save value at the first byte of measure
		//the second byte will be the same, as state 2
		meassure |= SPDR;

		/*
		 * calculate previous channel (lastpin), to save the data, for it
		 * if the actual pin is 0, it gets the last channel (CHANNEL - 1)
		 * otherwise it gets the pinnumber -1
		 */
		uint8_t lastPin = CHANNEL - 1;
		if(pin){
			lastPin = pin -1;
		}

		adcValue[lastPin][ADCNEWVALUE] = meassure >> 4;
		adcValue[lastPin][NEWVALUEFLAG] = 1;

		/*in Fader.c
		 * adcData[lastPin][ADCREAL] = meassure >> 4;
		//double tmp =  255.0 / fader[lastPin].maxvalue;
		adcData[lastPin][ADCNEWVALUE] = meassure >> 4;//((meassure >> 4) - fader[lastPin].minvalue) * tmp;
		adcData[lastPin][ADCREAL] = meassure >> 4;
		adcData[lastPin][NEWVALUEFLAG] = 1;

		//unterlauf
		if((adcData[lastPin][ADCREAL] < 20) && (adcData[lastPin][ADCNEWVALUE] > 230))
			adcData[lastPin][ADCNEWVALUE] = 0;
		//overflow
		if((adcData[lastPin][ADCREAL] > 230) && (adcData[lastPin][ADCNEWVALUE] < 20))
			adcData[lastPin][ADCNEWVALUE] = 255;*/

		//nächsten Wert speichern

		//select chip
#if TYPE == MOTOR_FADER
		PORTB &= ~(1<<2);
#elif TYPE == ONLY_FADER
		if(pin < 8){
			PORTB &= ~(1<<2);
		} else {
			PORTB &= ~(1<<1);
		}
#endif

		//erstes Byte zum ADC senden
		// the LSB is the third bit of the pin number
		uint8_t mask = 0b00000110;

		if(pin & (1 << 2))
			mask |= 1;

		SPDR = mask;
	}
	else if((state % 3) == 1){
		SPDR = pin << 6;
	}
	else{
		//store value at the second byte of measure
		meassure = (uint16_t)SPDR << 8;
		meassure = meassure & (0x0F00);
		SPDR = 0;
	}
	state ++;
	if(state == (CHANNEL * 3))
		state = 0;
}

/**
 * read measure of the ADC
 * this waits until message is receiving
 */
uint16_t readMeassure(uint8_t pin){
#if TYPE == MOTOR_FADER
		PORTB &= ~(1<<2);
#elif TYPE == ONLY_FADER
		if(pin < 8){
			PORTB &= ~(1<<2);
		} else {
			PORTB &= ~(1<<1);
		}
#endif
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

	PORTB |= (1<<DD_SS1);
#if TYPE == ONLY_FADER
	PORTB |= (1<<DD_SS2);
#endif
	return meassure;
}
