# PoolGuard

**Battery-powered pool skimmer monitor for Home Assistant and ESPHome**

[Deutsch](README_DE.md) · English

PoolGuard is a DIY sensor module for an **AstralPool 17.5 L skimmer**. It measures water level and water temperature and also analyses movement of the water surface. From this, PoolGuard can estimate whether the circulation pump is running and whether unusually strong surface movement indicates swimming/bathing activity.

The design focuses on low power consumption: the XIAO ESP32-C3 wakes only briefly during normal operation, the A02YYUW is fully power-switched by a Pololu MOSFET, and Wi-Fi is used only when needed.

> **Project status:** prototype / work in progress. Pump and activity detection must be tested and calibrated on the real pool.

> **Important:** PoolGuard does not directly detect a person. It only classifies water-surface motion and must never be used as a safety system or substitute for pool supervision.

## Features

- A02YYUW distance measurement to the water surface
- real water depth from a single reference measurement
- water-level percentage and estimated pool volume
- DS18B20 water temperature
- low-water warning with hysteresis
- circulation-pump detection from surface motion
- experimental swimming/person-activity indication
- guided quiet/pump/person motion calibration
- persistent calibration values
- automatic Initial Setup Mode for commissioning
- Home Assistant Maintenance Mode
- event-driven Wi-Fi/API reporting
- deep sleep and complete A02YYUW power-off
- OTA updates while awake / in maintenance mode
- status heartbeat for external Home Assistant offline monitoring

## Hardware

