#include "main.hpp"

void blink_task(void*) {
  const gpio_config_t builtin_led_gpio_config = {
      .pin_bit_mask = 1 << BUILTIN_LED_PIN,
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };

  gpio_config(&builtin_led_gpio_config);

  int level = 0;
  while(1) {
    level = level ? 0 : 1;
    gpio_set_level(BUILTIN_LED_PIN, level);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
