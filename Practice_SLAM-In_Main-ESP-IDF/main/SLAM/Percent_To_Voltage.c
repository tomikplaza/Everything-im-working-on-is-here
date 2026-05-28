#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

// Fix: Matches the usage below
#define LED_WORKING GPIO_NUM_12

#define LEDC_TIMER       LEDC_TIMER_0
#define LEDC_MODE        LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL     LEDC_CHANNEL_0
#define LEDC_RESOLUTION  LEDC_TIMER_10_BIT
#define LEDC_FREQUENCY   (20000)

float voltage;
int percent;

void PWM_init() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_RESOLUTION,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED_WORKING, // Spelling fixed!
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

// Note: Removed extern "C" because your file is named .c
float Percent_to_voltage(int percent){
float percent_num = percent / 100.0f ;
return 3.3 * percent_num;

}


void app_main(void) {
    PWM_init();

    percent = 50;

    voltage = Percent_to_voltage(percent);
    float PWM = (voltage / 3.3) * 1023;

    printf("Put PWM: %.2f PWM\n", PWM);
    printf("Voltage: %.2fV\n", voltage);
}
    