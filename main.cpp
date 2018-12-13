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

static void tim_setup(void)
{
    /* Enable TIM2 clock. */
    rcc_periph_clock_enable(RCC_TIM2);
    
    /* Enable TIM2 interrupt. */
    nvic_enable_irq(NVIC_TIM2_IRQ);
    
    /* Reset TIM2 peripheral to defaults. */
    rcc_periph_reset_pulse(RST_TIM2);
    
    /* Timer global mode:
     * - No divider
     * - Alignment edge
     * - Direction up
     * (These are actually default values after reset above, so this call
     * is strictly unnecessary, but demos the api for alternative settings)
     */
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
                   TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    
    /*
     * Please take note that the clock source for STM32 timers
     * might not be the raw APB1/APB2 clocks.  In various conditions they
     * are doubled.  See the Reference Manual for full details!
     * In our case, TIM2 on APB1 is running at double frequency, so this
     * sets the prescaler to have the timer run at 5kHz
     */
    timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 1) / 5000));
    
    /* Disable preload. */
    timer_disable_preload(TIM2);
    timer_continuous_mode(TIM2);
    
    /* count full range, as we'll update compare value continuously */
    timer_set_period(TIM2, 65535);
    
    /* Set the initual output compare value for OC1. */
    timer_set_oc_value(TIM2, TIM_OC1, frequency_sequence[frequency_sel++]);
    
    /* Counter enable. */
    timer_enable_counter(TIM2);
    
    /* Enable Channel 1 compare interrupt to recalculate compare values */
    timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

void tim2_isr(void)
{
    if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
        
        /* Clear compare interrupt flag. */
        timer_clear_flag(TIM2, TIM_SR_CC1IF);
        
        /*
         * Get current timer value to calculate next
         * compare register value.
         */
        uint16_t compare_time = timer_get_counter(TIM2);
        
        /* Calculate and set the next compare value. */
        uint16_t frequency = frequency_sequence[frequency_sel++];
        uint16_t new_time = compare_time + frequency;
        
        timer_set_oc_value(TIM2, TIM_OC1, new_time);
        if (frequency_sel == ARRAY_LEN(frequency_sequence)) {
            frequency_sel = 0;
        }
        
        /* Toggle LED to indicate compare event. */
        gpio_toggle(LED1_PORT, LED1_PIN);
    }
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