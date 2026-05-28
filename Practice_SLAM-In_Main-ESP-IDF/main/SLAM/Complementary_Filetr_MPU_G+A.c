#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <math.h>

#define _USE_MATH_DEFINES

static const char *TAG = "MPU6050";

int64_t old_time;

float GyroX;
float GyroY;
float GyroZ;

float AccX;
float AccY;
float AccZ;

float Roll_fused;
float Pitch_fused;

float theta_encoder = 400;
float theta_fused;

void Complementary(void){
    old_time = esp_timer_get_time();
    while (1){
        float cur_time = esp_timer_get_time();

        float dt = (cur_time - old_time) / 1000000.0f;
        old_time = esp_timer_get_time();

        float Aroll = atan2f(AccY, AccZ);
        float Apitch = atan2f(-AccX, sqrtf(AccY * AccY + AccZ * AccZ));

        Roll_fused = 0.98f * (Roll_fused + GyroX * dt) + 0.02f * Aroll;
        Pitch_fused = 0.98f * (Pitch_fused + GyroY * dt) + 0.02f * Apitch;

        theta_fused = 0.98 * (theta_fused + GyroZ * dt) + 0.02f * theta_encoder;
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

void app_main(void){
    Complementary();
}