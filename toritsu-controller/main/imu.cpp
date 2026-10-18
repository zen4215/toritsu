#include "main.hpp"

#include <cstring>
#include <driver/i2c_master.h>

namespace lsm6dsv {

enum class OperatingMode : uint8_t {
  HIGH_PEFORMANCE = 0b000,
  HIGH_ACCURACY_ODR = 0b001,
  RESERVED = 0b010,
  ODR_TRIGGERED = 0b011,
  LOW_POWER_1 = 0b100,
  LOW_POWER_2 = 0b101,
  LOW_POWER_3 = 0b110,
  NORMAL = 0b111,
};

constexpr uint8_t DATA_BEGIN_ADDR = 0x22;
constexpr uint8_t ACCEL_CONF_ADDR = 0x10;
constexpr uint8_t GYRO_CONF_ADDR = 0x11;

esp_err_t write_reg(i2c_master_dev_handle_t dev, uint8_t addr, uint8_t data);
uint8_t read_reg(i2c_master_dev_handle_t dev, uint8_t addr);
constexpr uint8_t ctrl_reg_data(OperatingMode mode, OutputDataRate rate);

} // namespace lsm6dsv

void imu_task(void*) {
  i2c_master_bus_handle_t i2c_bus;
  i2c_master_bus_config_t i2c_bus_config = {
      .i2c_port = -1,
      .sda_io_num = SDA_PIN,
      .scl_io_num = SCL_PIN,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .intr_priority = 0,
      .trans_queue_depth = 16,
      .flags = {},
  };

  i2c_new_master_bus(&i2c_bus_config, &i2c_bus);

  i2c_master_dev_handle_t lsm6dsv;
  i2c_device_config_t lsm6dsv_config = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = lsm6dsv::I2C_ADDR,
      .scl_speed_hz = lsm6dsv::I2C_FREQ,
      .scl_wait_us = 0,
      .flags = {},
  };

  i2c_master_bus_add_device(i2c_bus, &lsm6dsv_config, &lsm6dsv);

  ESP_ERROR_CHECK(lsm6dsv::write_reg(
      lsm6dsv, lsm6dsv::GYRO_CONF_ADDR,
      lsm6dsv::ctrl_reg_data(lsm6dsv::OperatingMode::HIGH_PEFORMANCE,
                             lsm6dsv::GYRO_RATE)));
  vTaskDelay(pdMS_TO_TICKS(100)); // gyro startup time

  ESP_ERROR_CHECK(lsm6dsv::write_reg(
      lsm6dsv, lsm6dsv::ACCEL_CONF_ADDR,
      lsm6dsv::ctrl_reg_data(lsm6dsv::OperatingMode::HIGH_PEFORMANCE,
                             lsm6dsv::ACCEL_RATE)));

  // lsm6dsv::write_reg(lsm6dsv, 0x19, 0b01000100); // self test gyro
  // lsm6dsv::write_reg(lsm6dsv, 0x5e, 0b01101011);
  // lsm6dsv::write_reg(lsm6dsv, 0x04, 0b00000010);

  while(1) {
    TickType_t begin_time = xTaskGetTickCount();

    uint8_t datas[12];

    i2c_master_transmit_receive(lsm6dsv, &lsm6dsv::DATA_BEGIN_ADDR,
                                sizeof(lsm6dsv::DATA_BEGIN_ADDR), datas,
                                sizeof(datas), 10);

    constexpr auto pick = [](uint8_t* ptr) -> int16_t {
      return (ptr[1] << 8) | ptr[0];
    };

    xSemaphoreTake(lsm6dsv::lock, pdMS_TO_TICKS(10));

    lsm6dsv::data.gyroX = pick(datas + 0) * lsm6dsv::GYRO_SENSITIVITY_DPS;
    lsm6dsv::data.gyroY = pick(datas + 2) * lsm6dsv::GYRO_SENSITIVITY_DPS;
    lsm6dsv::data.gyroZ = pick(datas + 4) * lsm6dsv::GYRO_SENSITIVITY_DPS;
    lsm6dsv::data.accelX = pick(datas + 6) * lsm6dsv::ACCEL_SENSITIVITY_G;
    lsm6dsv::data.accelY = pick(datas + 8) * lsm6dsv::ACCEL_SENSITIVITY_G;
    lsm6dsv::data.accelZ = pick(datas + 10) * lsm6dsv::ACCEL_SENSITIVITY_G;

    xSemaphoreGive(lsm6dsv::lock);

    vTaskDelayUntil(&begin_time, imu_sampling_period);
  }
}

esp_err_t lsm6dsv::write_reg(i2c_master_dev_handle_t dev, uint8_t addr,
                             uint8_t data) {
  return i2c_master_transmit(dev, (uint8_t[]){addr, data}, 2, -1);
}

uint8_t lsm6dsv::read_reg(i2c_master_dev_handle_t dev, uint8_t addr) {
  uint8_t ret;
  ESP_ERROR_CHECK(i2c_master_transmit_receive(dev, &addr, 1, &ret, 1, 10));
  // i2c_master_transmit(dev, &addr, 1, 10);
  // i2c_master_receive(dev, &ret, 1, 10);
  return ret;
}

constexpr uint8_t lsm6dsv::ctrl_reg_data(lsm6dsv::OperatingMode mode,
                                         lsm6dsv::OutputDataRate rate) {
  return (static_cast<uint8_t>(mode) << 4) | static_cast<uint8_t>(rate);
}
