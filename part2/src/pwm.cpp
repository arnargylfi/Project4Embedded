#include "pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

Pwm::Pwm() {}

void Pwm::set(float duty_cycle) {

    // echo 0 í enable
    int enable_fd = open("enable", O_WRONLY);
    write(enable_fd, "0", 1);

    // echo duty ns í 
    int duty_ns = (int)(duty_cycle * period);
    sprintf(carray, "%d", duty_ns);
    
    int duty_cycle_fd = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY);
    write(duty_cycle_fd, carray, strlen(carray));
    close(duty_cycle_fd);

    // echo 1 í enable
    write(enable_fd, "1", 1);
    close(enable_fd);
}

void Pwm::init(int period_ns) {
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
    Pwm::set(0.5);
}

void Pwm::exit() {
    // echo 0 í unexport
    int unexport_fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
    write(unexport_fd, "0", 1);
    close(unexport_fd);
}