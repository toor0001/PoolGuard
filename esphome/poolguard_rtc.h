#pragma once

#include <cstdint>
#include "esp_attr.h"

// Kept in ESP32 RTC memory so the configurable low-power detection loop does
// not write flash on every deep-sleep cycle. A full power loss resets this state.
//
// Normal battery operation evaluates one burst per wake. Maintenance Mode can
// evaluate many bursts during the same ESP boot. CycleAwareStreak therefore
// counts at most once per wake and never confirms a new state in the same wake
// in which the candidate started. This prevents fast Maintenance Mode sampling
// from turning one pump-start event into many independent confirmations.
struct PoolGuardRtcState;
uint16_t poolguard_current_wake_count();

template<uint8_t Tag>
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
    const uint16_t current_wake = poolguard_current_wake_count();
    if (value == 0) {
      first_wake_count = current_wake;
      value = 1;
      return;
    }

    // Repeated Maintenance Mode bursts happen within one wake. They must not
    // accumulate confirmation credit; only a later real wake may advance it.
    if (current_wake == first_wake_count) return;

    if (value < 255) value++;
  }

  bool operator>=(int configured_minimum) const {
    if (value == 0) return false;

    // A candidate may never become confirmed during the same wake in which it
    // started. In normal battery operation the next deep-sleep wake can confirm
    // it; Maintenance Mode remains observational and cannot manufacture extra
    // confirmations just by sampling every few seconds.
    if (poolguard_current_wake_count() == first_wake_count) return false;

    return value >= configured_minimum;
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

  // Person-like motion must persist into a later real wake before entering the
  // person state. Repeated Maintenance Mode bursts in one wake do not count.
  CycleAwareStreak<3> person_on_streak;
  uint8_t person_off_streak;

  // Low water still requires consecutive normal battery wakes. Repeated
  // Maintenance Mode bursts in one wake do not count as extra confirmations.
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
