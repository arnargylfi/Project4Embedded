#ifndef PWM_H
#define PWM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

class Pwm {
    public:
        Pwm() {}

        void set(float duty_cycle) {

            // echo duty ns í 
            int duty_ns = (int)(duty_cycle * period);
            sprintf(carray, "%d", duty_ns);
            
            int duty_cycle_fd = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY);
            write(duty_cycle_fd, carray, strlen(carray));
            close(duty_cycle_fd);

            
        }

        void init(int period_ns = 100000) {
            printf("INIT");
            period = period_ns;
            sprintf(carray, "%d", period_ns);

            // echo 0 í Export
            int export_fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
            write(export_fd, "0", 1);
            close(export_fd);

            // echo into period
            int period_fd = open("/sys/class/pwm/pwmchip0/pwm0/period", O_WRONLY);
            write(period_fd, carray, strlen(carray));
            close(period_fd);

            // echo 0 duty
            set(0.5);

            // echo 1 í enable
            int enable_fd = open("/sys/class/pwm/pwmchip0/pwm0/enable", O_WRONLY);
            write(enable_fd, "1", 1);
            close(enable_fd);
        }

        void exit() {
            int enable_fd = open("/sys/class/pwm/pwmchip0/pwm0/enable", O_WRONLY);
            write(enable_fd, "0", 1);
            close(enable_fd);
            // echo 0 í unexport
            int unexport_fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
            write(unexport_fd, "0", 1);
            close(unexport_fd);
        }

    private:
        int period;
        char carray[20];
};

#endif