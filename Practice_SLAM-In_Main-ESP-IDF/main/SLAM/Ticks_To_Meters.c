#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

#define _USE_MATH_DEFINES

float wheel_size = 0.065;
float ticks_per_rotation = 1440;

void app_main() {
float Circum = M_PI * wheel_size;

float Meters_Per_Tick = Circum / ticks_per_rotation;
printf("%f\n", Meters_Per_Tick);
}
