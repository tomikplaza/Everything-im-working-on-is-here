#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/pulse_cnt.h" // The official ESP-IDF pulse counter library
#include "esp_log.h"

// Let's define the encoder pins for the Left Motor
#define LEFT_ENCODER_A_PIN GPIO_NUM_18
#define LEFT_ENCODER_B_PIN GPIO_NUM_19

// Global handle for our hardware unit
pcnt_unit_handle_t left_encoder_unit = NULL;

void init_left_encoder() {
    // 1. Configure the Main PCNT Unit
    pcnt_unit_config_t unit_config = {
        .low_limit = -30000, // Maximum negative count before rolling over
        .high_limit = 30000, // Maximum positive count before rolling over
    };
    // Initialize the unit
    pcnt_new_unit(&unit_config, &left_encoder_unit);

    // 2. Configure a Glitch Filter (Crucial for noisy electric motor wires!)
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000, // Ignore any electrical spikes shorter than 1000 nanoseconds
    };
    pcnt_unit_set_glitch_filter(left_encoder_unit, &filter_config);

    // 3. Configure Channel A (Monitors Phase A pin, watches Phase B for direction)
    pcnt_chan_handle_t chan_a = NULL;
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = LEFT_ENCODER_A_PIN,
        .level_gpio_num = LEFT_ENCODER_B_PIN,
    };
    pcnt_new_channel(left_encoder_unit, &chan_a_config, &chan_a);

    // 4. Configure Channel B (Monitors Phase B pin, watches Phase A for direction)
    pcnt_chan_handle_t chan_b = NULL;
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = LEFT_ENCODER_B_PIN,
        .level_gpio_num = LEFT_ENCODER_A_PIN,
    };
    pcnt_new_channel(left_encoder_unit, &chan_b_config, &chan_b);

    // 5. Define what actions to take on signal changes (X4 Decoding Mode)
    // When edge pin rises, increment if level pin is low, decrement if high
    pcnt_channel_set_edge_action(chan_a, PCNT_EDGE_ACTION_INCREASE, PCNT_EDGE_ACTION_DECREASE);
    pcnt_channel_set_level_action(chan_a, PCNT_LEVEL_ACTION_KEEP, PCNT_LEVEL_ACTION_INVERSE);
    
    // Do the exact same thing for Channel B to catch every single pulse edge
    pcnt_channel_set_edge_action(chan_b, PCNT_EDGE_ACTION_INCREASE, PCNT_EDGE_ACTION_DECREASE);
    pcnt_channel_set_level_action(chan_b, PCNT_LEVEL_ACTION_KEEP, PCNT_LEVEL_ACTION_INVERSE);

    // 6. Enable and Start the Counter!
    pcnt_unit_enable(left_encoder_unit);
    pcnt_unit_start(left_encoder_unit);
}