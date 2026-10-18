#include "main.hpp"

#include <driver/ledc.h>

void motor_set_a(duty_cycle_t duty);
void motor_set_b(duty_cycle_t duty);
void motor_set(duty_cycle_t duty);

void motor_task(void*) {
  // dir config
  // gpio_config_t dir_gpio_config = {
  //     .pin_bit_mask = (1 << DIR_A_PIN) | (1 << DIR_B_PIN),
  //     .mode = GPIO_MODE_OUTPUT,
  //     .pull_up_en = GPIO_PULLUP_DISABLE,
  //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
  //     .intr_type = GPIO_INTR_DISABLE,
  // };

  // ESP_ERROR_CHECK(gpio_config(&dir_gpio_config));

  // pwm config
  ledc_timer_config_t pwm_timer_config = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_8_BIT,
      .timer_num = LEDC_TIMER_0,
      .freq_hz = PWM_FREQ_HZ,
      .clk_cfg = LEDC_AUTO_CLK,
      .deconfigure = false,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer_config));

  ledc_channel_config_t pwm_a_channel_config = {
      .gpio_num = PWM_A_PIN,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_0,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0,
      .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
      .flags = {},
      .deconfigure = false,
  };

  ledc_channel_config_t pwm_b_channel_config = {
      .gpio_num = PWM_B_PIN,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_1,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0,
      .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
      .flags = {},
      .deconfigure = false,
  };

  ESP_ERROR_CHECK(ledc_channel_config(&pwm_a_channel_config));
  ESP_ERROR_CHECK(ledc_channel_config(&pwm_b_channel_config));

  ESP_ERROR_CHECK(ledc_fade_func_install(0));

  vTaskDelay(pdMS_TO_TICKS(500));

  // ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 255, 0);
  // ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 220, 0);
  // motor_set(-0.1f);
  // while(1)
  //   vTaskDelay(1000);
  // while(1) {
  //   vTaskDelay(1000);
  //   motor_set(0.5f);
  //   vTaskDelay(pdMS_TO_TICKS(1000));
  //   motor_set(-0.5f);
  //   vTaskDelay(pdMS_TO_TICKS(1000));
  // }

  while(1) {
    if(stopped) {
      motor_set(0.0f);

      while(1) {
        vTaskDelay(1000);
      }
    }

    TickType_t begin_time = xTaskGetTickCount();
    constexpr TickType_t PERIOD = pdMS_TO_TICKS(MOTOR_PERIOD_MS);

    static float fix = 0;
    xQueuePeek(fix_output, &fix, pdMS_TO_TICKS(15));

    // motor_set_a(fix);
    // motor_set_b(fix);
    motor_set(fix);

    vTaskDelayUntil(&begin_time, PERIOD);
  }
}

void motor_set(duty_cycle_t duty) {
  if(duty >= 0) {
    // printf("%f by %d\n", duty, (uint8_t)(UINT8_MAX * (1.0f - duty)));

    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                             (uint8_t)(UINT8_MAX * (1.0f - duty)), 0);
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 255, 0);
  } else {
    // printf("%f by %d\n", duty, (uint8_t)(UINT8_MAX * (1.0f + duty)));

    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 255, 0);
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1,
                             (uint8_t)(UINT8_MAX * (1.0f + duty)), 0);
  }
}

void motor_set_a(duty_cycle_t duty) {
  // gpio_set_level(DIR_A_PIN, duty > 0 ? 1 : 0);
  // if(duty < 0) {
  //   duty *= -1;
  // }
  // ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
  //                          (uint8_t)(UINT8_MAX * duty), 0);
}

void motor_set_b(duty_cycle_t duty) {
  // gpio_set_level(DIR_B_PIN, duty > 0 ? 1 : 0);
  // if(duty < 0) {
  //   duty *= -1;
  // }
  // ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1,
  //                          (uint8_t)(UINT8_MAX * duty), 0);
}
