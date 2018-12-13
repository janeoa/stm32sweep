#include "pwm.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

int frequency_sel = 0;

PWM::PWM(int32 gpioport, int16 gpios){
    _gpioport = gpioport;
    _gpios = gpios;
    // tim_setup
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
    timer_set_oc_value(TIM2, TIM_OC1, 200);
    
    /* Counter enable. */
    timer_enable_counter(TIM2);
    
    /* Enable Channel 1 compare interrupt to recalculate compare values */
    timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

void tim2_isr(void){
    if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
        
        /* Clear compare interrupt flag. */
        timer_clear_flag(TIM2, TIM_SR_CC1IF);
        
        /*
         * Get current timer value to calculate next
         * compare register value.
         */
        uint16_t compare_time = timer_get_counter(TIM2);
        
        /* Calculate and set the next compare value. */
        uint16_t frequency = 200;
        uint16_t new_time = compare_time + frequency;
        
        timer_set_oc_value(TIM2, TIM_OC1, new_time);
        if (frequency_sel == ARRAY_LEN(frequency_sequence)) {
            frequency_sel = 0;
        }
        
        /* Toggle LED to indicate compare event. */
        gpio_toggle(LED1_PORT, LED1_PIN);
    }
}

void PWM::analogWrite(int period, int dutycycle){
    
}