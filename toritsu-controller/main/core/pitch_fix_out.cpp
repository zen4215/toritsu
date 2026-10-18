#include "main.hpp"

duty_cycle_t pitch_fix_out(degree_t target_pitch, degree_t now_pitch) {
  duty_cycle_t output = 0.0f;

  degree_t error = target_pitch - now_pitch;

  if((error * error) <
     (FIX_PITCH_DAEDZONE_SIZE * FIX_PITCH_DAEDZONE_SIZE)) { // deadzone
    return 0.0f;
  }

  static float integral = 0.0f;
  integral += error;

  if((integral * integral) > (INTEGRAL_MAX * INTEGRAL_MAX)) {
    integral = INTEGRAL_MAX * (integral > 0 ? 1.0f : -1.0f);
  }

  output = error * DUTY_KP + integral * DUTY_KI;

  if((output * output) >
     (FIX_PITCH_DUTY_MAX * FIX_PITCH_DUTY_MAX)) { // output clamp
    output = FIX_PITCH_DUTY_MAX * (output > 0.0f ? 1.0f : -1.0f);
  }

  printf("e: %f, i: %f, o: %f\n", error, integral, output);

  return output;
}
