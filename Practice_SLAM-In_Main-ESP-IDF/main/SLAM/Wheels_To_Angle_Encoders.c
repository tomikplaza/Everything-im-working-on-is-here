//FIRST LOOK AT Ticks_To_Meters.c

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

#define _USE_MATH_DEFINES

float right_wheel_meters_driven = 100.0;
float left_wheel_meters_driven = -100.0;
float distance_between_two_wheels = 0.5;

void app_main() {
float theta = (right_wheel_meters_driven - left_wheel_meters_driven) / distance_between_two_wheels;

printf("%.4f\n", theta);
}

