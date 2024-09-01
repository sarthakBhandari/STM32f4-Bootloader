#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>

#include "core/system.h"

static volatile uint64_t ticks = 0;
void sys_tick_handler(void){
    // this is 64 bit operation on 32 bit cpu
    // so this will more than 1 operation to complete
    // this means that another interrupt could happen in the middle of this operation
    // you can handle this be disabling other interrupts
    ticks++;
}

static void rcc_setup(void){
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);
}

static void systick_setup(void){
    systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
    systick_counter_enable();
    systick_interrupt_enable();
}

uint64_t system_get_ticks(void){
    return ticks;
}

void system_setup(void){
    rcc_setup();
    systick_setup();
}