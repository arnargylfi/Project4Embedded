#!/bin/bash
#  Short script to toggle a GPIO pin at the highest frequency
#  possible using Bash - by Derek Molloy

echo 17 > /sys/class/gpio/export
echo 27 > /sys/class/gpio/export
sleep 0.5
echo "out" > /sys/class/gpio/gpio17/direction
echo "in" > /sys/class/gpio/gpio27/direction
COUNTER=0
while [ $COUNTER -lt 100000 ]; do
    cat /sys/class/gpio/gpio27/value > /sys/class/gpio/gpio17/value
    let COUNTER=COUNTER+1
done

echo 17 > /sys/class/gpio/unexport
echo 27 > /sys/class/gpio/unexport