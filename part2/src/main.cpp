#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>


// Definitions for get_speed()
int old_pos = 0;
int current_time = 0;
int last_time = 0;
int speed_in_rpm = 0;

// definitions for motor control
double duty_cycle = 0;
// Digital_out motorIN2(0); // low     pin ?
// Analog_out motorIN1(1);  // PWM     pin ?

// PI controller
float ti = 0.2095;
float kp = 0.006;
PI_controller controller(kp,ti);

// definition for printing
unsigned long lastPrintTime = 0;
int control_rate = ?


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
    int pos = syscall(SYS_get_pos); // reading the current position counter value
    int actual_speed = get_speed(pos) // speed in rpm
    duty_cycle = controller.update(ref_speed, actual_speed);
    // motorIN1.set(pwm);

    // print the actual speed at the control rate. 
    if (millis() - lastPrintTime >= control_rate) {  
            Serial.print("speed: (");
            Serial.print("Ref: ");
            Serial.print(ref_speed);
            Serial.print(" - Act: ");
            Serial.print(actual_speed);
            Serial.print(") [RPM], ");
            Serial.print(" duty cycle: ");
            Serial.print(duty_cycle);
            lastPrintTime = millis();  
        }
}

