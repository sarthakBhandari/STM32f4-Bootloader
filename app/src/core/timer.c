#include "core/timer.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>

#define PRESCALAR (84)
#define ARR_VALUE (1000)

// freq_desired = sys_freq/(prescalar * arr)
void timer_setup(void){
    rcc_periph_clock_enable(RCC_TIM2);
    
    // high level timer configuration
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    
    // setup pwm mode
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
    
    // enable counter
    timer_enable_counter(TIM2);
    // enable output compare
    timer_enable_oc_output(TIM2, TIM_OC1);

    // setup freq and resolution
    timer_set_prescaler(TIM2, PRESCALAR - 1);
    timer_set_period(TIM2, ARR_VALUE - 1);
}

void timer_pwm_set_duty_cycle(float duty_cycle){
    // compare = (1 - duty_cycle/100)*arr
    const float raw_value = (float) (1 - duty_cycle/100.0f)*ARR_VALUE;
    timer_set_oc_value(TIM2, TIM_OC1, (uint32_t)raw_value);
}