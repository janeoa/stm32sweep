#include "pwm.h"

PWM::PWM(int32 gpioport, int16 gpios){
    _gpioport = gpioport;
    _gpios = gpios;
}

void PWM::analogWrite(int period, int dutycycle){
    
}