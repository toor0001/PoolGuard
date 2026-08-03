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
- ungewöhnlich starke Badeaktivität erkennen, die auf eine Person im Pool hindeuten kann.

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

## Messzyklus und Stromsparbetrieb

Im Normalbetrieb wacht PoolGuard ungefähr einmal pro Minute auf und macht lokal einen kurzen Messburst mit dem A02YYUW. WLAN bleibt bei diesen Prüfungen ausgeschaltet. Erst wenn sich der erkannte Pumpenstatus, die Badeaktivität oder der Niedrigwasserstatus ändert, verbindet sich PoolGuard sofort mit WLAN und meldet den neuen Zustand an Home Assistant. Wasserstand, Wassertemperatur und Batteriestand werden zusätzlich ungefähr alle 30 Minuten übertragen.

Die zuletzt gemeldeten Zustände werden während Deep Sleep im RTC-Speicher des ESP32 gehalten. Dadurch sind keine Flash-Schreibvorgänge im Minutentakt nötig. Falls eine WLAN-/API-Übertragung fehlschlägt, bleibt die Zustandsänderung offen und wird beim nächsten Aufwachen erneut versucht.

## Geführte Bewegungs-Kalibrierung

Die Erkennung von Pumpe und Badeaktivität hängt stark vom konkreten Skimmer, Pumpendurchfluss, Wasserstand und der Poolgeometrie ab. Deshalb enthält PoolGuard einen geführten Kalibriermodus, statt sich ausschließlich auf feste Grenzwerte zu verlassen.

Für die Erstinbetriebnahme wird in `esphome/poolguard.yaml` vorübergehend `calibration_mode_on_boot: "true"` gesetzt und der XIAO neu geflasht. PoolGuard bleibt dann wach und mit Home Assistant verbunden. Danach werden die drei Kalibrier-Buttons in dieser Reihenfolge ausgeführt:

1. **Calibrate Quiet Water** – Pumpe aus, niemand im Pool.
2. **Calibrate Pump** – Umwälzpumpe an, niemand im Pool.
3. **Calibrate Person** – normale Bade-/Schwimmbewegung im Pool.

Jede Phase misst ungefähr 60 Sekunden. Intern werden zwölf Zeitfenster zu jeweils fünf Sekunden ausgewertet. PoolGuard speichert den **Median** der gemessenen Wasserbewegung, damit ein einzelner ungewöhnlicher Spritzer die Kalibrierung nicht verfälscht. Wenn die drei Profile eindeutig in der Reihenfolge `ruhig < Pumpe < Person` liegen, setzt PoolGuard automatisch den Pumpen-Grenzwert zwischen Ruhe und Pumpe und den Personen-/Aktivitäts-Grenzwert zwischen Pumpe und Person.

Die gelernten Werte werden dauerhaft gespeichert und bleiben auch nach einem normalen Neustart oder Stromverlust erhalten. Überlappen sich die drei Profile oder liegen sie nicht in der erwarteten Reihenfolge, bleiben die bisherigen beziehungsweise die Fallback-Grenzwerte aktiv und die Kalibrierung sollte wiederholt werden. In Home Assistant werden die drei gelernten Bewegungsprofile, die aktuell verwendeten Grenzwerte und der Kalibrierstatus als Diagnosewerte angezeigt.

Nach Abschluss wird `calibration_mode_on_boot` wieder auf `false` gesetzt und PoolGuard noch einmal geflasht. Danach läuft wieder der normale stromsparende Minutentakt.

> **Wichtig:** PoolGuard erkennt keine Person direkt. Er klassifiziert die Bewegung der Wasseroberfläche. Pumpen- und Badeaktivitätserkennung sind deshalb experimentelle Hinweise und dürfen nicht als Sicherheitssystem oder Ersatz für Poolaufsicht verwendet werden.

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
6. Den realen Mindestwasserstand bestimmen, bei dem der Skimmer die Pumpe noch zuverlässig versorgt, und `min_safe_water_level_percent` entsprechend anpassen.
7. Die geführte Kalibrierung für ruhiges Wasser, Pumpe und Badeaktivität durchführen.
8. `calibration_mode_on_boot` wieder auf `false` setzen und die stromsparende Normalversion flashen.

## Sicherheit

- Nur eine geschützte, qualitativ gute 18650-Zelle verwenden.
- Akku nicht kurzschließen, quetschen, verpolen oder unbeaufsichtigt laden.
- Elektronik vor Kondenswasser und Spritzwasser schützen.
- Den A02YYUW über den Pololu 2810 im Deep Sleep vollständig abschalten.
- Pumpen- und Personen-/Aktivitätserkennung nur als Hinweis verwenden, niemals als alleinige sicherheitsrelevante Abschaltung oder Überwachung.

## Unterstützung

<a href="https://paypal.me/toor0001"><img src="assets/paypal-support-de.svg" alt="Spendiere mir einen Kaffee via PayPal" width="430"></a>

## Lizenz

Software und Dokumentation stehen unter der MIT-Lizenz. Die CAD-Dateien sind aktuell noch Prototypen; vor dem ersten stabilen Release kann dafür eine eigene Hardware-Lizenz ergänzt werden.
