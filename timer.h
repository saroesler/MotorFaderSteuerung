#ifndef TIMER_H
#define TIMER_H

#include "main.h"
#include "type.h"
#include "fader.h"

#if TYPE == MOTOR_FADER
#define STARTTIMER TCCR1B |= (1<<CS11) | (1<<CS10)
#define STOPTIMER TCCR1B &= ~((1<<CS11) | (1<<CS10))

extern void timer_init(void);
extern void setCycles(uint8_t num, uint32_t mycycles);
extern void timerUpdate(void);

extern uint8_t timerActionHappend(faderstruct* fader);
#endif

#endif
