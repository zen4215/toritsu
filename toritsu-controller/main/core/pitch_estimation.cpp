#include "main.hpp"

#include <cmath>

// types
template <size_t LEN> struct RingBuf {
    float datas[LEN] = {};
    size_t head = 0;
};

// var
static degree_t pitch = 0.0f;
static RingBuf<GYRO_HISTORY_LEN> gyro_history_buf;
static RingBuf<ACCEL_WEIGHT_PITCH_HISTORY_LEN> accel_weight_history_buf;

// fn
template <size_t LEN> void history_update(RingBuf<LEN>& buffer, float new_v);
template <size_t LEN> float lpf_fir_moving_average(RingBuf<LEN>& buffer);
template <size_t LEN> float hpf_fir_moving_average(RingBuf<LEN>& buffer);

// main
degree_t pitch_estimation(const lsm6dsv::Data& data) {
  history_update<GYRO_HISTORY_LEN>(gyro_history_buf, data.gyroY);

  // pitch add gyro diff via lpf
  const float gyro_diff = data.gyroX;
  pitch += gyro_diff * (MAIN_PERIOD_MS / 1000.0f);

  // pitch fix by accel
  const float accel_xz_vec_size =
      std::sqrtf(data.accelX * data.accelX + data.accelZ * data.accelZ);
  const float accel_pitch = atan2f(data.accelY, accel_xz_vec_size) *
                            57.2957795f * -1; // gyroとaccelで逆なので合わせる

  const float accel_vec_size =
      std::sqrtf(data.accelX * data.accelX + data.accelY * data.accelY +
                 data.accelZ * data.accelZ);
  const float accel_g_diff = accel_vec_size - 1.0f;

  const float accel_weight =
      std::expf(-(accel_g_diff * accel_g_diff) /
                (2.0f * ACCEL_PITCH_TRUST_SIZE_G * ACCEL_PITCH_TRUST_SIZE_G));
  history_update<ACCEL_WEIGHT_PITCH_HISTORY_LEN>(accel_weight_history_buf,
                                                 accel_weight);

  pitch = (accel_pitch * accel_weight) +
          ((1 - accel_weight) *
           (pitch)); // ジャイロと加速度の重み付平均(相補フィルタでない)

  return pitch;
}

// helper
template <size_t LEN> void history_update(RingBuf<LEN>& buffer, float new_v) {
  buffer.datas[buffer.head % LEN] = new_v;
  buffer.head++;
}

template <size_t LEN> float lpf_fir_moving_average(RingBuf<LEN>& buffer) {
  float sum = 0.0f;
  for(float v : buffer.datas) {
    sum += v;
  }

  return sum / LEN;
}

template <size_t LEN> float hpf_fir_moving_average(RingBuf<LEN>& buffer) {
  return buffer.datas[(buffer.head - 1) % LEN] - lpf_fir_moving_average(buffer);
}
