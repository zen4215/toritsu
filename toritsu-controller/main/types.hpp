#pragma once

#include <cstdint>

using grav_accel_t = float;
using deg_per_sec_t = float;
using degree_t = float;
using duty_cycle_t = float;

namespace lsm6dsv {

struct Data {
  public:
    deg_per_sec_t gyroX;
    deg_per_sec_t gyroY;
    deg_per_sec_t gyroZ;

    grav_accel_t accelX;
    grav_accel_t accelY;
    grav_accel_t accelZ;
};

enum class OutputDataRate : uint8_t {
  POWER_DOWN = 0x0,
  HZ_1_875 = 0x1,
  HZ_7_5 = 0x2,
  HZ_15 = 0x3,
  HZ_30 = 0x4,
  HZ_60 = 0x5,
  HZ_120 = 0x6,
  HZ_240 = 0x7,
  HZ_480 = 0x8,
  HZ_960 = 0x9,
  KHZ_1_92 = 0xa,
  KHZ_3_84 = 0xb,
  KHZ_7_68 = 0xc,
};

enum class AccelFullScale {
  G_2,
  G_4,
  G_8,
  G_16,
};

enum class GyroFullScale {
  DPS_125,
  DPS_250,
  DPS_500,
  DPS_1000,
  DPS_2000,
  DPS_4000,
};

enum class WiFiMode {
  AP,
  STA,
};

enum class PWMSource {
  MCPWM,
  LEDC,
};

struct BlinkPattern {
    struct BlinkChild {
        bool isHigh;
        uint16_t interval;
    };

    BlinkChild* children;
    uint8_t len;
};

} // namespace lsm6dsv
