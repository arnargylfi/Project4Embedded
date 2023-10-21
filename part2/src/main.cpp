#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <chrono>
#include "pwm.h"
#include "PI_controller.h"
#include <algorithm>  // for std::clamp
#include <fcntl.h>

// Definitions for get_speed()
int old_pos = 0;
unsigned long long current_time = 0;
unsigned long long last_time = 0;
int speed_in_rpm = 0;

// definitions for motor control
double duty_cycle = 0;
int ref_speed = 30;

// PI controller
float ti = 0.2095;
float kp = 0.001;
PI_controller controller(kp,ti);


unsigned long long millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int get_speed(int pos) {
  current_time = millis();
  if (current_time - last_time >= 100) {
    int delta_pos = pos - old_pos;
    speed_in_rpm = ((float)delta_pos / (current_time - last_time))*60000.0/1400.0 ; // Convert to RPM
    // Reset old position and last time
    old_pos = pos;
    last_time = current_time;
  }
  return speed_in_rpm;
}

int main() {
    Pwm motorIN1;
    motorIN1.init();
    while (true) {
        FILE *fp = fopen("/sys/kernel/custom_module/pos", "r");
        if (!fp) {
            perror("Failed to open sysfs entry");
            exit(EXIT_FAILURE);
        }
        int pos;
        fscanf(fp, "%d", &pos);
        fclose(fp);

        int actual_speed = get_speed(pos); // speed in rpm
        duty_cycle = controller.update(ref_speed, actual_speed);
        duty_cycle = std::clamp(duty_cycle, 0.0, 1.0);
        motorIN1.set(duty_cycle);

        // print the actual speed at the control rate. 
        printf("sped: (Ref: %d - Act: %d) [RPM], duty cycle: %f \n",ref_speed, actual_speed, duty_cycle);
        //printf("pos: %d \n",pos);
    }
}
