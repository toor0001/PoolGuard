# Wiring / Verdrahtung

> Prototype documentation. Confirm the pin labels on the exact XIAO ESP32-C3 board before soldering.

## Proposed GPIO assignment

| Function | ESP32-C3 GPIO | Notes |
|---|---:|---|
| A02 UART RX | GPIO20 | Sensor TX connects to ESP RX |
| A02 power control | GPIO5 | Drives the Pololu 2810 ON pin; do not power the A02 directly from the GPIO |
| DS18B20 data | GPIO4 | Add 4.7 kΩ pull-up to 3.3 V |
| Battery ADC | GPIO3 | Connect only through the resistor divider |

These assignments are substitutions at the top of `esphome/poolguard.yaml` and can be changed without editing the rest of the configuration.

## Block diagram

```text
Protected 18650
     │
     ├──────────────> XIAO battery/power input
     │
     ├─ Pololu 2810 ─────────────> A02YYUW VCC
     │                              A02 TX ─────> GPIO20
     │                              A02 GND ────> GND
     │
     ├─ 1 MΩ ──┬─────────────────> GPIO3 ADC
     │          │
     │        330 kΩ
     │          │
     └──────────┴─────────────────> GND

3.3 V ── 4.7 kΩ ──┬──────────────> GPIO4
                  └──────────────> DS18B20 DATA
3.3 V ───────────────────────────> DS18B20 VCC
GND   ───────────────────────────> DS18B20 GND
```

## Important details

### A02 power switching

The distance sensor must be fully switched off during deep sleep. GPIO5 is only a control signal for the Pololu 2810 ON input; do not power the sensor directly from the GPIO. Confirm that the sensor supply is disconnected without back-powering through its UART line.

A series resistor on the A02 TX line may be useful, and the UART line should not be allowed to feed the powered-down sensor. The final switching circuit is still to be validated on the bench.

### DS18B20

Use the normal three-wire connection rather than parasite power. Fit an external pull-up of about 4.7 kΩ between 3.3 V and the data line. Keep the cable entry sealed and provide strain relief.

### Battery measurement

The proposed 1 MΩ / 330 kΩ divider reduces current consumption. The ADC value must be calibrated against a trusted multimeter. The capacitor near the ADC input helps stabilise the high-impedance divider.

### Antenna

Place the adhesive antenna close to the plastic skimmer lid, away from the 18650 cell, sensor cable and large water-facing surfaces. Do not bend the active antenna area sharply.

## Before closing the housing

1. Verify polarity and resistance values.
2. Measure current in active mode and deep sleep.
3. Confirm that A02 current falls to effectively zero when switched off.
4. Check battery ADC against a multimeter.
5. Confirm reliable Wi-Fi connection with the skimmer lid in its real position.
6. Run several hours with the housing open before sealing it.
