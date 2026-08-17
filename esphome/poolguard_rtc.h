#pragma once

#include <cmath>
#include <cstdint>
#include "esp_attr.h"
#include "esp_system.h"
#include "esp_sleep.h"

// Kept in ESP32 RTC memory so the configurable low-power detection loop does
// not write flash on every deep-sleep cycle. A full power loss resets this state.
//
// Normal battery operation evaluates one burst per wake. Maintenance Mode can
// evaluate many bursts during the same ESP boot. CycleAwareStreak therefore
// counts at most once per wake and never confirms a new state in the same wake
// in which the candidate started. If repeated same-wake samples were seen, the
// first later wake starts a fresh candidate instead of inheriting Maintenance
// Mode evidence. This prevents a Maintenance -> normal transition from causing
// an immediate confirmation after a very short sleep.
struct PoolGuardRtcState;
uint16_t poolguard_current_wake_count();

template<uint8_t Tag>
struct CycleAwareStreak {
  uint8_t value;
  uint16_t first_wake_count;
  bool repeated_same_wake;

  CycleAwareStreak &operator=(uint8_t new_value) {
    value = new_value;
    if (new_value == 0) {
      first_wake_count = 0;
      repeated_same_wake = false;
    }
    return *this;
  }

  operator uint8_t() const { return value; }
  bool operator<(int rhs) const { return value < rhs; }

  void operator++(int) {
    const uint16_t current_wake = poolguard_current_wake_count();
    if (value == 0) {
      first_wake_count = current_wake;
      repeated_same_wake = false;
      value = 1;
      return;
    }

    // Repeated Maintenance Mode bursts happen within one wake. They must not
    // accumulate confirmation credit. Remember that repeated same-wake evidence
    // occurred so the first later wake can deliberately start fresh.
    if (current_wake == first_wake_count) {
      repeated_same_wake = true;
      return;
    }

    // A candidate observed repeatedly during one long wake most likely came
    // from Maintenance Mode. Do not carry that 1/2 state across the transition
    // into normal battery operation; make this later wake the new first sample.
    if (repeated_same_wake) {
      first_wake_count = current_wake;
      repeated_same_wake = false;
      value = 1;
      return;
    }

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

// The circulation pump lowers the local level inside this skimmer by roughly
// 2.5-3 cm even though the actual pool level is unchanged. The normal editable
// low-water limit therefore remains valid while the pump is stopped, while an
// already detected/pending pump gets a lower emergency floor. This value is
// deliberately conservative relative to the observed ~100.4-101.2 cm running
// level; a genuinely falling pump-running level can still trigger protection.
static constexpr float POOLGUARD_PUMP_RUNNING_LOW_WATER_DEPTH_CM = 99.0f;

struct PumpAwareLowWaterTriggerDepth {
  float value;

  PumpAwareLowWaterTriggerDepth &operator=(float new_value);
  operator float() const { return value; }
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
  PumpAwareLowWaterTriggerDepth low_water_trigger_depth_cm;

  // Remaining person-alert lockout in minutes. This is RTC-only: it survives
  // deep sleep without flash writes, but intentionally resets after power loss.
  uint16_t person_alert_cooldown_minutes;
};

// Bump whenever the RTC structure layout changes so stale RTC data is reset.
static constexpr uint32_t POOLGUARD_RTC_MAGIC = 0x50474D38;  // "PGM8"
RTC_DATA_ATTR inline PoolGuardRtcState poolguard_rtc{};

inline uint16_t poolguard_current_wake_count() {
  return poolguard_rtc.wake_count;
}

inline PumpAwareLowWaterTriggerDepth &PumpAwareLowWaterTriggerDepth::operator=(float new_value) {
  value = new_value;

  // poolguard.yaml stores the confirming depth immediately after setting the
  // low-water state. At that exact point we can reject only the known hydraulic
  // pump drawdown without changing the normal user-configured low-water limit.
  if (std::isfinite(new_value) && poolguard_rtc.low_water_state != 0) {
    const bool pump_context =
        poolguard_rtc.pump_state != 0 || poolguard_rtc.pump_on_streak > 0;

    if (pump_context && new_value > POOLGUARD_PUMP_RUNNING_LOW_WATER_DEPTH_CM) {
      poolguard_rtc.low_water_state = 0;
      value = NAN;
    }
  }

  return *this;
}
