# PoolGuard

**Battery-powered pool skimmer monitor for Home Assistant and ESPHome**

[Deutsch](README_DE.md) · English

PoolGuard is a DIY sensor module designed to fit precisely into the lid of an **AstralPool 17.5 L skimmer** and can be glued in place without drilling the original lid. The module measures the distance to the water surface and therefore the current pool level. It also monitors the pool-water temperature. By analysing movement and short-term fluctuations at the water surface, PoolGuard can additionally estimate whether the circulation pump is currently running. Significantly stronger and more irregular surface movement can also indicate that someone is currently in the pool. The electronics are designed for extremely low power consumption. By using deep sleep and fully powering down the sensors between measurements, the goal is to run PoolGuard for an entire pool season on a single battery charge.

## Home Assistant possibilities

PoolGuard data can be used to build automations such as:

- switching a pool heat pump on or off based on the measured water temperature;
- disabling the circulation pump when the water level is too low, helping to prevent dry running;
- sending a warning when the pool level is too high or too low;
- operating a UV-C lamp only while active water circulation is actually detected;
- detecting unusually strong pool activity that can indicate somebody is in the pool.

> **Project status:** prototype / work in progress. Do not drill, cut or permanently glue the original skimmer lid before checking the current version.

> **Name and affiliation:** PoolGuard is an independent open-source DIY project. It is not affiliated with, endorsed by or connected to any company, brand or commercial product using the PoolGuard name.

## Parts

The following list will be expanded with purchase links over time:

| Part | Purpose | Link |
|---|---|---|
| Seeed Studio XIAO ESP32-C3 | Microcontroller / ESPHome | [Amazon](https://link.amazon/B0iQQg2zF) |
| USB-C breakout cable | Power / connection | [Amazon](https://link.amazon/B0eO8jr4N) |
| DFRobot A02YYUW | Distance measurement to the water surface | [Amazon](https://link.amazon/B0dWRfbC4) |
| Waterproof DS18B20 | Pool-water temperature | [Amazon](https://link.amazon/B08FcJbtj) |
| Protected 18650 Li-ion cell | Power supply | – |
| 18650 battery holder | Battery mounting | [Amazon](https://link.amazon/B0hraa6X7) |
| **Pololu Mini MOSFET Slide Switch LV #2810** | Fully powers down the A02YYUW during deep sleep | [Pololu](https://www.pololu.com/product/2810) |
| External 2.4 GHz Wi-Fi antenna | Improving Wi-Fi reception | – |

> **Affiliate disclosure:** Some of the product links listed here may be affiliate links. If you purchase something through one of these links, I may receive a small commission at no additional cost to you. This helps support the continued development of my DIY projects.

## A02YYUW power switching

The A02YYUW is powered through a ready-made **Pololu Mini MOSFET Slide Switch with Reverse Voltage Protection, LV (#2810)**. The module operates from 2 V and is therefore suitable for the XIAO ESP32-C3's 3.3 V supply.

For automatic operation, leave the physical slide switch on the Pololu module in the **OFF** position. The ESP32 then controls the module through its `ON` input.

```text
XIAO 3.3 V  -------- VIN   Pololu 2810
XIAO GND    -------- GND   Pololu 2810
GPIO2       -------- ON    Pololu 2810
Pololu VOUT -------- VCC   A02YYUW
A02YYUW GND -------- GND
A02YYUW TX  -------- GPIO20
A02YYUW RX  -------- leave unconnected
```

This gives the following behavior:

- **GPIO2 HIGH:** A02YYUW powered on
- **GPIO2 LOW:** A02YYUW powered off
- during deep sleep the A02YYUW remains unpowered

## Measurement and low-power operation

In normal operation PoolGuard wakes roughly once per minute and performs a short A02YYUW measurement burst locally. Wi-Fi remains disabled for these checks. PoolGuard only connects to Wi-Fi immediately when the detected pump, pool-activity or low-water state changes. Water level, water temperature and battery level are additionally reported approximately every 30 minutes.

The last reported states are kept in ESP32 RTC memory during deep sleep. This avoids unnecessary flash writes every minute. If a Wi-Fi/API transmission fails, the state change remains pending and is retried after the next wake-up.

## Guided motion calibration

Pump and pool-activity detection depend heavily on the actual skimmer, pump flow, water level and pool geometry. PoolGuard therefore includes a guided calibration mode instead of relying only on fixed thresholds.

For initial commissioning, temporarily set `calibration_mode_on_boot: "true"` in `esphome/poolguard.yaml` and flash the XIAO. PoolGuard then stays awake and connected to Home Assistant. Run the three calibration buttons in this order:

1. **Calibrate Quiet Water** – pump off and nobody in the pool.
2. **Calibrate Pump** – circulation pump running, nobody in the pool.
3. **Calibrate Person** – normal swimming/bathing movement in the pool.

Each phase measures about 60 seconds. Internally this is split into twelve five-second windows. PoolGuard stores the **median** water-motion range, so one unusual splash should not dominate the calibration. When the learned profiles are clearly ordered `quiet < pump < person`, PoolGuard automatically places the pump threshold halfway between quiet and pump, and the person/activity threshold halfway between pump and person.

The learned values are stored persistently and survive normal power cycles. If the three profiles overlap or are in the wrong order, the previous/fallback thresholds remain active and the calibration should be repeated. Diagnostic entities in Home Assistant show the three learned motion profiles, the active thresholds and the current calibration status.

After calibration, set `calibration_mode_on_boot` back to `false` and flash PoolGuard once more to return to the normal battery-saving mode.

> **Important:** PoolGuard does not detect a person directly. It classifies water-surface motion. Pump/activity detection is therefore an experimental indication and must not be used as a safety system or as a substitute for pool supervision.

## Mechanical concept

The housing body sits inside the skimmer and will later be fixed to the side ribs using suitable neutral-curing silicone. The original centre rib remains completely intact. The removable lower lid carries the electronics:

- battery holder and ESP on the dry/internal side;
- A02YYUW on the water-facing side;
- feed-through for the DS18B20 cable;
- external Wi-Fi antenna positioned as close as possible to the plastic skimmer lid.

The current printable files are available in [`3D-Files/`](3D-Files/).

## Quick start

1. Download and print the current files from `3D-Files/`.
2. Use `esphome/secrets.example.yaml` as a template for your own ESPHome secrets.
3. Review all pins and calibration values in `esphome/poolguard.yaml`.
4. Flash the XIAO ESP32-C3 via USB first.
5. Calibrate `distance_empty_cm` and `distance_full_cm` with the module installed in the skimmer.
6. Determine the real minimum safe water level for your skimmer/pump and adjust `min_safe_water_level_percent`.
7. Run the guided quiet-water, pump and person/activity calibration described above.
8. Return `calibration_mode_on_boot` to `false` and flash the normal low-power configuration.

## Safety

- Use a protected, reputable 18650 cell.
- Do not short, crush, reverse or charge the cell unattended.
- Keep the electronics protected from condensation and splash water.
- Use the Pololu 2810 to fully power down the A02YYUW during deep sleep.
- Treat pump and person/activity detection as indications only, never as the sole basis for a safety-critical shutdown or monitoring function.

## Support

<a href="https://paypal.me/toor0001"><img src="assets/paypal-support-en.svg" alt="Buy me a coffee via PayPal" width="430"></a>

## License

Software and documentation are released under the MIT License. CAD files are currently prototype files and may receive a separate hardware license before the first stable release.
