# PoolGuard

**Batteriebetriebener Pool-Skimmer-Sensor für Home Assistant und ESPHome**

Deutsch · [English](README.md)

PoolGuard ist ein DIY-Sensormodul für einen **AstralPool-Skimmer mit 17,5 l**. Es misst Wasserstand und Wassertemperatur und wertet zusätzlich die Bewegung der Wasseroberfläche aus. Daraus kann PoolGuard abschätzen, ob die Umwälzpumpe läuft und ob ungewöhnlich starke Wasserbewegung auf Badeaktivität hindeutet.

Der Schwerpunkt liegt auf sehr niedrigem Energieverbrauch: Ein XIAO ESP32-C3 wacht im Normalbetrieb nur kurz auf, der A02YYUW wird über einen Pololu-MOSFET vollständig ein- und ausgeschaltet und WLAN wird nur bei Bedarf verwendet.

> **Projektstatus:** Prototyp / in Entwicklung. Pumpen- und Personenerkennung müssen am realen Pool getestet und kalibriert werden.

> **Wichtig:** PoolGuard erkennt keine Person direkt. Es klassifiziert ausschließlich Wasserbewegung und ist kein Sicherheitssystem oder Ersatz für Poolaufsicht.

## Funktionen

- Abstand zur Wasseroberfläche mit A02YYUW
- reale Wassertiefe aus einer einzigen Wasserstandsreferenz
- Wasserstand in Prozent
- geschätztes Poolvolumen
- Wassertemperatur mit DS18B20
- Niedrigwasserwarnung mit Hysterese
- Erkennung der Umwälzpumpe anhand der Wasserbewegung
- experimentelle Erkennung von Bade-/Personenaktivität
- geführte Bewegungs-Kalibrierung für ruhiges Wasser, Pumpe und Badebetrieb
- persistente Kalibrierdaten
- automatischer Initial-Setup-Modus bei Erstinbetriebnahme
- Wartungsmodus über Home Assistant
- ereignisgesteuerte WLAN-/API-Verbindung
- Deep Sleep und vollständiges Abschalten des A02YYUW
- OTA-Updates im Wach-/Wartungsbetrieb

## Möglichkeiten in Home Assistant

Die Werte können z. B. verwendet werden, um eine Pool-Wärmepumpe abhängig von der Wassertemperatur zu steuern, die Umwälzpumpe bei zu niedrigem Wasserstand abzuschalten, eine Niedrigwasserwarnung zu senden, eine UV-C-Lampe nur bei erkannter Umwälzung zu betreiben oder Wasserstand und Poolvolumen im Dashboard darzustellen.

## Hardware

