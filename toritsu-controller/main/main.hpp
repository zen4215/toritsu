#pragma once

#include "types.hpp"

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// constants define
constexpr size_t PITCH_HISTORY_LEN = 8;
constexpr size_t GYRO_HISTORY_LEN = 32;
constexpr size_t ACCEL_WEIGHT_PITCH_HISTORY_LEN = 16;
constexpr float ACCEL_PITCH_TRUST_SIZE_G = 0.2f;
constexpr float FIX_PITCH_DUTY_MAX = 0.32f;
constexpr degree_t FIX_PITCH_DAEDZONE_SIZE = 0.8f;
constexpr float DUTY_KP = 0.0030f;
constexpr float DUTY_KI = 0.0050f;
constexpr float INTEGRAL_MAX = 5.0f;

constexpr uint32_t MAIN_PERIOD_MS = 50;
constexpr uint32_t MOTOR_PERIOD_MS = 50;

constexpr uint32_t PWM_FREQ_HZ = 20 * 1000; // 20kHz

static const char* WIFI_AP_SSID = "trt-ctrl";

constexpr gpio_num_t BUILTIN_LED_PIN = GPIO_NUM_21;

constexpr gpio_num_t SDA_PIN = GPIO_NUM_5;
constexpr gpio_num_t SCL_PIN = GPIO_NUM_6;

// constexpr gpio_num_t DIR_A_PIN = GPIO_NUM_1;
// constexpr gpio_num_t PWM_A_PIN = GPIO_NUM_2;
// constexpr gpio_num_t DIR_B_PIN = GPIO_NUM_4;
// constexpr gpio_num_t PWM_B_PIN = GPIO_NUM_43;

constexpr gpio_num_t PWM_A_PIN = GPIO_NUM_4;
constexpr gpio_num_t PWM_B_PIN = GPIO_NUM_43;

// constexpr float ACCEL_X_OFFSET_FIX = 0.0f;
// constexpr float ACCEL_X_SCALE_FIX = 1.0f;
// constexpr float ACCEL_Y_OFFSET_FIX = 0.0f;
// constexpr float ACCEL_Y_SCALE_FIX = 1.0f;
// constexpr float ACCEL_Z_OFFSET_FIX = 0.0f;
// constexpr float ACCEL_Z_SCALE_FIX = 1.0f;

namespace lsm6dsv {

constexpr uint8_t I2C_ADDR = 0x6a;
// constexpr uint8_t I2C_ADDR = 0x6b;
constexpr uint32_t I2C_FREQ = 100 * 1000;

constexpr OutputDataRate ACCEL_RATE = OutputDataRate::KHZ_1_92;
constexpr OutputDataRate GYRO_RATE = OutputDataRate::KHZ_1_92;

constexpr AccelFullScale ACCEL_FS = AccelFullScale::G_2;
constexpr grav_accel_t ACCEL_SENSITIVITY_G = 0.061f * 0.001f; // in 2g
// constexpr grav_accel_t ACCEL_SENSITIVITY_G = 0.122f * 0.001f; // in 4g
// constexpr grav_accel_t ACCEL_SENSITIVITY_G = 0.244f * 0.001f; // in 8g
// constexpr grav_accel_t ACCEL_SENSITIVITY_G = 0.488f * 0.001f; // in 16g

constexpr deg_per_sec_t GYRO_SENSITIVITY_DPS = 4.375f * 0.001f; // in 125g
// constexpr deg_per_sec_t GYRO_SENSITIVITY_DPS = 8.75f * 0.001f;  // in 250g
// constexpr deg_per_sec_t GYRO_SENSITIVITY_DPS = 17.50f * 0.001f; // in 500g
// constexpr deg_per_sec_t GYRO_SENSITIVITY_DPS = 35.0f * 0.001f;  // in 1000g
// constexpr deg_per_sec_t GYRO_SENSITIVITY_DPS = 70.0f * 0.001f;  // in 2000g
// constexpr deg_per_sec_t GYRO_SENSITIVITY_DPS = 140.0f * 0.001f; // in 4000g

} // namespace lsm6dsv

constexpr TickType_t imu_sampling_period = pdMS_TO_TICKS(10);

// tasks declare
void blink_task(void*);
void imu_task(void*);
void motor_task(void*);
void server_task(void*);

degree_t pitch_estimation(const lsm6dsv::Data& data);
duty_cycle_t pitch_fix_out(degree_t target_pitch, degree_t now_pitch);

// global values
namespace lsm6dsv {

extern SemaphoreHandle_t lock;
extern Data data;

} // namespace lsm6dsv

extern QueueHandle_t fix_output;

extern bool stopped;
