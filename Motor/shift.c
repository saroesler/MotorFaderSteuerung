/*
 * shift.c
 *
 *  Created on: 26.11.2015
 *      Author: sascha
 */
#include "shift.h"

#if TYPE == ONLY_FADER
void init_shift(void){
	SHIFT_DDR |= (1<< SHIFT_DATA) | (1<< SHIFT_CLK) | (1<< SHIFT_PS);
}

void shift(uint32_t data){
	for(int8_t i = 31; i >= 0 ; i --){
		if(data & (1 << i))
			SHIFT_PORT |= (1 << SHIFT_DATA);
		else
			SHIFT_PORT &= ~(1 << SHIFT_DATA);
		SHIFT_PORT |= (1 << SHIFT_CLK);
		SHIFT_PORT &= ~(1 << SHIFT_CLK);
	}

	SHIFT_PORT |= (1 << SHIFT_PS);
	SHIFT_PORT |= (1 << SHIFT_CLK);
	SHIFT_PORT &= ~(1 << SHIFT_CLK);
	SHIFT_PORT &= ~(1 << SHIFT_PS);
}
#endif
