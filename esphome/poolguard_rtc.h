#pragma once

#include <cstdint>
#include "esp_attr.h"

// Kept in ESP32 RTC memory so the configurable low-power detection loop does
// not write flash on every deep-sleep cycle. A full power loss resets this state.
//
// Maintenance Mode evaluates many measurement bursts during the same ESP boot,
// while normal battery operation evaluates one burst per wake. CycleAwareStreak
// uses that distinction to prevent a short pump-start transient from satisfying
// confirmation rules merely because Maintenance Mode samples much faster.
struct PoolGuardRtcState;
uint16_t poolguard_current_wake_count();

template<uint8_t SameWakeMinimum>
struct CycleAwareStreak {
  uint8_t value;
  uint16_t first_wake_count;

  CycleAwareStreak &operator=(uint8_t new_value) {
    value = new_value;
    if (new_value == 0) first_wake_count = 0;
    return *this;
  }

  operator uint8_t() const { return value; }
  bool operator<(int rhs) const { return value < rhs; }

  void operator++(int) {
    if (value == 0) first_wake_count = poolguard_current_wake_count();
    if (value < 255) value++;
  }

  bool operator>=(int configured_minimum) const {
    int required = configured_minimum;
    if (value > 0 && poolguard_current_wake_count() == first_wake_count &&
        SameWakeMinimum > required) {
      required = SameWakeMinimum;
    }
    return value >= required;
  }
};

struct PoolGuardRtcState {
  uint32_t magic;
  uint32_t report_sequence;
  uint16_t wake_count;

  int8_t pump_state;
  int8_t person_state;
  int8_t low_water_state;
  int8_t reported_pump_state;
  int8_t reported_person_state;
  int8_t reported_low_water_state;

  uint8_t pump_on_streak;
  uint8_t pump_off_streak;

  // Person-like motion must persist across two normal battery wakes. During
  // Maintenance Mode, where repeated bursts occur within one wake, require at
  // least three consecutive bursts before entering the person state.
  CycleAwareStreak<3> person_on_streak;
  uint8_t person_off_streak;

  // Low water still confirms after two consecutive normal battery wakes. If
  // many bursts are evaluated within one wake (Maintenance Mode), require at
  // least twelve consecutive low readings (~1 minute) instead, so pump-start
  // turbulence cannot immediately trigger the low-water safety chain.
  CycleAwareStreak<12> low_water_on_streak;
  float low_water_trigger_depth_cm;

  // Remaining person-alert lockout in minutes. This is RTC-only: it survives
  // deep sleep without flash writes, but intentionally resets after power loss.
  uint16_t person_alert_cooldown_minutes;
};

// Bump whenever the RTC structure layout changes so stale RTC data is reset.
static constexpr uint32_t POOLGUARD_RTC_MAGIC = 0x50474D36;  // "PGM6"
RTC_DATA_ATTR inline PoolGuardRtcState poolguard_rtc{};

inline uint16_t poolguard_current_wake_count() {
  return poolguard_rtc.wake_count;
}
