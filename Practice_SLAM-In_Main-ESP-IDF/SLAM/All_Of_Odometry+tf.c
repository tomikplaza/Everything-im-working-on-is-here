#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "freertos/task.h"

#include "driver/pulse_cnt.h" // The official ESP-IDF pulse counter library

#include "esp_log.h"

#include <math.h>

#include "esp_timer.h"

#include "sdkconfig.h"

#include "driver/i2c_master.h"

#include <nav_msgs/msg/odometry.h>


#include <rcl/rcl.h>

#include <rclc/rclc.h>

#include <tf2_msgs/msg/tf_message.h>

#include <geometry_msgs/msg/transform_stamped.h>

#include <string.h>




#define _USE_MATH_DEFINES



volatile float finished_X;

volatile float finished_Y;



int16_t gyro[3];

int16_t accel[3];



float left_wheel_tics;

float right_wheel_tics;

float distance_between_wheels = 0.6;



float old_X;

float old_Y;



int64_t old_time;



float Roll_fused;

float Pitch_fused;

float theta_fused;



// Let's define the encoder pins for the Left Motor

#define LEFT_ENCODER_A_PIN GPIO_NUM_18

#define LEFT_ENCODER_B_PIN GPIO_NUM_19

#define RIGHT_ENCODER_A_PIN GPIO_NUM_17

#define RIGHT_ENCODER_B_PIN GPIO_NUM_16



static const char *TAG = "MPU6050";



#define I2C_MASTER_SCL_IO           GPIO_NUM_9    // Change to your physical SCL Pin

#define I2C_MASTER_SDA_IO           GPIO_NUM_8    // Change to your physical SDA Pin

#define I2C_MASTER_NUM              I2C_NUM_0

#define MPU6050_SENSOR_ADDR         0x68          // Default I2C address for MPU6050



// MPU6050 Register Maps

#define MPU6050_PWR_MGMT_1          0x6B

#define MPU6050_ACCEL_XOUT_H        0x3B

#define MPU6050_GYRO_XOUT_H         0x43





i2c_master_dev_handle_t dev_handle;



// Global handle for our hardware unit

pcnt_unit_handle_t left_encoder_unit = NULL;

pcnt_unit_handle_t right_encoder_unit = NULL;





void i2c_master_init(void) {

    i2c_master_bus_config_t bus_config = {

        .clk_source = I2C_CLK_SRC_DEFAULT,

        .i2c_port = I2C_MASTER_NUM,

        .scl_io_num = I2C_MASTER_SCL_IO,

        .sda_io_num = I2C_MASTER_SDA_IO,

        .glitch_ignore_cnt = 7,

        .flags.enable_internal_pullup = true,

    };

    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));



    i2c_device_config_t dev_config = {

        .dev_addr_length = I2C_ADDR_BIT_LEN_7,

        .device_address = MPU6050_SENSOR_ADDR,

        .scl_speed_hz = 400000, // 400kHz Fast Mode

    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

}



// 2. Wake up the MPU6050 (It starts in sleep mode by default!)

void mpu6050_init(void) {

    uint8_t wake_cmd[] = {MPU6050_PWR_MGMT_1, 0x00};

    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, wake_cmd, sizeof(wake_cmd), -1));

    ESP_LOGI(TAG, "MPU6050 initialized and awakened successfully.");

}



// 3. Read the Raw Sensor Burst Data

void mpu6050_read_raw(void *pvParameters) {

  while (1){

    uint8_t reg_addr_accel = MPU6050_ACCEL_XOUT_H;

    uint8_t data_accel[6];

    uint8_t reg_addr_gyro = MPU6050_GYRO_XOUT_H;

    uint8_t data_gyro[6];



    // Read 6 bytes of Accelerometer Data (X, Y, Z high/low bytes)

    i2c_master_transmit_receive(dev_handle, &reg_addr_accel, 1, data_accel, 6, -1);

    // Read 6 bytes of Gyroscope Data (X, Y, Z high/low bytes)

    i2c_master_transmit_receive(dev_handle, &reg_addr_gyro, 1, data_gyro, 6, -1);



    // Combine high and low bytes into signed 16-bit integers

    accel[0] = (data_accel[0] << 8) | data_accel[1]; // Accel X

    accel[1] = (data_accel[2] << 8) | data_accel[3]; // Accel Y

    accel[2] = (data_accel[4] << 8) | data_accel[5]; // Accel Z



    gyro[0] = (data_gyro[0] << 8) | data_gyro[1];   // Gyro X

    gyro[1] = (data_gyro[2] << 8) | data_gyro[3];   // Gyro Y

    gyro[2] = (data_gyro[4] << 8) | data_gyro[5];   // Gyro Z (Our target for Yaw!)

   

    vTaskDelay(pdMS_TO_TICKS(30));

  }

}