| Part | Purpose | Link |
|---|---|---|
| Seeed Studio XIAO ESP32-C3 | Microcontroller / ESPHome | [Amazon](https://link.amazon/B0iQQg2zF) |
| USB-C breakout cable | Power / connection | [Amazon](https://link.amazon/B0eO8jr4N) |
| DFRobot A02YYUW | Distance and surface-motion measurement | [Amazon](https://link.amazon/B0dWRfbC4) |
| Waterproof DS18B20 | Water temperature | [Amazon](https://link.amazon/B08FcJbtj) |
| Protected 18650 Li-ion cell | Power supply | – |
| 18650 battery holder | Battery mounting | [Amazon](https://link.amazon/B0hraa6X7) |
| **Pololu Mini MOSFET Slide Switch LV #2810** | Fully switches A02YYUW power | [Pololu](https://www.pololu.com/product/2810) |
| External 2.4 GHz Wi-Fi antenna | Improved Wi-Fi reception | – |

> **Affiliate disclosure:** Some product links may be affiliate links. If you purchase through one of them, I may receive a small commission at no additional cost to you.

## Pin assignment

The firmware deliberately avoids ESP32-C3 strapping pins GPIO2, GPIO8 and GPIO9.

| Function | XIAO ESP32-C3 |
|---|---|
| Pololu ON | D2 / GPIO4 |
| A02YYUW TX → ESP RX | D7 / GPIO20 |
| DS18B20 DATA | D3 / GPIO5 |
| GPIO3 | unused/free |

The DS18B20 requires a **4.7 kΩ pull-up between DATA and 3.3 V**.

```text
XIAO 3.3 V  -------- VIN   Pololu 2810
XIAO GND    -------- GND   Pololu 2810
GPIO4 / D2  -------- ON    Pololu 2810
Pololu VOUT -------- VCC   A02YYUW
A02YYUW GND -------- GND
A02YYUW TX  -------- GPIO20 / D7
A02YYUW RX  -------- leave unconnected
```

Pololu `ON` is active-high. During deep sleep the A02YYUW remains unpowered.

## Normal operation and power saving

Defaults are:

```yaml
sleep_duration: 55s
detection_burst_duration: 5s
status_every_wakes: "30"
```

After commissioning, PoolGuard wakes roughly once per minute. The A02YYUW is powered, samples are collected during the measurement burst, and the sensor is switched off again immediately afterwards. Water level, low-water state, pump state and bathing activity are evaluated locally without turning Wi-Fi on for every wake.

A change in **pump**, **person/bathing activity** or **low-water state** causes PoolGuard to connect to Wi-Fi/Home Assistant during that measurement cycle and report the change. Without a state change, the regular status report is due after about **30 wake cycles**, roughly every 30 minutes. Water temperature is refreshed with that periodic report.

Last states and counters are retained in ESP32 RTC memory across deep sleep. If Wi-Fi/API transmission fails, a state change remains pending and is retried on a later wake.

### Measurement burst vs. battery life

`detection_burst_duration` controls the trade-off between detection robustness and battery life:

```yaml
detection_burst_duration: 5s
```

A longer burst supplies more A02 samples but keeps the ESP and sensor active for longer. **Five seconds is a conservative commissioning/testing value.** After real-world validation, values such as **2 s or 1.5 s** can be tested. At least five valid A02 frames are required for a valid normal cycle, so shorter bursts should only be used after practical testing.

The full wake interval can also be increased later, for example to roughly two minutes. This substantially reduces the number of measurement bursts, but state changes can then only be detected on the next wake. If the wake interval changes, adjust `status_every_wakes` so the desired periodic reporting interval – currently about 30 minutes – stays unchanged.

Guided motion calibration is independent of the normal burst duration and continues to use its own 5-second windows.

## Robust A02 processing

The A02YYUW sends UART frames with a checksum. PoolGuard accepts only valid frames and plausible distances from 30 to 4500 mm. A normal cycle requires at least five valid frames.

Distance is calculated from the **median** of the collected samples. Surface motion is calculated from approximately the 10th-to-90th-percentile span rather than raw min/max values, reducing the influence of isolated outliers and splashes.

## Water level, volume and low-water detection

One known real water depth is enough as a reference. **Set Water Level Reference** stores the current A02 distance together with the actual measured pool depth. Future changes in sensor distance are converted directly into a new water depth.

Default geometry:

```yaml
pool_diameter_m: "5.0"
pool_max_depth_cm: "120.0"
low_water_hysteresis_cm: "1.0"
```

`Water Level` is the current depth relative to configured maximum depth. `Pool Volume` assumes a cylindrical pool and uses `π × radius² × current depth`; at 5.0 m diameter and 120 cm depth this is approximately **23.56 m³**.

`Minimum Safe Water Depth` is set in Home Assistant for the real skimmer. `Low Water Level` becomes active when the minimum is reached or crossed and only clears after the level rises to minimum + hysteresis, preventing rapid toggling around the threshold.

## Initial Setup Mode

A new or reset device automatically starts in **Initial Setup Mode**. `initial_setup_completed` is stored persistently in flash and survives deep sleep, reset and complete power loss. No second firmware flash is required to leave commissioning mode.

While setup is incomplete, PoolGuard stays awake and reachable; the A02YYUW remains off while idle. Commissioning mainly consists of setting the water-level reference, minimum safe depth and optionally the motion profiles. **Finish Initial Setup** stores completion and enters normal deep-sleep operation. **Reset Initial Setup** uses a two-press, 10-second confirmation safeguard.

## Guided motion calibration

Three profiles can be learned:

1. **Calibrate Quiet Water** – pump off, nobody in the pool.
2. **Calibrate Pump** – circulation pump running, nobody in the pool.
3. **Calibrate Person** – representative swimming/bathing movement.

By default each profile uses **12 × 5-second windows**. If the profiles are clearly ordered `quiet < pump < person`, PoolGuard calculates thresholds between the profiles automatically. Otherwise previous or fallback thresholds remain active:

```yaml
pump_motion_threshold_cm: "1.5"
person_motion_threshold_cm: "3.0"
```

Pump motion must normally be confirmed in two consecutive cycles. Person/activity indication switches on faster and needs two quiet cycles to clear. While person-like motion is active, the pump state is deliberately held.

## Maintenance Mode and OTA

Create a persistent Home Assistant helper with exactly this entity ID:

```text
input_boolean.poolguard_maintenance_mode
```

PoolGuard does **not** wake Wi-Fi on every cycle just to check this helper. Home Assistant retains the request and PoolGuard receives it during the next API connection that would have happened anyway. While Maintenance Mode is active, PoolGuard stays awake and continuously performs measurement bursts. Switching it off returns PoolGuard to deep sleep if Initial Setup has been completed.

For reliable OTA updates, enable Maintenance Mode first so PoolGuard stays awake and reachable during the update.

## Battery monitoring without ADC: Status Heartbeat

PoolGuard intentionally has **no battery-voltage measurement**, voltage divider or Battery Level sensor. For the actual requirement – noticing that the battery is empty or the device has stopped reporting – external monitoring in Home Assistant is more robust.

The firmware therefore exposes a diagnostic **Status Heartbeat** value. It is updated after a successful Wi-Fi/API report. With no events, that happens at the normal periodic report, by default about every 30 minutes; state changes can create additional heartbeats.

The notification itself should **not** live in ESPHome: a PoolGuard with an empty battery cannot send a low-battery warning. Home Assistant should detect that expected heartbeats have stopped. A generous threshold such as **2 hours** is recommended so a single missed Wi-Fi connection does not immediately generate an alarm.

A robust HA setup uses an `input_datetime.poolguard_last_seen` helper. One automation updates that helper whenever `sensor.poolguard_status_heartbeat` reports a valid value; a second automation checks whether the timestamp is more than two hours old and then sends a notification. Ignore `unknown` and `unavailable` when updating the helper because PoolGuard is intentionally offline between connections during normal deep-sleep operation.

Example helper-update automation:

```yaml
alias: PoolGuard - Store heartbeat
trigger:
  - platform: state
    entity_id: sensor.poolguard_status_heartbeat
condition:
  - condition: template
    value_template: >-
      {{ trigger.to_state.state not in ['unknown', 'unavailable', 'none'] }}
action:
  - service: input_datetime.set_datetime
    target:
      entity_id: input_datetime.poolguard_last_seen
    data:
      timestamp: "{{ now().timestamp() }}"
mode: queued
```

The alert automation can then check periodically, for example every 15 minutes, whether `poolguard_last_seen` is more than two hours old. This catches an empty battery, Wi-Fi outage or a stuck device with the same mechanism.

## Home Assistant entities

Important values/states include **Water Temperature**, **Distance to Water**, **Water Depth**, **Water Level**, **Pool Volume**, **Water Surface Motion**, **Pump Detected**, **Person Detected**, **Low Water Level**, **Status Heartbeat**, **WiFi Signal** and **Calibration Status**. Diagnostic entities also expose learned motion profiles and active thresholds.

## Mechanical concept

The housing body sits inside the skimmer and is fixed to the side ribs using suitable neutral-curing silicone. The original centre rib remains intact. The body is slightly sloped toward the skimmer so water can drain back. The removable lid carries the battery, XIAO ESP32-C3 and electronics; the A02YYUW and DS18B20 monitor water surface and temperature. An external Wi-Fi antenna can be positioned close to the plastic skimmer lid.

Current printable files are available in [`3D-Files/`](3D-Files/). I use a Bambu Lab A1 Mini.

<p align="center">
  <img src="images/PG.jpg" alt="PoolGuard" width="90%">
</p>

## Safety

- Use a protected, reputable 18650 cell.
- Do not short, crush, reverse or charge the cell unattended.
- Keep electronics protected from condensation and splash water.
- Fully switch the A02YYUW off through the Pololu during deep sleep.
- Never use pump/person/activity detection as the sole basis for safety-critical monitoring or shutdown.

## Support

<a href="https://paypal.me/toor0001/5"><img src="assets/paypal-support-en.svg" alt="Buy me a coffee via PayPal" width="430"></a>

## License

Software and documentation are released under the MIT License. CAD files are currently prototypes and may receive a separate hardware license before the first stable release.

## Development Note

This project was created as part of a collaborative **vibe-coding workflow** with **ChatGPT** and **OpenAI Codex**. Both tools were used for code generation, reviews, troubleshooting and documentation.

Hardware assembly, integration decisions, practical testing and final responsibility for the project remain with the project operator.