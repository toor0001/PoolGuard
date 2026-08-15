#pragma once

#include <cstdint>
#include "esp_attr.h"

// Kept in ESP32 RTC memory so the configurable low-power detection loop does
// not write flash on every deep-sleep cycle. A full power loss resets this state.
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
  uint8_t person_on_streak;
  uint8_t person_off_streak;

  // Remaining person-alert lockout in minutes. This is RTC-only: it survives
  // deep sleep without flash writes, but intentionally resets after power loss.
  uint16_t person_alert_cooldown_minutes;
};

// Bump whenever the RTC structure layout changes so stale RTC data is reset.
static constexpr uint32_t POOLGUARD_RTC_MAGIC = 0x50474D34;  // "PGM4"
RTC_DATA_ATTR inline PoolGuardRtcState poolguard_rtc{};
