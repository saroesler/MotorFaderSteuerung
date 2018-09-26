/*
 * uartVar.h
 *
 *  Created on: 24.11.2015
 *      Author: sascha
 */

#ifndef UARTVAR_H_
#define UARTVAR_H_

#include "../main.h"

//Zähler um zusendene Nachricht zu durchwandern
uint8_t volatile iOutMessage = 0;
//Länge der zusendenen Nachricht
uint8_t volatile numOutMessage = 0;

//Zähler um zusendene Nachricht zu durchwandern
uint8_t volatile iInMessage = 0;
//Länge der zusendenen Nachricht
uint8_t volatile numInMessage = 0;

//Sendeerlaubnis
uint8_t volatile allowSending = 0;

char volatile inMessage[20];
char volatile outMessage[100];

#endif /* UARTVAR_H_ */
