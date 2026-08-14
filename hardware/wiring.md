# Wiring / Verdrahtung

> Prototype documentation. Confirm the pin labels on the exact XIAO ESP32-C3 board before soldering.

## GPIO assignment

| XIAO pin | ESP32-C3 GPIO | Function | Notes |
|---|---:|---|---|
| D7 / RX | GPIO20 | A02 UART RX | Sensor TX connects to ESP RX |
| D2 | GPIO4 | A02 power control | Drives the Pololu 2810 ON pin; do not power the A02 directly from the GPIO |
| D3 | GPIO5 | DS18B20 data | Add 4.7 kΩ pull-up to 3.3 V |
| D1 / A1 | GPIO3 | A02 trigger output | Connects to A02 RX (yellow); falling edge requests one measurement |

These assignments are substitutions at the top of `esphome/poolguard.yaml` and can be changed without editing the rest of the configuration.
GPIO2, GPIO8 and GPIO9 are ESP32-C3 strapping pins; none is used here.

## Block diagram

```text
Protected 18650
     │
     ├──────────────> XIAO battery/power input
     │
     ├─ Pololu 2810 ─────────────> A02YYUW VCC
     │                              A02 TX ─────> D7 / GPIO20
     │                              A02 RX <───── D1 / GPIO3
     │                              A02 GND ────> GND

3.3 V ── 4.7 kΩ ──┬──────────────> D3 / GPIO5
                  └──────────────> DS18B20 DATA
3.3 V ───────────────────────────> DS18B20 VCC
GND   ───────────────────────────> DS18B20 GND
```

## Important details

### A02 power switching

The distance sensor must be fully switched off during deep sleep. D2/GPIO4 is only a control signal for the Pololu 2810 ON input; do not power the sensor directly from the GPIO. Put the Pololu's physical slide switch in the off position so that its ON input controls the output. ON is active-high and the Pololu remains off when ON is low or disconnected. ESPHome initializes the GPIO output inactive; it is explicitly switched off before deep sleep, and the Pololu remains off when GPIO4 becomes high-impedance during reset or sleep. Confirm that the sensor supply is disconnected without back-powering through its UART line.

A series resistor on the A02 TX line may be useful, and the UART line should not be allowed to feed the powered-down sensor. The final switching circuit is still to be validated on the bench.

### A02 UART-Controlled sequence

The firmware keeps A02 RX/GPIO3 high while idle. For every measurement phase it
first establishes that idle level, enables the complete sensor supply through
the Pololu and waits 200 ms for startup. It then generates a 2 ms low pulse
followed by 78 ms high, so falling-edge triggers are 80 ms apart. The A02 reply
is read at 9600 baud, 8N1 on GPIO20 and accepted only when the
`FF Data_H Data_L Checksum` frame, checksum and distance range are valid. After
the trigger loop stops, the firmware allows another 70 ms for the final reply
before switching off the Pololu. A five-second burst therefore requests about
63 readings. The exact DYP sensor variant and its controlled-UART response must
still be confirmed on the physical prototype.

### DS18B20

Use the normal three-wire connection rather than parasite power. Fit an external pull-up of about 4.7 kΩ between 3.3 V and the data line. Keep the cable entry sealed and provide strain relief.

### Antenna

Place the adhesive antenna close to the plastic skimmer lid, away from the 18650 cell, sensor cable and large water-facing surfaces. Do not bend the active antenna area sharply.

## Before closing the housing

1. Verify polarity and resistance values.
2. Measure current in active mode and deep sleep.
3. Confirm that A02 current falls to effectively zero when switched off.
4. Confirm reliable Wi-Fi connection with the skimmer lid in its real position.
5. Run several hours with the housing open before sealing it.
