# PoolGuard

**Batteriebetriebener Pool-Skimmer-Sensor für Home Assistant und ESPHome**

Deutsch · [English](README.md)

PoolGuard ist ein DIY-Sensormodul, das passgenau und ohne Bohren in den Deckel eines **AstralPool-Skimmers mit 17,5 l** eingeklebt werden kann. Das Modul misst den Abstand zur Wasseroberfläche und damit den aktuellen Füllstand des Pools. Zusätzlich erfasst es die Wassertemperatur. Anhand der Bewegung beziehungsweise Unruhe der Wasseroberfläche kann außerdem erkannt werden, ob die Umwälzpumpe gerade läuft. Deutlich stärkere und unregelmäßige Oberflächenbewegungen können zusätzlich darauf hindeuten, dass sich gerade jemand im Pool befindet. Die Elektronik ist konsequent auf einen sehr niedrigen Energieverbrauch ausgelegt. Durch Deep Sleep und das vollständige Abschalten der Sensorik zwischen den Messungen soll PoolGuard mit einer Akkuladung über die gesamte Poolsaison betrieben werden können.

## Möglichkeiten in Home Assistant

Mit den Messwerten von PoolGuard lassen sich unter anderem folgende Automationen umsetzen:

- eine Pool-Wärmepumpe abhängig von der gemessenen Wassertemperatur ein- oder ausschalten;
- die Umwälzpumpe bei zu niedrigem Füllstand deaktivieren, damit sie nicht trockenläuft;
- bei zu hohem oder zu niedrigem Wasserstand eine Warnung senden;
- eine UV-C-Lampe ausschließlich dann einschalten, wenn tatsächlich eine Wasserumwälzung erkannt wird;
- Personenerkennung im Pool

> **Projektstatus:** Prototyp / in Entwicklung. Aktuell wird zuerst die Passschablone für die Fächer 3 und 4 getestet. Den Originaldeckel noch nicht bohren, schneiden oder dauerhaft verkleben, bevor die aktuelle CAD-Version geprüft wurde.

> **Name und Zugehörigkeit:** PoolGuard ist ein unabhängiges Open-Source-DIY-Projekt. Es besteht keine Verbindung zu Unternehmen, Marken oder kommerziellen Produkten, die ebenfalls den Namen PoolGuard verwenden.

## Bauteile

Die folgende Liste wird nach und nach um Bezugsquellen ergänzt:

| Bauteil | Verwendung | Link |
|---|---|---|
| Seeed Studio XIAO ESP32-C3 | Mikrocontroller / ESPHome | [Amazon](https://link.amazon/B0iQQg2zF) |
| USB-C Breakout-Kabel | Stromversorgung / Anschluss | [Amazon](https://link.amazon/B0eO8jr4N) |
| DFRobot A02YYUW | Abstandsmessung zur Wasseroberfläche | [Amazon](https://link.amazon/B0dWRfbC4) |
| Wasserdichter DS18B20 | Wassertemperatur | [Amazon](https://link.amazon/B08FcJbtj) |
| Geschützte 18650 Li-Ion-Zelle | Stromversorgung | – |
| 18650-Batteriehalter | Aufnahme des Akkus | [Amazon](https://link.amazon/B0hraa6X7) |
| MOSFET / Load-Switch | Abschalten des A02YYUW im Deep Sleep | – |
| Externe 2,4-GHz-WLAN-Antenne | Verbesserung der WLAN-Verbindung | – |

> **Affiliate-Hinweis:** Einige der hier verlinkten Produktlinks können Affiliate-Links sein. Wenn du über einen solchen Link etwas kaufst, erhalte ich möglicherweise eine kleine Provision. Für dich ändert sich der Preis dadurch nicht. Damit unterstützt du die Weiterentwicklung meiner DIY-Projekte.

## Mechanisches Konzept

Der Body sitzt im Skimmer und wird später seitlich mit geeignetem neutralvernetzendem Silikon an den Rippen fixiert. Der originale Mittelsteg bleibt vollständig erhalten. Der lösbare Deckel trägt die Technik:

- Batteriehalter und ESP auf der trockenen Innenseite;
- A02YYUW außen in Richtung Wasser;
- Durchführung für das DS18B20-Kabel;
- externe WLAN-Antenne möglichst nah am Kunststoffdeckel.

## Schnellstart

1. Passschablone aus `cad/stl/` drucken und die Bohrungen manuell vornehmen.
2. `esphome/secrets.example.yaml` als Vorlage für die eigenen ESPHome-Secrets verwenden.
3. Pins und Kalibrierwerte in `esphome/poolguard.yaml` prüfen.
4. XIAO ESP32-C3 zunächst per USB flashen.
5. `distance_empty_cm` und `distance_full_cm` im eingebauten Zustand kalibrieren.
6. Vor Aktivierung der Pumpen- oder Personenerkennung mehrere Messzyklen mit Pumpe an und aus sowie mit und ohne Badebetrieb vergleichen.

## Sicherheit

- Nur eine geschützte, qualitativ gute 18650-Zelle verwenden.
- Akku nicht kurzschließen, quetschen, verpolen oder unbeaufsichtigt laden.
- Elektronik vor Kondenswasser und Spritzwasser schützen.
- Den A02YYUW über MOSFET oder Load-Switch im Deep Sleep vollständig abschalten.
- Pumpen- und Personenerkennung nur als Hinweis verwenden, niemals als alleinige sicherheitsrelevante Abschaltung oder Überwachung.

## Unterstützung

<a href="https://buymeacoffee.com/toor0001"><img src="https://img.buymeacoffee.com/button-api/?text=Spendiere%20mir%20einen%20Kaffee&emoji=%E2%98%95&slug=toor0001&button_colour=FFDD00&font_colour=000000&font_family=Lato&outline_colour=000000&coffee_colour=ffffff" alt="Spendiere mir einen Kaffee"></a>

## Lizenz

Software und Dokumentation stehen unter der MIT-Lizenz. Die CAD-Dateien sind aktuell noch Prototypen; vor dem ersten stabilen Release kann dafür eine eigene Hardware-Lizenz ergänzt werden.
