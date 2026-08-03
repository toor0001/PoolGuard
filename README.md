# PoolGuard

**Battery-powered pool skimmer monitor for Home Assistant and ESPHome**

[Deutsch](README_DE.md) · English

PoolGuard is a DIY sensor module designed to fit precisely into the lid of an **AstralPool 17.5 L skimmer**. It can be installed without drilling or cutting the original lid and secured with a suitable adhesive or neutral-curing silicone.

The module measures the distance to the water surface and therefore the current pool level. It also monitors the pool-water temperature. By analysing movement and short-term fluctuations at the water surface, PoolGuard can additionally estimate whether the circulation pump is currently running. Significantly stronger and more irregular surface movement can also indicate that someone is currently in the pool.

The electronics are designed for extremely low power consumption. By using deep sleep and fully powering down the sensors between measurements, the goal is to run PoolGuard for an entire pool season on a single battery charge.

## Home Assistant possibilities

PoolGuard data can be used to build automations such as:

- switching a pool heat pump on or off based on the measured water temperature;
- disabling the circulation pump when the water level is too low, helping to prevent dry running;
- sending a warning when the pool level is too high or too low;
- operating a UV-C lamp only while active water circulation is actually detected;
- **person detection in the pool:** if the water-surface movement is significantly stronger and more irregular than the pattern caused by the circulation pump, PoolGuard can infer that someone is probably swimming or bathing.

> **Project status:** prototype / work in progress. The mechanical fit template is currently being tested. Do not drill, cut or permanently glue the original skimmer lid before checking the current CAD notes.

> **Name and affiliation:** PoolGuard is an independent open-source DIY project. It is not affiliated with, endorsed by or connected to any company, brand or commercial product using the PoolGuard name.

## Features

- Water-level monitoring with a waterproof **DFRobot A02YYUW** ultrasonic sensor
- Water temperature with a waterproof **DS18B20**
- **Seeed Studio XIAO ESP32-C3** with external 2.4 GHz antenna
- One protected 18650 Li-ion cell
- Deep sleep for very long battery life
- Battery-voltage monitoring through a resistor divider
- Experimental pump detection from short-term water-surface movement
- Experimental person detection from significantly stronger and irregular surface movement
- Local integration with ESPHome and Home Assistant
- Custom 3D-printed insert designed for the AstralPool skimmer lid

## Repository layout

```text
cad/                 Parametric source and printable test parts
esphome/             ESPHome configuration
hardware/            Wiring notes and bill of materials
docs/                Installation and calibration notes
```

## Current hardware concept

The housing body sits in skimmer compartments 3 and 4 and is bonded to the side ribs with suitable neutral-curing silicone. The original centre rib remains intact. The removable lower lid carries the electronics:

- battery holder and ESP on the dry/internal side;
- A02YYUW on the water-facing side;
- DS18B20 cable through the lid;
- external Wi-Fi antenna close to the plastic skimmer lid.

## Quick start

1. Print and test the fit template from `cad/stl/`.
2. Copy `esphome/secrets.example.yaml` to your ESPHome secrets file and enter your credentials.
3. Review all pins and calibration values in `esphome/poolguard.yaml`.
4. Flash the XIAO ESP32-C3 by USB.
5. Calibrate `distance_empty_cm` and `distance_full_cm` in the installed skimmer.
6. Compare multiple measurement cycles with the pump on and off, and with and without people in the pool, before enabling pump or person detection.

## Safety

- Use a protected, reputable 18650 cell.
- Do not short, crush or reverse the cell.
- Keep electronics sealed from condensation and splash water.
- Use a suitable MOSFET or load switch to fully power down the A02YYUW during deep sleep.
- Treat pump and person detection as indications only, never as the sole basis for a safety-critical shutdown or monitoring function.

## Support

<a href="https://buymeacoffee.com/toor0001"><img src="https://img.buymeacoffee.com/button-api/?text=Buy%20me%20a%20coffee&emoji=%E2%98%95&slug=toor0001&button_colour=FFDD00&font_colour=000000&font_family=Lato&outline_colour=000000&coffee_colour=ffffff" alt="Buy me a coffee"></a>

## License

Software and documentation are released under the MIT License. CAD files are currently prototype files and may receive a separate hardware license before the first stable release.
