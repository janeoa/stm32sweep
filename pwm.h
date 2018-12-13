#ifndef PWM_h
#define PWM_h

class PWM{
    public:
        PWM(int32 gpioport, int16 gpios);
        void analogWrite(int period, int dutycycle);
    private:
        int32 _gpioport;
        int16 _gpios;
        int period, dutycycle, frequency_sel;
        void tim2_isr(void);
        static void tim_setup(void);
};

#endif