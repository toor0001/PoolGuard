# PoolGuard

**Batteriebetriebener Pool-Skimmer-Sensor für Home Assistant und ESPHome**

Deutsch · [English](README.md)

PoolGuard ist ein DIY-Sensormodul, das passgenau und ohne Bohren in den Deckel eines **AstralPool-Skimmers mit 17,5 l** eingeklebt werden kann. Das Modul misst den Abstand zur Wasseroberfläche und berechnet daraus die aktuelle Wassertiefe und den Wasserstand. Zusätzlich erfasst es die Wassertemperatur. Anhand der Bewegung beziehungsweise Unruhe der Wasseroberfläche kann außerdem erkannt werden, ob die Umwälzpumpe gerade läuft. Deutlich stärkere und unregelmäßige Oberflächenbewegungen können darauf hindeuten, dass sich gerade jemand im Pool befindet. Durch Deep Sleep und das vollständige Abschalten des A02YYUW zwischen den Messungen soll PoolGuard mit einer Akkuladung möglichst über die gesamte Poolsaison laufen.

## Möglichkeiten in Home Assistant

Mit den Messwerten von PoolGuard lassen sich unter anderem folgende Automationen umsetzen:

- eine Pool-Wärmepumpe abhängig von der Wassertemperatur ein- oder ausschalten;
- die Umwälzpumpe bei zu niedrigem Wasserstand deaktivieren;
- bei Unterschreiten der sicheren Mindestwassertiefe eine Warnung senden;
- eine UV-C-Lampe nur bei tatsächlich erkannter Wasserumwälzung einschalten;
- ungewöhnlich starke Badeaktivität erkennen, die auf eine Person im Pool hindeuten kann;
- aktuelle Wassertiefe, Wasserstand in Prozent und geschätztes Poolvolumen anzeigen.

> **Projektstatus:** Prototyp / in Entwicklung. Den Originaldeckel noch nicht bohren, schneiden oder dauerhaft verkleben, bevor die aktuelle Version geprüft wurde.

> **Name und Zugehörigkeit:** PoolGuard ist ein unabhängiges Open-Source-DIY-Projekt. Es besteht keine Verbindung zu Unternehmen, Marken oder kommerziellen Produkten, die ebenfalls den Namen PoolGuard verwenden.

## Bauteile

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

> **Affiliate-Hinweis:** Einige Produktlinks können Affiliate-Links sein. Wenn du darüber etwas kaufst, erhalte ich möglicherweise eine kleine Provision. Für dich ändert sich der Preis dadurch nicht.

## A02YYUW-Stromversorgung

