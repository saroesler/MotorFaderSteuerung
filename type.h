/*
 * type.h
 *
 *  Created on: 24.09.2016
 *      Author: sascha
 */

#ifndef TYPE_H_
#define TYPE_H_

#define ONLY_FADER 1
#define MOTOR_FADER 2

#define GERAET MOTOR_FADER //ONLY_FADER //


/*
 * input here the number of channels and the first address
 */

#if GERAET == MOTOR_FADER
#define CHANNEL			8
#define STARTADDRESS	0
#define TYPE MOTOR_FADER
#elif GERAET == ONLY_FADER
#define CHANNEL			14
#define STARTADDRESS	8
#define TYPE ONLY_FADER
#endif

#endif /* TYPE_H_ */