void Complementary(void *pvParameters){

  // ==================================================

    // 1. Setup Right Encoder Hardware Unit

    // ==================================================

    pcnt_unit_config_t unit_config = {

        .low_limit = -30000,

        .high_limit = 30000,

    };

    pcnt_new_unit(&unit_config, &right_encoder_unit);



    pcnt_glitch_filter_config_t filter_config = {

        .max_glitch_ns = 1000,

    };

    pcnt_unit_set_glitch_filter(right_encoder_unit, &filter_config);



    pcnt_chan_handle_t chan_a = NULL;

    pcnt_chan_config_t chan_a_config = {

        .edge_gpio_num = RIGHT_ENCODER_A_PIN,

        .level_gpio_num = RIGHT_ENCODER_B_PIN,

    };

    pcnt_new_channel(right_encoder_unit, &chan_a_config, &chan_a);



    pcnt_chan_handle_t chan_b = NULL;

    pcnt_chan_config_t chan_b_config = {

        .edge_gpio_num = RIGHT_ENCODER_B_PIN,

        .level_gpio_num = RIGHT_ENCODER_A_PIN,

    };

    pcnt_new_channel(right_encoder_unit, &chan_b_config, &chan_b);



    pcnt_channel_set_edge_action(chan_a, PCNT_EDGE_ACTION_INCREASE, PCNT_EDGE_ACTION_DECREASE);

    pcnt_channel_set_level_action(chan_a, PCNT_LEVEL_ACTION_KEEP, PCNT_LEVEL_ACTION_INVERSE);

   

    pcnt_channel_set_edge_action(chan_b, PCNT_EDGE_ACTION_INCREASE, PCNT_EDGE_ACTION_DECREASE);

    pcnt_channel_set_level_action(chan_b, PCNT_LEVEL_ACTION_KEEP, PCNT_LEVEL_ACTION_INVERSE);



    pcnt_unit_enable(right_encoder_unit);

    pcnt_unit_start(right_encoder_unit);



    // ==================================================

    // 2. Setup Left Encoder Hardware Unit (FIX 2: Reusing existing handles/configs)

    // ==================================================

    pcnt_new_unit(&unit_config, &left_encoder_unit);

    pcnt_unit_set_glitch_filter(left_encoder_unit, &filter_config);



    chan_a_config.edge_gpio_num = LEFT_ENCODER_A_PIN;

    chan_a_config.level_gpio_num = LEFT_ENCODER_B_PIN;

    pcnt_new_channel(left_encoder_unit, &chan_a_config, &chan_a);



    chan_b_config.edge_gpio_num = LEFT_ENCODER_B_PIN;

    chan_b_config.level_gpio_num = LEFT_ENCODER_A_PIN;

    pcnt_new_channel(left_encoder_unit, &chan_b_config, &chan_b);



    pcnt_channel_set_edge_action(chan_a, PCNT_EDGE_ACTION_INCREASE, PCNT_EDGE_ACTION_DECREASE);

    pcnt_channel_set_level_action(chan_a, PCNT_LEVEL_ACTION_KEEP, PCNT_LEVEL_ACTION_INVERSE);

   

    pcnt_channel_set_edge_action(chan_b, PCNT_EDGE_ACTION_INCREASE, PCNT_EDGE_ACTION_DECREASE);

    pcnt_channel_set_level_action(chan_b, PCNT_LEVEL_ACTION_KEEP, PCNT_LEVEL_ACTION_INVERSE);



    pcnt_unit_enable(left_encoder_unit);

    pcnt_unit_start(left_encoder_unit);



    old_time = esp_timer_get_time();

  while (1){

   int pulse_counter_snapshot = 0;

   

   pcnt_unit_get_count(left_encoder_unit, &pulse_counter_snapshot);

   left_wheel_tics = (float)pulse_counter_snapshot;



   pcnt_unit_get_count(right_encoder_unit, &pulse_counter_snapshot);

   right_wheel_tics = (float)pulse_counter_snapshot;



   float cur_time = esp_timer_get_time();

     

   float dt = (cur_time - old_time) / 1000000.0f;

   old_time = cur_time;



   if (dt <= 0.0f) dt = 0.03f;



    float Circum = M_PI * 0.065;



    float Meters_Per_Tick = Circum / 1440;

    float Meters_driven = ((right_wheel_tics * Meters_Per_Tick) + (left_wheel_tics * Meters_Per_Tick)) / 2;

    float left_dist  = left_wheel_tics * Meters_Per_Tick;

    float right_dist = right_wheel_tics * Meters_Per_Tick;



float theta_encoder = (right_dist - left_dist) / distance_between_wheels;





// Caclculate the speed on robot

float V_linear = Meters_driven / dt;



float GyroX = ((float)gyro[0] / 131.0f) * (M_PI / 180.0f);

float GyroY = ((float)gyro[1] / 131.0f) * (M_PI / 180.0f);

    float GyroZ = ((float)gyro[2] / 131.0f) * (M_PI / 180.0f);



  float AccX = (float)accel[0];

  float AccY = (float)accel[1];

  float AccZ = (float)accel[2];





  float Aroll = atan2f(AccY, AccZ);

  float Apitch = atan2f(-AccX, sqrtf(AccY * AccY + AccZ * AccZ));

 

  Roll_fused = 0.98f * (Roll_fused + GyroX * dt) + 0.02f * Aroll;

  Pitch_fused = 0.98f * (Pitch_fused + GyroY * dt) + 0.02f * Apitch;



    float theta_gyro = theta_fused + GyroZ * dt;



    theta_fused = 0.98f * theta_gyro + 0.02f * theta_encoder;



  float X_new = Meters_driven * cosf(theta_fused);

  float Y_new = Meters_driven * sinf(theta_fused);



  finished_X += X_new;

  finished_Y += Y_new;



  // CLEAR hardware counters back to zero for the next cycle window

  pcnt_unit_clear_count(left_encoder_unit);

  pcnt_unit_clear_count(right_encoder_unit);



  vTaskDelay(pdMS_TO_TICKS(30));

  }

}

