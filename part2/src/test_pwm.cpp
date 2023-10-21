#include "pwm.h"

Pwm pwm;

int main() {
    pwm.exit();
    pwm.init(100000);

    for (int i = 0; i < 20000; i++) {
        pwm.set(0.8);
    }

    pwm.exit();

    return 0;
}