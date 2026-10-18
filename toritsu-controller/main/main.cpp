#include "main.hpp"

#include <cmath>
#include <cstdio>
#include <freertos/FreeRTOS.h>

SemaphoreHandle_t lsm6dsv::lock;
lsm6dsv::Data lsm6dsv::data;
QueueHandle_t fix_output;
bool stopped = false;

extern "C" void app_main() {
  lsm6dsv::lock = xSemaphoreCreateMutex();

  xTaskCreate(blink_task, "blink task", 1024, nullptr, 1, nullptr);
  xTaskCreate(imu_task, "imu task", 8192, nullptr, 2, nullptr);
  xTaskCreate(motor_task, "motor task", 2048, nullptr, 4, nullptr);
  // xTaskCreate(server_task, "server task", 8192, nullptr, 3, nullptr);

  while(1) {
    static lsm6dsv::Data data;
    fix_output = xQueueCreate(1, sizeof(duty_cycle_t));

    if(xSemaphoreTake(lsm6dsv::lock, pdMS_TO_TICKS(15)) == pdTRUE) {
      data = lsm6dsv::data;
      xSemaphoreGive(lsm6dsv::lock);
    }

    // data.accelX -= ACCEL_X_OFFSET_FIX;
    // data.accelX *= ACCEL_X_SCALE_FIX;
    // data.accelY -= ACCEL_Y_OFFSET_FIX;
    // data.accelY *= ACCEL_Y_SCALE_FIX;
    // data.accelZ -= ACCEL_Z_OFFSET_FIX;
    // data.accelZ *= ACCEL_Z_SCALE_FIX;

    degree_t pitch = pitch_estimation(data);
    duty_cycle_t duty = pitch_fix_out(0, pitch);

    // printf("gyro  x: %f\n", data.gyroX);
    // printf("gyro  y: %f\n", data.gyroY);
    // printf("gyro  z: %f\n", data.gyroZ);
    // printf("accel x: %f\n", data.accelX);
    // printf("accel y: %f\n", data.accelY);
    // printf("accel z: %f\n", data.accelZ);

    // printf("pitch  : %f\n", pitch);
    // printf("duty   : %f\n", duty);

    xQueueOverwrite(fix_output, &duty);

    vTaskDelay(pdMS_TO_TICKS(MAIN_PERIOD_MS));
  }
}
