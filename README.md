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
- detecting whether someone is in the pool.

> **Project status:** prototype / work in progress. The mechanical fit template is currently being tested. Do not drill, cut or permanently glue the original skimmer lid before checking the current CAD version.

> **Name and affiliation:** PoolGuard is an independent open-source DIY project. It is not affiliated with, endorsed by or connected to any company, brand or commercial product using the PoolGuard name.

## Parts

The following list will be expanded with purchase links over time:

| Part | Purpose | Link |
|---|---|---|
| Seeed Studio XIAO ESP32-C3 | Microcontroller / ESPHome | [Amazon](https://link.amazon/B0iQQg2zF) |
| USB-C breakout cable | Power / connection | [Amazon](https://link.amazon/B0eO8jr4N) |
| DFRobot A02YYUW | Distance measurement to the water surface | [Amazon](https://link.amazon/B0dWRfbC4) |
| Waterproof DS18B20 | Pool-water temperature | – |
| Protected 18650 Li-ion cell | Power supply | – |
| 18650 battery holder | Battery mounting | [Amazon](https://link.amazon/B0hraa6X7) |
| MOSFET / load switch | Powering down the A02YYUW during deep sleep | – |
| External 2.4 GHz Wi-Fi antenna | Improving Wi-Fi reception | – |

> **Affiliate disclosure:** Some of the product links listed here may be affiliate links. If you purchase something through one of these links, I may receive a small commission at no additional cost to you. This helps support the continued development of my DIY projects.

## Mechanical concept

The housing body sits inside the skimmer and will later be fixed to the side ribs using suitable neutral-curing silicone. The original centre rib remains completely intact. The removable lower lid carries the electronics:

- battery holder and ESP on the dry/internal side;
- A02YYUW on the water-facing side;
- feed-through for the DS18B20 cable;
- external Wi-Fi antenna positioned as close as possible to the plastic skimmer lid.

## Quick start

1. Print the fit template from `cad/stl/` and drill the required holes manually.
2. Use `esphome/secrets.example.yaml` as a template for your own ESPHome secrets.
3. Review all pins and calibration values in `esphome/poolguard.yaml`.
4. Flash the XIAO ESP32-C3 via USB first.
5. Calibrate `distance_empty_cm` and `distance_full_cm` with the module installed in the skimmer.
6. Compare multiple measurement cycles with the pump on and off, and with and without people in the pool, before enabling pump or person detection.

## Safety

- Use a protected, reputable 18650 cell.
- Do not short, crush, reverse or charge the cell unattended.
- Keep the electronics protected from condensation and splash water.
- Use a suitable MOSFET or load switch to fully power down the A02YYUW during deep sleep.
- Treat pump and person detection as indications only, never as the sole basis for a safety-critical shutdown or monitoring function.

## Support

<a href="https://buymeacoffee.com/toor0001"><img src="https://img.buymeacoffee.com/button-api/?text=Buy%20me%20a%20coffee&emoji=%E2%98%95&slug=toor0001&button_colour=FFDD00&font_colour=000000&font_family=Lato&outline_colour=000000&coffee_colour=ffffff" alt="Buy me a coffee"></a>

## License

Software and documentation are released under the MIT License. CAD files are currently prototype files and may receive a separate hardware license before the first stable release.