Für den A02YYUW wird der **Pololu Mini MOSFET Slide Switch with Reverse Voltage Protection, LV (#2810)** verwendet.

```text
XIAO 3.3 V  -------- VIN   Pololu 2810
XIAO GND    -------- GND   Pololu 2810
D2 / GPIO4  -------- ON    Pololu 2810
Pololu VOUT -------- VCC   A02YYUW
A02YYUW GND -------- GND
A02YYUW TX  -------- D7 / GPIO20 (UART RX)
A02YYUW RX  -------- nicht anschließen
```

- **D2 / GPIO4 HIGH:** A02YYUW eingeschaltet
- **D2 / GPIO4 LOW oder hochohmig:** A02YYUW ausgeschaltet
- im Deep Sleep bleibt der A02YYUW stromlos

Der mechanische Schiebeschalter des Pololu muss auf „off“ stehen, damit ESPHome
den ON-Pin steuern kann. Die Datenleitung des DS18B20 liegt an D3/GPIO5 und
benötigt einen 4,7-kΩ-Pull-up nach 3,3 V. Der Ausgang des Spannungsteilers liegt
am nicht als Strapping-Pin verwendeten ADC1-Anschluss D1/GPIO3.

## Messzyklus und Stromsparbetrieb

Im Normalbetrieb wacht PoolGuard ungefähr einmal pro Minute auf und macht lokal einen kurzen Messburst mit dem A02YYUW. WLAN bleibt dabei ausgeschaltet. Erst wenn sich Pumpenstatus, Badeaktivität oder Niedrigwasserstatus ändern, verbindet sich PoolGuard sofort und meldet den neuen Zustand. Wassertiefe, Wasserstand in Prozent, geschätztes Poolvolumen, Wassertemperatur und Batteriestand werden zusätzlich ungefähr alle 30 Minuten übertragen.

Die zuletzt gemeldeten Zustände werden während Deep Sleep im RTC-Speicher gehalten. Schlägt eine WLAN-/API-Übertragung fehl, bleibt die Zustandsänderung offen und wird beim nächsten Aufwachen erneut versucht.

## Wasserstands-Referenzkalibrierung

Für den Wasserstand sind keine getrennten „leer“-/„voll“-Referenzpunkte mehr nötig. **Ein einziger bekannter realer Wasserstand reicht.**

Für die Erstinbetriebnahme wird in `esphome/poolguard.yaml` vorübergehend `calibration_mode_on_boot: "true"` gesetzt und der XIAO neu geflasht. Danach:

1. Die **aktuelle tatsächliche Wassertiefe** im Pool in Zentimetern messen.
2. Diesen Wert in Home Assistant bei **Reference Water Depth** eintragen.
3. Während der Wasserstand unverändert ist **Set Water Level Reference** drücken.
4. PoolGuard speichert gleichzeitig den aktuellen A02-Abstand und die bekannte reale Wassertiefe.
5. Anschließend bei **Minimum Safe Water Depth** die niedrigste reale Wassertiefe eintragen, bei der der Skimmer die Umwälzpumpe noch zuverlässig mit Wasser versorgt.

Ab diesem Zeitpunkt wird die Wassertiefe aus der Änderung des A02-Abstands berechnet. Der Abstand vom Sensor bis zum Beckenboden muss nicht separat gemessen werden.

In der YAML ist standardmäßig ein runder Pool mit **5,0 m Durchmesser** und **120 cm maximaler Wassertiefe** hinterlegt. `Water Level` wird daraus als Prozentwert berechnet. `Pool Volume` nutzt die aktuelle Wassertiefe und berechnet das Volumen als Zylinder. Bei 5,0 m Durchmesser und 120 cm Tiefe ergeben sich theoretisch etwa **23,56 m³**. Für einen anderen Pool werden `pool_diameter_m` und `pool_max_depth_cm` angepasst. Bei nicht zylindrischer Bodenform ist das Volumen entsprechend nur eine Näherung.

## Geführte Bewegungs-Kalibrierung

Die Erkennung von Pumpe und Badeaktivität hängt stark vom konkreten Skimmer, Pumpendurchfluss, Wasserstand und der Poolgeometrie ab. Deshalb enthält PoolGuard einen geführten Kalibriermodus.

Im Kalibriermodus werden diese drei Buttons nacheinander ausgeführt:

1. **Calibrate Quiet Water** – Pumpe aus, niemand im Pool.
2. **Calibrate Pump** – Umwälzpumpe an, niemand im Pool.
3. **Calibrate Person** – normale Bade-/Schwimmbewegung im Pool.

Jede Phase misst ungefähr 60 Sekunden und speichert den Median der Wasserbewegung. Die Bewegung wird aus einer getrimmten Messwert-Spanne berechnet, damit einzelne Ausreißer oder Spritzer weniger Einfluss haben als bei einem einfachen Min/Max-Wert. Liegen die Profile eindeutig in der Reihenfolge `ruhig < Pumpe < Person`, berechnet PoolGuard automatisch die Grenzwerte. Überlappen sich die Profile, bleiben die bisherigen beziehungsweise Fallback-Grenzwerte aktiv.

Nach Wasserstands- und Bewegungs-Kalibrierung wird `calibration_mode_on_boot` wieder auf `false` gesetzt und PoolGuard erneut geflasht. Danach läuft wieder der stromsparende Normalbetrieb.

> **Wichtig:** PoolGuard erkennt keine Person direkt. Er klassifiziert die Bewegung der Wasseroberfläche. Pumpen- und Badeaktivitätserkennung sind deshalb experimentelle Hinweise und dürfen nicht als Sicherheitssystem oder Ersatz für Poolaufsicht verwendet werden.

## Mechanisches Konzept

Der Body sitzt im Skimmer und wird seitlich mit geeignetem neutralvernetzendem Silikon an den Rippen fixiert. Der originale Mittelsteg bleibt erhalten. Der Body ist absichtlich **leicht nach innen zum Skimmer hin abgeschrägt**, damit Wasser, das auf das Gehäuse gelangt, nach innen in den Skimmer zurücklaufen kann und sich nicht auf dem Body sammelt. Der lösbare Deckel trägt die Technik:

- Batteriehalter und ESP auf der trockenen Innenseite;
- A02YYUW außen in Richtung Wasser;
- Durchführung für das DS18B20-Kabel;
- externe WLAN-Antenne möglichst nah am Kunststoffdeckel.

Die aktuellen Druckdateien liegen im Ordner [`3D-Files/`](3D-Files/). Ich drucke mit einem Bambu Lab A1 Mini.

<p align="center">
  <img src="images/PG.jpg" alt="Body" width="90%">
</p>

## Schnellstart

1. Aktuelle Dateien aus `3D-Files/` herunterladen und drucken.
2. `esphome/secrets.example.yaml` als Vorlage für die eigenen ESPHome-Secrets verwenden.
3. Pins, Poolgeometrie und Kalibrierwerte in `esphome/poolguard.yaml` prüfen.
4. XIAO ESP32-C3 zunächst per USB flashen.
5. `calibration_mode_on_boot: "true"` setzen und die Kalibrier-Version flashen.
6. Reale Wassertiefe messen, bei **Reference Water Depth** eintragen und **Set Water Level Reference** drücken.
7. **Minimum Safe Water Depth** für den realen Skimmer/Pumpenbetrieb festlegen.
8. Kalibrierung für ruhiges Wasser, Pumpe und Badeaktivität durchführen.
9. `calibration_mode_on_boot` wieder auf `false` setzen und die stromsparende Normalversion flashen.

## Sicherheit

- Nur eine geschützte, qualitativ gute 18650-Zelle verwenden.
- Akku nicht kurzschließen, quetschen, verpolen oder unbeaufsichtigt laden.
- Elektronik vor Kondenswasser und Spritzwasser schützen.
- Den A02YYUW über den Pololu 2810 im Deep Sleep vollständig abschalten.
- Pumpen- und Personen-/Aktivitätserkennung nur als Hinweis verwenden, niemals als alleinige sicherheitsrelevante Abschaltung oder Überwachung.

## Unterstützung

<a href="https://paypal.me/toor0001/5"><img src="assets/paypal-support-en.svg" alt="Buy me a coffee via PayPal" width="430"></a>

## Lizenz

Software und Dokumentation stehen unter der MIT-Lizenz. Die CAD-Dateien sind aktuell noch Prototypen; vor dem ersten stabilen Release kann dafür eine eigene Hardware-Lizenz ergänzt werden.

## Hinweis zur Entwicklung

Dieses Projekt ist im Rahmen eines kollaborativen **Vibe-Coding-Workflows** mit **ChatGPT** und **OpenAI Codex** entstanden. Beide Tools wurden für Code-Erstellung, Reviews, Fehlersuche und Dokumentation eingesetzt.

Hardwareaufbau, Integrationsentscheidungen, praktische Tests und die abschließende Verantwortung für das Projekt liegen beim Projektbetreiber.
