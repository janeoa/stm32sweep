/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@esden.net>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include "pwm.cpp"

PWM pwm(GPIOC, GPIO13);

#ifndef ARRAY_LEN
#define ARRAY_LEN(array) (sizeof((array))/sizeof((array)[0]))
#endif

#define LED1_PORT GPIOC
#define LED1_PIN GPIO13

volatile uint16_t frequency_sequence[] = {
    10,
    190
};

int frequency_sel = 0;

static void clock_setup(void)
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

static void gpio_setup(void)
{
    /* Enable GPIO clock for leds. */
    rcc_periph_clock_enable(RCC_GPIOC);
    
    /* Enable led as output */
    gpio_set_mode(LED1_PORT, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, LED1_PIN);
    gpio_set(LED1_PORT, LED1_PIN);
}

void setPWM(int period, int work){
//    int work = period*dutyCycle/100;
    frequency_sequence[0] = work;
    frequency_sequence[1] = period - work;
}

int main(void)
{
    
    setPWM(200, 15);
    clock_setup();
    gpio_setup();
    tim_setup();
    
    int sweep = 0;
    
    while (1) {
//        for(int i=0; i<20000; i++){
//            __asm__("nop");
//        }
//        setPWM(200, 0);
//        setPWM(200, sweep++);
//        sweep = (sweep>30)? 0: sweep;
        
    }
    
    return 0;
}
