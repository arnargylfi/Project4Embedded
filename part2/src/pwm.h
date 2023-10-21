#ifndef PWM_H
#define PWM_H

class Pwm {
    public:
        Pwm();
        void init(int period_ns = 100000);
        void set(float duty_cycle);
        void exit();

    private:
        int period;
        char carray[20];
};

#endif