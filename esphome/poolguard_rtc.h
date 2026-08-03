#pragma once

#include <cstdint>
#include "esp_attr.h"

// Kept in ESP32 RTC memory so the one-minute detection loop does not need to
// write flash on every deep-sleep cycle. A full power loss resets this state.
struct PoolGuardRtcState {
  uint32_t magic;
  uint16_t wake_count;

  int8_t pump_state;
  int8_t person_state;
  int8_t reported_pump_state;
  int8_t reported_person_state;

  uint8_t pump_on_streak;
  uint8_t pump_off_streak;
  uint8_t person_on_streak;
  uint8_t person_off_streak;
};

static constexpr uint32_t POOLGUARD_RTC_MAGIC = 0x50474D31;  // "PGM1"
RTC_DATA_ATTR inline PoolGuardRtcState poolguard_rtc{};
