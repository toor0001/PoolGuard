# PoolGuard – Moisture, condensation and silica gel

PoolGuard is installed in the skimmer only a few centimetres above the water surface. The electronics are therefore exposed to persistently high relative humidity and, especially with warm pool water and cooler nights, an increased risk of condensation.

## Basic approach

The electronics enclosure should keep direct splashes and droplets away from the electronics. Fully potting the complete assembly in epoxy is not part of the normal build: it would unnecessarily complicate repairs, battery replacement, USB access and later modifications. Completely covering the enclosure in silicone is likewise not required.

Pay particular attention to openings, cable entries, screw locations and the existing louvre slots. Direct water ingress is more critical than ordinary humid air. Condensation can nevertheless occur when humid air inside the enclosure cools below its dew point.

## Silica gel as an additional moisture buffer

A small **silica-gel desiccant sachet** inside the electronics enclosure is useful as an additional moisture buffer. For the small PoolGuard enclosure, one or two small sachets totalling roughly 5–10 g are a practical starting point if they can be secured without stressing components or electrical contacts.

Silica gel does not prevent direct water ingress and is not a substitute for sensible sealing. It does, however, adsorb water vapour from the enclosed air and can therefore reduce the risk of condensation.

### Does silica gel get used up?

Silica gel is not simply chemically consumed, but its pores gradually fill with water. Once saturated, it can absorb very little additional moisture and therefore loses its protective effect. How quickly this happens depends on how much humid outside air enters the enclosure and on temperature/humidity cycling.

In a non-hermetic enclosure, silica gel should therefore be treated as a **maintenance item**. Regenerable sachets are preferable, ideally with a humidity indicator. When PoolGuard is opened for seasonal maintenance, inspect the sachet and dry/regenerate it according to the manufacturer's instructions or replace it if necessary.

> Do not install loose silica-gel granules where they could reach the PCB, connectors or battery. A closed, vapour-permeable desiccant sachet is preferable.

## Do not use table salt

Ordinary table salt is not a suitable replacement for silica gel. At high humidity salt can take up water and may form a conductive salt solution. Such a solution near the PCB, ESP32, battery or contacts significantly increases corrosion and short-circuit risk.

## Practical recommendation

- Protect the electronics from direct splashes and droplets.
- Seal cable entries and obvious ingress paths cleanly.
- Avoid full epoxy potting while serviceability is desired.
- Add a small closed silica-gel sachet as an additional humidity buffer.
- Prefer regenerable silica gel with a humidity indicator.
- Inspect the desiccant during seasonal maintenance and regenerate or replace it when saturated.
- Do not place loose table salt inside the electronics enclosure.

Silica gel is therefore an additional protective measure, not PoolGuard's primary enclosure seal.