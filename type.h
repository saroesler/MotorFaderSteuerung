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

#define TYPE ONLY_FADER

/*
 * input here the number of channels and the first address
 */

#if TYPE == MOTOR_FADER
#define CHANNEL			2
#define STARTADDRESS	0
#elif TYPE == ONLY_FADER
#define CHANNEL			16
#define STARTADDRESS	8
#endif

#endif /* TYPE_H_ */