| Bauteil | Verwendung | Link |
|---|---|---|
| Seeed Studio XIAO ESP32-C3 | Mikrocontroller / ESPHome | [Amazon](https://link.amazon/B0iQQg2zF) |
| USB-C Breakout-Kabel | Stromversorgung / Anschluss | [Amazon](https://link.amazon/B0eO8jr4N) |
| DFRobot A02YYUW | Abstand und Wasserbewegung | [Amazon](https://link.amazon/B0dWRfbC4) |
| Wasserdichter DS18B20 | Wassertemperatur | [Amazon](https://link.amazon/B08FcJbtj) |
| Geschützte 18650 Li-Ion-Zelle | Stromversorgung | – |
| 18650-Batteriehalter | Aufnahme des Akkus | [Amazon](https://link.amazon/B0hraa6X7) |
| **Pololu Mini MOSFET Slide Switch LV #2810** | A02YYUW vollständig abschalten | [Pololu](https://www.pololu.com/product/2810) |
| Externe 2,4-GHz-WLAN-Antenne | bessere WLAN-Reichweite | – |

> **Affiliate-Hinweis:** Einige Produktlinks können Affiliate-Links sein. Wenn du darüber etwas kaufst, erhalte ich möglicherweise eine kleine Provision. Für dich ändert sich der Preis dadurch nicht.

## Pinbelegung

Die aktuelle Firmware verwendet bewusst keine ESP32-C3-Strapping-Pins GPIO2, GPIO8 oder GPIO9.

| Funktion | XIAO ESP32-C3 |
|---|---|
| Pololu ON | D2 / GPIO4 |
| A02YYUW TX → ESP RX | D7 / GPIO20 |
| DS18B20 DATA | D3 / GPIO5 |
| GPIO3 | frei |

Der DS18B20 benötigt einen **4,7-kΩ-Pull-up zwischen DATA und 3,3 V**.

### A02YYUW über Pololu 2810

```text
XIAO 3.3 V  -------- VIN   Pololu 2810
XIAO GND    -------- GND   Pololu 2810
GPIO4 / D2  -------- ON    Pololu 2810
Pololu VOUT -------- VCC   A02YYUW
A02YYUW GND -------- GND
A02YYUW TX  -------- GPIO20 / D7
A02YYUW RX  -------- nicht anschließen
```

Pololu `ON` ist active-high. Im Deep Sleep bleibt der GPIO nicht aktiv und der A02YYUW ist stromlos.

## Normalbetrieb und Stromsparen

Standardmäßig sind konfiguriert:

```yaml
sleep_duration: 55s
detection_burst_duration: 5s
status_every_wakes: "30"
```

Nach abgeschlossenem Setup wacht PoolGuard ungefähr einmal pro Minute auf. Der A02YYUW wird eingeschaltet, sammelt während des Messbursts Messwerte und wird anschließend sofort wieder abgeschaltet. Für die normalen lokalen Messungen ist keine dauerhafte WLAN-Verbindung nötig.

PoolGuard verbindet sich mit WLAN/Home Assistant, wenn sich ein relevanter Zustand ändert – Pumpe, Badeaktivität oder Niedrigwasser – oder wenn der regelmäßige Statusbericht fällig ist. Der Statusbericht erfolgt standardmäßig nach ungefähr 30 Wake-Zyklen. Dabei wird auch die Wassertemperatur aktualisiert.

Die letzten Zustände und Zähler werden im RTC-Speicher über Deep Sleep hinweg gehalten. Schlägt eine WLAN-/API-Verbindung fehl, bleibt eine Zustandsänderung offen und wird bei einem späteren Wake erneut übertragen.

### Messburst und Akkulaufzeit

`detection_burst_duration` ist der wichtigste Parameter für den Kompromiss zwischen Erkennungsqualität und Akkulaufzeit:

```yaml
detection_burst_duration: 5s
```

Ein längerer Burst liefert mehr A02-Messwerte und kann die Bewegungsanalyse robuster machen, hält ESP und Sensor aber länger aktiv. **5 Sekunden sind ein konservativer Ausgangswert für Inbetriebnahme und Tests.** Nach realen Tests am Pool können z. B. **2 s oder 1,5 s** ausprobiert werden. Das kann die Akkulaufzeit erheblich verbessern.

Wichtig: Die Firmware benötigt für einen gültigen normalen Zyklus mindestens fünf gültige A02-Frames. Deshalb sollte ein kürzerer Burst erst nach praktischer Prüfung verwendet werden.

Die geführte Bewegungs-Kalibrierung ist davon getrennt und verwendet weiterhin eigene 5-Sekunden-Fenster.

## Robuste A02-Auswertung

Der A02YYUW sendet UART-Frames mit Prüfsumme. PoolGuard akzeptiert nur gültige Frames und plausible Entfernungen von 30 bis 4500 mm. Für einen normalen Zyklus werden mindestens fünf gültige Frames benötigt.

Für die Entfernung wird der **Median** der Messwerte verwendet. Die Wasserbewegung wird nicht aus dem extremen Min/Max-Abstand berechnet, sondern aus dem Bereich zwischen ungefähr dem 10. und 90. Perzentil. Dadurch haben einzelne Ausreißer oder Spritzer weniger Einfluss.

## Wasserstandsberechnung

PoolGuard benötigt keine getrennte Leer-/Voll-Kalibrierung. Eine einzige bekannte reale Wassertiefe reicht.

Bei der Referenzkalibrierung speichert PoolGuard gleichzeitig:

- den aktuellen A02-Abstand zur Wasseroberfläche;
- die tatsächlich gemessene Wassertiefe.

Ändert sich später der Abstand zur Wasseroberfläche, wird daraus direkt die neue Wassertiefe berechnet.

Die Standardgeometrie ist:

```yaml
pool_diameter_m: "5.0"
pool_max_depth_cm: "120.0"
```

`Water Level` ist die aktuelle Tiefe relativ zur konfigurierten Maximaltiefe. `Pool Volume` wird für einen zylindrischen Pool als `π × Radius² × Wassertiefe` geschätzt. Bei 5,0 m Durchmesser und 120 cm Tiefe sind das etwa **23,56 m³**.

Bei Pools mit anderer Geometrie müssen die Werte angepasst werden; bei nicht zylindrischer Bodenform ist das Volumen nur eine Näherung.

## Niedrigwasser-Erkennung

`Minimum Safe Water Depth` ist in Home Assistant ein konfigurierbarer Wert und muss am realen Skimmer bestimmt werden. Standardmäßig ist zusätzlich eine Hysterese von 1 cm vorgesehen:

```yaml
low_water_hysteresis_cm: "1.0"
```

Wird die Mindesttiefe unterschritten, wird `Low Water Level` aktiv. Zurückgesetzt wird der Zustand erst, wenn der Wasserstand wieder mindestens Mindesttiefe + Hysterese erreicht. Das verhindert Flattern direkt am Grenzwert.

## Initial Setup Mode

Ein neues oder zurückgesetztes Gerät startet automatisch im **Initial Setup Mode**. Es gibt keinen Compile-Time-Schalter und keinen notwendigen zweiten Firmware-Flash mehr.

Der Zustand `initial_setup_completed` wird persistent im Flash gespeichert und übersteht Deep Sleep, Reset und vollständigen Stromverlust.

Solange das Setup nicht abgeschlossen ist:

- wird Deep Sleep verhindert;
- WLAN/API bleiben verfügbar;
- der A02YYUW bleibt im Leerlauf ausgeschaltet;
- Messungen erfolgen nur auf Anforderung oder während einer Kalibrierung;
- die Wassertemperatur wird im Wachbetrieb regelmäßig aktualisiert.

### Erstinbetriebnahme

1. PoolGuard einmal per USB flashen.
2. Das Gerät bleibt automatisch im Initial Setup Mode online.
3. Die tatsächliche aktuelle Wassertiefe in cm messen.
4. Den Wert bei **Reference Water Depth** eintragen.
5. **Set Water Level Reference** drücken.
6. **Minimum Safe Water Depth** für den eigenen Skimmer festlegen.
7. Optional die drei Bewegungsprofile kalibrieren.
8. **Finish Initial Setup** drücken.

Danach wird der Setup-Status gespeichert, WLAN abgeschaltet und PoolGuard wechselt in den normalen Deep-Sleep-Betrieb.

`Finish Initial Setup` wird verweigert, solange eine Bewegungs-Kalibrierung läuft oder noch keine gültige Wasserstandsreferenz gespeichert wurde. Eine vollständige Bewegungs-Kalibrierung ist dagegen optional; ohne sie bleiben Fallback-Grenzwerte aktiv.

### Initial Setup zurücksetzen

Der Button **Reset Initial Setup** ist gegen versehentliches Betätigen geschützt. Er muss zweimal innerhalb von 10 Sekunden gedrückt werden. Danach bleibt PoolGuard wieder wach und kann neu eingerichtet werden.

## Geführte Bewegungs-Kalibrierung

Für eine möglichst gute Unterscheidung zwischen ruhigem Wasser, laufender Pumpe und Badeaktivität gibt es drei Kalibrierungen:

1. **Calibrate Quiet Water** – Pumpe aus, niemand im Pool.
2. **Calibrate Pump** – Pumpe an, niemand im Pool.
3. **Calibrate Person** – typische Bade-/Schwimmbewegung.

Standardmäßig werden pro Profil **12 Fenster à 5 Sekunden**, also ungefähr 60 Sekunden, ausgewertet. Jedes Fenster liefert einen robusten Bewegungswert; anschließend wird der Median der gültigen Fenster gespeichert.

Sind die Profile eindeutig `quiet < pump < person`, berechnet PoolGuard automatisch die Grenzwerte jeweils in der Mitte zwischen den Profilen. Überlappen sich die Profile oder liegen sie in falscher Reihenfolge, werden die bisherigen bzw. Fallback-Grenzwerte weiter verwendet.

Fallbacks:

```yaml
pump_motion_threshold_cm: "1.5"
person_motion_threshold_cm: "3.0"
```

Mit **Reset Motion Calibration** können die gelernten Profile gelöscht und die Fallback-Grenzwerte wieder aktiviert werden.

## Zustandsstabilisierung

Damit einzelne Messbursts nicht unnötig Zustände umschalten, verwendet PoolGuard Bestätigungszyklen. Standardmäßig muss die Pumpenbewegung zweimal hintereinander bestätigt werden. Personen-/Badeaktivität wird schneller eingeschaltet und erst nach zwei unauffälligen Zyklen wieder gelöscht.

Während personentypische Bewegung erkannt wird, wird der Pumpenzustand bewusst gehalten. Starke Badebewegung soll also nicht fälschlich als Änderung des Pumpenzustands interpretiert werden.

## Wartungsmodus

Für Tests, Diagnose und OTA kann in Home Assistant ein persistenter Helper angelegt werden:

```text
input_boolean.poolguard_maintenance_mode
```

PoolGuard fragt dafür **nicht jede Minute extra WLAN ab**. Der in Home Assistant gespeicherte Wunsch wird bei der nächsten ohnehin stattfindenden API-Verbindung übertragen. Dadurch bleibt der normale Batteriebetrieb sparsam; die Aktivierung kann entsprechend bis zum nächsten regulären Status-/Ereignisbericht dauern.

Ist der Wartungsmodus aktiv, bleibt PoolGuard wach und führt fortlaufend Messbursts durch. Wird der Helper wieder ausgeschaltet, beendet PoolGuard den Wartungsbetrieb und kehrt – sofern das Initial Setup abgeschlossen ist – in Deep Sleep zurück.

Da Home Assistant den gewünschten Zustand persistent hält, kann ein Wartungswunsch auch dann gesetzt werden, wenn PoolGuard gerade schläft.

## OTA

OTA ist über ESPHome eingerichtet. Für ein zuverlässiges OTA-Update sollte PoolGuard zunächst über den Wartungsmodus wach und online gehalten werden. Bei der Erstinbetriebnahme ist OTA ebenfalls möglich, weil der Initial Setup Mode das Gerät wach hält.

## Home-Assistant-Entitäten

Wichtige Messwerte und Zustände sind:

- **Water Temperature**
- **Distance to Water**
- **Water Depth**
- **Water Level**
- **Pool Volume**
- **Water Surface Motion**
- **Pump Detected**
- **Person Detected**
- **Low Water Level**
- **WiFi Signal**
- **Calibration Status**

Diagnosewerte zeigen außerdem die drei gelernten Bewegungsprofile und die aktuell verwendeten Pumpen-/Personen-Grenzwerte.

Konfigurations-/Bedienelemente umfassen **Reference Water Depth**, **Minimum Safe Water Depth**, **Measure Now**, die Wasserstandsreferenz, die drei Bewegungs-Kalibrierungen, **Reset Motion Calibration**, **Finish Initial Setup** und **Reset Initial Setup**.

## Mechanisches Konzept

Der Body sitzt im Skimmer und wird seitlich mit geeignetem neutralvernetzendem Silikon an den Rippen fixiert. Der originale Mittelsteg bleibt erhalten. Der Body ist leicht zum Skimmer hin geneigt, damit Wasser zurücklaufen kann. Der lösbare Deckel trägt Batterie, XIAO ESP32-C3 und Elektronik; A02YYUW und DS18B20 erfassen Wasseroberfläche bzw. Wassertemperatur. Eine externe WLAN-Antenne kann unter dem Kunststoffdeckel positioniert werden.

Die aktuellen Druckdateien liegen in [`3D-Files/`](3D-Files/). Gedruckt wird mit einem Bambu Lab A1 Mini.

<p align="center">
  <img src="images/PG.jpg" alt="PoolGuard" width="90%">
</p>

## Sicherheit

- Nur eine geschützte, qualitativ gute 18650-Zelle verwenden.
- Akku nicht kurzschließen, quetschen, verpolen oder unbeaufsichtigt laden.
- Elektronik vor Kondenswasser und Spritzwasser schützen.
- Den A02YYUW im Deep Sleep über den Pololu vollständig abschalten.
- Pumpen- und Personen-/Aktivitätserkennung niemals als alleinige sicherheitsrelevante Überwachung verwenden.

## Unterstützung

<a href="https://paypal.me/toor0001/5"><img src="assets/paypal-support-en.svg" alt="Buy me a coffee via PayPal" width="430"></a>

## Lizenz

Software und Dokumentation stehen unter der MIT-Lizenz. Die CAD-Dateien sind aktuell noch Prototypen; vor dem ersten stabilen Release kann dafür eine eigene Hardware-Lizenz ergänzt werden.

## Hinweis zur Entwicklung

Dieses Projekt ist im Rahmen eines kollaborativen **Vibe-Coding-Workflows** mit **ChatGPT** und **OpenAI Codex** entstanden. Beide Tools wurden für Code-Erstellung, Reviews, Fehlersuche und Dokumentation eingesetzt.

Hardwareaufbau, Integrationsentscheidungen, praktische Tests und die abschließende Verantwortung für das Projekt liegen beim Projektbetreiber.
