# PoolGuard CAD

The mechanical design is currently in the fit-validation stage.

## Current part

`source/PoolGuard_Passschablone_V1.scad` is a 5 mm high open frame representing skimmer compartments 3 and 4 as two of twelve equal sectors. It is intended only to check the contour before the complete housing body is designed.

Current design inputs:

- skimmer-lid outside diameter: 242 mm
- measured radial compartment depth: 78.5 mm
- two compartments: nominally 60 degrees
- radial fit clearance: 0.5 mm
- angular clearance: 0.6 degrees per side
- template wall: 2 mm
- template height: 5 mm

## Test procedure

1. Render/export the SCAD file to STL or use the corresponding STL supplied during development.
2. Print without modifying the scale.
3. Check the fit in compartments 3 and 4.
4. Report separately:
   - outer-radius fit;
   - inner-radius fit;
   - left and right straight-edge fit;
   - centring over the original middle rib.

## Planned final parts

After the template is confirmed:

- `PoolGuard_Body`: closed enclosure tub, approximately 30 mm internal height;
- `PoolGuard_Lid`: removable electronics carrier;
- A02YYUW mounted on the water-facing lid side;
- single 18650 holder and XIAO ESP32-C3 mounted on the internal side;
- raised or recessed `PoolGuard` lettering.

Do not publish the prototype template as a final printable enclosure.