void micro_ros(void *pvParameters){

// --- STEP A: THE TRANSPORT ---

    rmw_microros_set_udp_transports("192.168.1.15", 8888);


    // --- STEP B: THE ALLOCATOR ---

    rcl_allocator_t allocator = rcl_get_default_allocator();


    // --- STEP C: THE SUPPORT ---

    rclc_support_t support;

    rclc_support_init(&support, 0, NULL, &allocator);



    // --- STEP D: THE NODE ---

    // Changed the node name so it doesn't conflict with ROS 2 standard names

    rcl_node_t node;

    rclc_node_init_default(&node, "esp32_base_controller", "", &support);


    // --- STEP E: THE PUBLISHERS ---
    rcl_publisher_t odom_publisher;

    const rosidl_message_type_support_t * odom_type_support =

        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry);


    rclc_publisher_init_default(&odom_publisher, &node, odom_type_support, "odom");


    rcl_publisher_t tf_publisher;

    const rosidl_message_type_support_t * tf_type_support =


        ROSIDL_GET_MSG_TYPE_SUPPORT(tf2_msgs, msg, TFMessage);


    rclc_publisher_init_default(&tf_publisher, &node, tf_type_support, "tf");


    // --- STEP F: THE MESSAGES ---
    nav_msgs__msg__Odometry odom_msg;

    tf2_msgs__msg__TFMessage tf_msg;

    geometry_msgs__msg__TransformStamped tf_stamped;


    // C requires us to manually link the array pointer and set the size for TF

    tf_msg.transforms.data = &tf_stamped;

    tf_msg.transforms.size = 1;

    tf_msg.transforms.capacity = 1;





    while(1) {



        odom_msg.header.frame_id.data = (char*)"odom";

        odom_msg.header.frame_id.size = strlen("odom");

        odom_msg.header.frame_id.capacity = strlen("odom") + 1;


        // YOU MUST ADD THESE THREE LINES:

        odom_msg.child_frame_id.data = (char*)"base_link";

        odom_msg.child_frame_id.size = strlen("base_link");

        odom_msg.child_frame_id.capacity = strlen("base_link") + 1;



        // TYPE MESSAGE HERE

        odom_msg.pose.pose.position.x = finished_X;


        
        odom_msg.pose.pose.position.y = finished_Y;

        odom_msg.pose.pose.position.z = 0.0;

        // This is the moment the "text" is published!

        // It sends the msg structure through the UDP transport to the Agent.



        odom_msg.pose.pose.orientation.z = sinf(theta_fused/2);

        odom_msg.pose.pose.orientation.w = cosf(theta_fused/2);


        tf_stamped.header.frame_id.data = (char*)"odom";

        tf_stamped.header.frame_id.size = strlen("odom");

        tf_stamped.child_frame_id.data = (char*)"base_link";

        tf_stamped.child_frame_id.size = strlen("base_link");


        tf_stamped.transform.translation.x = finished_X;

        tf_stamped.transform.translation.y = finished_Y;

        tf_stamped.transform.translation.z = 0.0;


        // The rotation math is identical to the odometry quaternion

        tf_stamped.transform.rotation.x = 0.0;

        tf_stamped.transform.rotation.y = 0.0;

        tf_stamped.transform.rotation.z = sinf(theta_fused/2.0f);

        tf_stamped.transform.rotation.w = cosf(theta_fused/2.0f);


        // ==========================================

        // 3. PUBLISH BOTH

        // ==========================================

        rcl_publish(&odom_publisher, &odom_msg, NULL);

        rcl_publish(&tf_publisher, &tf_msg, NULL);


        vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz update rate

    }

}









void app_main(void) {

    i2c_master_init();

    mpu6050_init();

    xTaskCreate(mpu6050_read_raw, "Read_mpu", 4096, NULL, 5, NULL);

    xTaskCreate(Complementary, "Complementary", 4096, NULL, 5, NULL);

    xTaskCreate(micro_ros, "micro_ros_odom_pub", 8192, NULL, 5, NULL);

}

