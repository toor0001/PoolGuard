# Bill of materials / Stückliste

This list describes the current PoolGuard prototype. Exact connectors, cable lengths and fasteners may change after the mechanical fit test.

| Qty | Component | Notes |
|---:|---|---|
| 1 | Seeed Studio XIAO ESP32-C3 | Version with external antenna connector preferred |
| 1 | 2.4 GHz adhesive antenna | U.FL/IPEX connector matching the XIAO board |
| 1 | DFRobot A02YYUW | Waterproof UART ultrasonic distance sensor |
| 1 | Waterproof DS18B20 probe | Three-wire version recommended |
| 1 | 18650 holder | Single-cell holder, approximately 70 × 20 × 19 mm, centre screw hole |
| 1 | Protected 18650 cell | Reputable manufacturer and suitable capacity |
| 1 | P-channel MOSFET or load switch | High-side switching of the A02 supply; final circuit still to be validated |
| 1 | 4.7 kΩ resistor | DS18B20 data pull-up to 3.3 V |
| 1 | 1 MΩ resistor | Battery-voltage divider, upper resistor |
| 1 | 330 kΩ resistor | Battery-voltage divider, lower resistor |
| 1 | 100 nF capacitor | ADC filtering near the ESP input |
| 1 | Optional on/off switch | Useful for transport and servicing |
| 4 | M2.5 screws | Removable PoolGuard housing lid; length depends on final bosses |
| 2 | M3 screws, washers and nuts | A02 mounting; confirm against the final printed lid |
| 1 | Small screw for battery holder | Match the centre hole in the actual holder |
| — | Heat-shrink tubing / wire | Moisture-resistant wiring and strain relief |
| — | Neutral-curing silicone | Suitable for permanent damp conditions and compatible with the skimmer plastic |
| — | PETG or ASA filament | Preferred over untreated PLA for the final outdoor part |

## Not yet final

- MOSFET/load-switch type and gate circuit
- exact GPIO assignment
- charger arrangement and charging connector
- final screw lengths
- gasket or sealing method between body and removable lid
- final battery-voltage calibration

Do not purchase large quantities from this list before the first complete prototype has been validated.
