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

> **Projektstatus:** Prototyp / in Entwicklung. Den Originaldeckel noch nicht bohren, schneiden oder dauerhaft verkleben, bevor die aktuelle Version geprüft wurde.

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
| **Pololu Mini MOSFET Slide Switch LV #2810** | A02YYUW im Deep Sleep vollständig abschalten | [Pololu](https://www.pololu.com/product/2810) |
| Externe 2,4-GHz-WLAN-Antenne | Verbesserung der WLAN-Verbindung | – |

> **Affiliate-Hinweis:** Einige der hier verlinkten Produktlinks können Affiliate-Links sein. Wenn du über einen solchen Link etwas kaufst, erhalte ich möglicherweise eine kleine Provision. Für dich ändert sich der Preis dadurch nicht. Damit unterstützt du die Weiterentwicklung meiner DIY-Projekte.

## A02YYUW-Stromversorgung

Für den A02YYUW wird der fertige **Pololu Mini MOSFET Slide Switch with Reverse Voltage Protection, LV (#2810)** verwendet. Das Modul arbeitet bereits ab 2 V und eignet sich deshalb für die 3,3-V-Versorgung des XIAO ESP32-C3.

Der mechanische Schiebeschalter des Pololu-Moduls bleibt für den automatischen Betrieb in Stellung **OFF**. Dann übernimmt der ESP32 die Steuerung über den `ON`-Eingang.

```text
XIAO 3.3 V  -------- VIN   Pololu 2810
XIAO GND    -------- GND   Pololu 2810
GPIO2       -------- ON    Pololu 2810
Pololu VOUT -------- VCC   A02YYUW
A02YYUW GND -------- GND
A02YYUW TX  -------- GPIO20
A02YYUW RX  -------- nicht anschließen
```

Damit gilt:

- **GPIO2 HIGH:** A02YYUW eingeschaltet
- **GPIO2 LOW:** A02YYUW ausgeschaltet
- im Deep Sleep bleibt der A02YYUW stromlos

## Mechanisches Konzept

Der Body sitzt im Skimmer und wird später seitlich mit geeignetem neutralvernetzendem Silikon an den Rippen fixiert. Der originale Mittelsteg bleibt vollständig erhalten. Der lösbare Deckel trägt die Technik:

- Batteriehalter und ESP auf der trockenen Innenseite;
- A02YYUW außen in Richtung Wasser;
- Durchführung für das DS18B20-Kabel;
- externe WLAN-Antenne möglichst nah am Kunststoffdeckel.

Die aktuellen Druckdateien liegen im Ordner [`3D-Files/`](3D-Files/).

## Schnellstart

1. Aktuelle Dateien aus `3D-Files/` herunterladen und drucken.
2. `esphome/secrets.example.yaml` als Vorlage für die eigenen ESPHome-Secrets verwenden.
3. Pins und Kalibrierwerte in `esphome/poolguard.yaml` prüfen.
4. XIAO ESP32-C3 zunächst per USB flashen.
5. `distance_empty_cm` und `distance_full_cm` im eingebauten Zustand kalibrieren.
6. Vor Aktivierung der Pumpen- oder Personenerkennung mehrere Messzyklen mit Pumpe an und aus sowie mit und ohne Badebetrieb vergleichen.

## Sicherheit

- Nur eine geschützte, qualitativ gute 18650-Zelle verwenden.
- Akku nicht kurzschließen, quetschen, verpolen oder unbeaufsichtigt laden.
- Elektronik vor Kondenswasser und Spritzwasser schützen.
- Den A02YYUW über den Pololu 2810 im Deep Sleep vollständig abschalten.
- Pumpen- und Personenerkennung nur als Hinweis verwenden, niemals als alleinige sicherheitsrelevante Abschaltung oder Überwachung.

## Unterstützung

<a href="https://paypal.me/toor0001"><img src="assets/paypal-support-de.svg" alt="Spendiere mir einen Kaffee via PayPal" width="430"></a>

## Lizenz

Software und Dokumentation stehen unter der MIT-Lizenz. Die CAD-Dateien sind aktuell noch Prototypen; vor dem ersten stabilen Release kann dafür eine eigene Hardware-Lizenz ergänzt werden.
