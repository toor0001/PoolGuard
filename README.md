# PoolGuard

**Battery-powered pool skimmer monitor for Home Assistant and ESPHome**

[Deutsch](README_DE.md) · English

PoolGuard is a DIY sensor module for an **AstralPool 17.5 L skimmer lid**. It measures the distance to the water surface, the pool-water temperature and the battery voltage. A series of rapid distance measurements can also be used to estimate whether the circulation pump is running from the movement of the water surface.

> **Project status:** prototype / work in progress. The mechanical fit template is currently being tested. Do not drill, cut or glue the original skimmer lid before checking the current CAD notes.

> **Name and affiliation:** PoolGuard is an independent open-source DIY project. It is not affiliated with, endorsed by or connected to any company, brand or commercial product using the PoolGuard name.

## Features

- Water-level monitoring with a waterproof **DFRobot A02YYUW** ultrasonic sensor
- Water temperature with a waterproof **DS18B20**
- **Seeed Studio XIAO ESP32-C3** with external 2.4 GHz antenna
- One 18650 Li-ion cell
- Deep sleep for long battery life
- Battery-voltage monitoring through a resistor divider
- Experimental pump detection from short-term water-surface movement
- Local integration with ESPHome and Home Assistant
- 3D-printed insert designed only for the pool-skimmer project

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
6. Test several pump-on and pump-off measurement cycles before enabling pump detection.

## Safety

- Use a protected, reputable 18650 cell.
- Do not short, crush or reverse the cell.
- Keep electronics sealed from condensation and splash water.
- Use a suitable MOSFET or load switch to fully power down the A02YYUW during deep sleep.
- Treat pump detection as an indication only, not as a safety interlock.

## Support

<a href="https://buymeacoffee.com/toor0001"><img src="https://img.buymeacoffee.com/button-api/?text=Buy%20me%20a%20coffee&emoji=%E2%98%95&slug=toor0001&button_colour=FFDD00&font_colour=000000&font_family=Lato&outline_colour=000000&coffee_colour=ffffff" alt="Buy me a coffee"></a>

## License

Software and documentation are released under the MIT License. CAD files are currently prototype files and may receive a separate hardware license before the first stable release.
