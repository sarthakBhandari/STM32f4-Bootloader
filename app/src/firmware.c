#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/scb.h>

#include "core/system.h"
#include "timer.h"
#include "core/uart.h"

#define LED_PORT (GPIOA)
#define LED_PIN (GPIO5)

#define UART_PORT (GPIOA)
#define TX_PIN (GPIO2)
#define RX_PIN (GPIO3)

#define BOOTLOADER_SIZE (0X8000U);

static void vector_setup(void){
    SCB_VTOR = BOOTLOADER_SIZE;
}

static void gpio_setup(void){
    gpio_mode_setup(LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED_PIN);
    gpio_set_af(LED_PORT, GPIO_AF1, LED_PIN);

    gpio_mode_setup(UART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, RX_PIN | TX_PIN);
    gpio_set_af(UART_PORT, GPIO_AF7, RX_PIN | TX_PIN);
}

int main(void){
    vector_setup();
    system_setup();
    gpio_setup();
    timer_setup();
    uart_setup();

    uint64_t start_time = system_get_ticks();
    float duty_cycle = 0.0f;
    
    while (1){
        if (system_get_ticks() - start_time >= 10){
            // gpio_toggle(LED_PORT, LED_PIN);
            duty_cycle += 1.0f;
            if (duty_cycle > 100.0f){
                duty_cycle = 0.0f;
            }
            
            timer_pwm_set_duty_cycle(duty_cycle);
            start_time = system_get_ticks();
        }

        if (uart_data_available()){
            uint8_t data = uart_read_byte();
            uart_write_byte(data + 1);
        }
    }

    return 0;
}