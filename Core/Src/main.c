#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>

#define LED_PORT (GPIOA)
#define LED_PIN (GPIO5)

#define SYSTICK_FREQ (1000)
#define CPU_FREQ (84000000)

volatile uint64_t ticks = 0;
void SysTick_Handler(void){
    // this is 64 bit operation on 32 bit cpu
    // so this will more than 1 operation to complete
    // this means that another interrupt could happen in the middle of this operation
    // you can handle this be disabling other interrupts
    ticks++;
}

static uint64_t get_ticks(void){
    return ticks;
}

static void rcc_setup(void){
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);
}

static void gpio_setup(void){
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void systick_setup(void){
    systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
    systick_counter_enable();
    systick_interrupt_enable();
}

// static void delay_cycles(uint32_t cycles){
//     for (uint32_t i = 0; i < cycles; i++){
//         __asm__("nop");
//     }
// }

int main(void){
    rcc_setup();
    gpio_setup();
    systick_setup();

    volatile uint64_t start_time = get_ticks();

    while (1){
        if (get_ticks() - start_time >= 1000){
            gpio_toggle(LED_PORT, LED_PIN);
            start_time = get_ticks();
        }
    }

    return 0;
}