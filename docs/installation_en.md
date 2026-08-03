# Installation and calibration

## Project status

PoolGuard is still a prototype. The first step is only to test the 5 mm fit template. The final housing body and electronics lid will be published after the contour has been confirmed.

## 1. Test the fit template

1. Print `cad/stl/PoolGuard_Passschablone_V1.stl` with a reasonably fine layer height.
2. Supports should normally not be required for the flat template.
3. Place it gently into skimmer compartments 3 and 4.
4. Check the outer radius, inner radius and both straight sides.
5. Confirm that the original centre rib passes freely through the open template.
6. Record any error in millimetres and take photos if possible.

Do not cut, drill or glue the original lid yet.

## 2. Planned final housing installation

The final body will be a 60-degree annular sector matching compartments 3 and 4. The original centre rib remains untouched. The body sides will be bonded to the existing skimmer ribs with neutral-curing silicone suitable for permanent damp conditions.

The body is the permanently installed enclosure tub. The removable lower lid carries the battery holder, XIAO ESP32-C3 and wiring. The A02YYUW is mounted on the water-facing side of the removable lid.

## 3. Bench-test the electronics

Before installation:

1. Flash ESPHome with deep sleep temporarily disabled.
2. Confirm the DS18B20 address and temperature reading.
3. Compare A02 raw distance readings with a ruler.
4. Calibrate battery voltage against a multimeter.
5. Verify complete A02 power shutdown.
6. Test Wi-Fi at the actual skimmer location using the external antenna.

## 4. Calibrate water level

PoolGuard measures the distance from the sensor to the water surface. A smaller distance means a higher water level.

- `distance_empty_cm`: distance at the lowest acceptable water level.
- `distance_full_cm`: distance at the highest desired water level.

The firmware maps this range linearly to 0–100% and clamps values outside the range.

## 5. Calibrate pump detection

Pump detection is experimental. During each wake cycle, PoolGuard collects a burst of distance readings. The difference between the maximum and minimum distance is published as `Water Surface Motion`.

Procedure:

1. Record several cycles with the pump off.
2. Record several cycles with the pump running.
3. Choose a threshold that separates both groups as reliably as possible.
4. Adjust `pump_motion_threshold_cm` in the YAML.

Foam, wind, swimmers, rain or floating objects can cause false results. Never use this indication as a safety interlock.

## 6. Sealing

- Provide strain relief at every cable entry.
- Do not completely pot the electronics in silicone.
- Run a condensation test before final closure.
- Inspect the removable-lid seal periodically.
- Use only plastic-compatible neutral-curing sealants.
