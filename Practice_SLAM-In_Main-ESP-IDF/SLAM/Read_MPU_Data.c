#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include <math.h>

#define _USE_MATH_DEFINES
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

// 1. Initialize the ESP32-S3 I2C Master Bus
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
void mpu6050_read_raw(int16_t *accel, int16_t *gyro) {
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
}

void mpu_task(void *pvParameters) {
    int16_t raw_accel[3];
    int16_t raw_gyro[3];

    while (1) {
        mpu6050_read_raw(raw_accel, raw_gyro);

        // Convert raw values to physical units
        // Default Gyro Full Scale Range is +/- 250 deg/s -> Sensitivity factor is 131.0
        float gyro_z_dps = (float)raw_gyro[2] / 131.0f;
        float gyro_z_rad = gyro_z_dps * (M_PI / 180.0f); // Convert to Radians/sec for our filter!

        ESP_LOGI(TAG, "Gyro Z (Yaw Speed): %.4f rad/s", gyro_z_rad);

        vTaskDelay(pdMS_TO_TICKS(20)); // Run loop at 50Hz (dt = 0.02s)
    }
}

void app_main(void) {
    i2c_master_init();
    mpu6050_init();
    xTaskCreate(mpu_task, "mpu_task", 4096, NULL, 5, NULL);
}