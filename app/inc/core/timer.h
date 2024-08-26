#ifndef INC_TIMER_H
#define INC_TIMER_H

#include "common-defines.h"

void timer_setup(void);

void timer_pwm_set_duty_cycle(float duty_cycle);

#endif //INC_TIMER_H