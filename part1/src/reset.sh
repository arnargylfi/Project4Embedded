#!/bin/bash
# Resets GPIO pins in case you ctrl+C
echo 17 > /sys/class/gpio/unexport
echo 27 > /sys/class/gpio/unexport