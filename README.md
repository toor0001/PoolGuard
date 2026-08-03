# PoolGuard

**Battery-powered pool skimmer monitor for Home Assistant and ESPHome**

[Deutsch](README_DE.md) · English

PoolGuard is a DIY sensor module designed to fit precisely into the lid of an **AstralPool 17.5 L skimmer** and can be glued in place without drilling the original lid. The module measures the distance to the water surface and derives the current pool-water depth and level. It also monitors pool-water temperature. By analysing movement and short-term fluctuations at the water surface, PoolGuard can additionally estimate whether the circulation pump is running. Significantly stronger and more irregular surface movement can indicate that somebody is currently in the pool. The electronics are designed for very low power consumption. Deep sleep and fully powering down the A02YYUW between checks are intended to make one battery charge last for an entire pool season.

## Home Assistant possibilities

PoolGuard exposes values and states that can be used for automations such as:

- switching a pool heat pump on or off based on water temperature;
- disabling the circulation pump when the water level is too low;
- sending a notification when the minimum safe water depth is crossed;
- operating a UV-C lamp only while circulation is actually detected;
- detecting unusually strong pool activity that can indicate somebody is in the pool;
- displaying current water depth, water-level percentage and estimated pool volume.

> **Project status:** prototype / work in progress. Do not drill, cut or permanently glue the original skimmer lid before checking the current version.

> **Name and affiliation:** PoolGuard is an independent open-source DIY project. It is not affiliated with, endorsed by or connected to any company, brand or commercial product using the PoolGuard name.

## Parts

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

> **Affiliate disclosure:** Some product links may be affiliate links. If you purchase through one of them, I may receive a small commission at no additional cost to you.

## A02YYUW power switching

The A02YYUW is powered through a **Pololu Mini MOSFET Slide Switch with Reverse Voltage Protection, LV (#2810)**.

```text
XIAO 3.3 V  -------- VIN   Pololu 2810
XIAO GND    -------- GND   Pololu 2810
GPIO5       -------- ON    Pololu 2810
Pololu VOUT -------- VCC   A02YYUW
A02YYUW GND -------- GND
A02YYUW TX  -------- GPIO20
A02YYUW RX  -------- leave unconnected
```

- **GPIO5 HIGH:** A02YYUW powered on
- **GPIO5 LOW:** A02YYUW powered off
- during deep sleep the A02YYUW remains unpowered

## Measurement and low-power operation

In normal operation PoolGuard wakes roughly once per minute and performs a short local A02YYUW measurement burst. Wi-Fi remains disabled for these checks. PoolGuard only connects immediately when the detected pump, pool-activity or low-water state changes. Water depth, water-level percentage, estimated pool volume, water temperature and battery level are additionally reported approximately every 30 minutes.

The last reported states are kept in ESP32 RTC memory during deep sleep. If a Wi-Fi/API transmission fails, the state change remains pending and is retried after the next wake-up.

## Water-level reference calibration

PoolGuard no longer requires separate "empty" and "full" distance calibration points. One known real water depth is enough.

For initial commissioning, temporarily set `calibration_mode_on_boot: "true"` in `esphome/poolguard.yaml` and flash the XIAO. Then:

1. Measure the **actual current water depth** in the pool in centimetres.
2. Enter this value in the Home Assistant number entity **Reference Water Depth**.
3. Press **Set Water Level Reference** while the water remains at that same level.
4. PoolGuard stores the current A02 distance together with the known real water depth.
5. Enter the real **Minimum Safe Water Depth** at which the skimmer can still supply the circulation pump safely.

From then on, PoolGuard calculates the current depth from the change in A02 distance. The distance from the sensor to the pool bottom does not have to be measured separately.

The default geometry in the YAML is a round pool with **5.0 m diameter** and **120 cm maximum depth**. `Water Level` is calculated as a percentage of that configured maximum depth. `Pool Volume` assumes a cylindrical pool and uses the current measured water depth. With the default geometry, 120 cm corresponds to approximately **23.56 m³**. For another pool, change `pool_diameter_m` and `pool_max_depth_cm` in the YAML. Pools with non-cylindrical bottoms will only get an approximate volume.

## Guided motion calibration

Pump and pool-activity detection depend heavily on the actual skimmer, pump flow, water level and pool geometry. PoolGuard therefore includes guided calibration instead of relying only on fixed thresholds.

In calibration mode run these three buttons in order:

1. **Calibrate Quiet Water** – pump off and nobody in the pool.
2. **Calibrate Pump** – circulation pump running, nobody in the pool.
3. **Calibrate Person** – normal swimming/bathing movement in the pool.

Each phase measures about 60 seconds and stores the median motion profile. Motion is evaluated from a trimmed sample span so single outliers or splashes have less influence than a raw min/max range. If the profiles are clearly ordered `quiet < pump < person`, PoolGuard automatically calculates the pump and person/activity thresholds. If they overlap, the previous or fallback thresholds remain active.

After water-level and motion calibration, set `calibration_mode_on_boot` back to `false` and flash PoolGuard again for normal battery-saving operation.

> **Important:** PoolGuard does not detect a person directly. It classifies water-surface motion. Pump/activity detection is therefore an experimental indication and must not be used as a safety system or substitute for pool supervision.

## Mechanical concept

The housing body sits inside the skimmer and is fixed to the side ribs using suitable neutral-curing silicone. The original centre rib remains intact. The body is intentionally **slightly sloped towards the inside of the skimmer**, so any water that reaches the housing can drain back into the skimmer instead of collecting on the body. The removable lower lid carries the electronics:

- battery holder and ESP on the dry/internal side;
- A02YYUW on the water-facing side;
- feed-through for the DS18B20 cable;
- external Wi-Fi antenna positioned close to the plastic skimmer lid.

The current printable files are available in [`3D-Files/`](3D-Files/).

## Quick start

1. Download and print the current files from `3D-Files/`.
2. Use `esphome/secrets.example.yaml` as a template for your own ESPHome secrets.
3. Review pins, pool geometry and calibration values in `esphome/poolguard.yaml`.
4. Flash the XIAO ESP32-C3 via USB.
5. Set `calibration_mode_on_boot: "true"` and flash the commissioning configuration.
6. Measure the real water depth, enter **Reference Water Depth** and press **Set Water Level Reference**.
7. Set **Minimum Safe Water Depth** for the real skimmer/pump installation.
8. Run the quiet-water, pump and person/activity calibration.
9. Set `calibration_mode_on_boot` back to `false` and flash the normal low-power configuration.

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

## Project creation

This project was developed with support from ChatGPT and Codex.
