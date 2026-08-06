# PoolGuard

**Batteriebetriebener Pool-Skimmer-Sensor für Home Assistant und ESPHome**

Deutsch · [English](README.md)

PoolGuard ist ein DIY-Sensormodul für einen **AstralPool-Skimmer mit 17,5 l**. Es misst Wasserstand und Wassertemperatur und wertet zusätzlich die Bewegung der Wasseroberfläche aus. Daraus kann PoolGuard abschätzen, ob die Umwälzpumpe läuft und ob ungewöhnlich starke Wasserbewegung auf Badeaktivität hindeutet.

Der Schwerpunkt liegt auf niedrigem Energieverbrauch: Ein XIAO ESP32-C3 wacht im Normalbetrieb nur kurz auf, der A02YYUW wird über einen Pololu-MOSFET vollständig ein- und ausgeschaltet und WLAN wird nur bei Bedarf verwendet.

> **Projektstatus:** Prototyp / in Entwicklung. Pumpen- und Personenerkennung müssen am realen Pool getestet und kalibriert werden.

> **Wichtig:** PoolGuard erkennt keine Person direkt. Es klassifiziert ausschließlich Wasserbewegung und ist kein Sicherheitssystem oder Ersatz für Poolaufsicht.

## Funktionen

- Abstand zur Wasseroberfläche mit A02YYUW
- reale Wassertiefe aus einer einzigen Wasserstandsreferenz
- Wasserstand in Prozent und geschätztes Poolvolumen
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
- Status-Heartbeat für eine externe Home-Assistant-Ausfallüberwachung

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

Die Firmware verwendet bewusst keine ESP32-C3-Strapping-Pins GPIO2, GPIO8 oder GPIO9.

| Funktion | XIAO ESP32-C3 |
|---|---|
| Pololu ON | D2 / GPIO4 |
| A02YYUW TX → ESP RX | D7 / GPIO20 |
| DS18B20 DATA | D3 / GPIO5 |
| GPIO3 | frei |

Der DS18B20 benötigt einen **4,7-kΩ-Pull-up zwischen DATA und 3,3 V**.

```text
XIAO 3.3 V  -------- VIN   Pololu 2810
XIAO GND    -------- GND   Pololu 2810
GPIO4 / D2  -------- ON    Pololu 2810
Pololu VOUT -------- VCC   A02YYUW
A02YYUW GND -------- GND
A02YYUW TX  -------- GPIO20 / D7
A02YYUW RX  -------- nicht anschließen
```

Pololu `ON` ist active-high. Im Deep Sleep bleibt der A02YYUW stromlos.

## Normalbetrieb und Stromsparen

Standardmäßig sind konfiguriert:

```yaml
sleep_duration: 55s
detection_burst_duration: 5s
status_every_wakes: "30"
```

Nach abgeschlossenem Setup wacht PoolGuard ungefähr einmal pro Minute auf. Der A02YYUW wird eingeschaltet, sammelt während des Messbursts Messwerte und wird anschließend sofort wieder abgeschaltet. Die Auswertung von Wasserstand, Niedrigwasser, Pumpenstatus und Badeaktivität erfolgt lokal, ohne dafür jedes Mal WLAN zu aktivieren.

Ändert sich **Pumpe**, **Bade-/Personenaktivität** oder **Niedrigwasserstatus**, verbindet sich PoolGuard beim betreffenden Messzyklus mit WLAN/Home Assistant und meldet die Änderung. Ohne Zustandsänderung erfolgt der reguläre Statusbericht standardmäßig nach etwa **30 Wake-Zyklen**, also ungefähr alle 30 Minuten. Dabei wird auch die Wassertemperatur aktualisiert.

Die letzten Zustände und Zähler werden im RTC-Speicher über Deep Sleep hinweg gehalten. Schlägt eine WLAN-/API-Verbindung fehl, bleibt eine Zustandsänderung offen und wird bei einem späteren Wake erneut übertragen.

### Messburst und Akkulaufzeit

`detection_burst_duration` bestimmt den Kompromiss zwischen Erkennungsqualität und Akkulaufzeit:

```yaml
detection_burst_duration: 5s
```

Ein längerer Burst liefert mehr A02-Messwerte, hält ESP und Sensor aber länger aktiv. **5 Sekunden sind ein konservativer Ausgangswert für Inbetriebnahme und Tests.** Nach realen Tests am Pool können z. B. **2 s oder 1,5 s** ausprobiert werden. Die Firmware benötigt für einen gültigen normalen Zyklus mindestens fünf gültige A02-Frames, deshalb sollten kürzere Bursts erst praktisch geprüft werden.

Auch der komplette Wake-Zyklus kann später vergrößert werden, z. B. auf ungefähr zwei Minuten. Dann reduziert sich die Zahl der Messbursts deutlich; Zustandsänderungen können entsprechend erst beim nächsten Wake erkannt werden. Wird der Wake-Abstand verändert, sollte `status_every_wakes` so angepasst werden, dass der gewünschte regelmäßige Statusabstand – derzeit etwa 30 Minuten – erhalten bleibt.

Die geführte Bewegungs-Kalibrierung ist davon getrennt und verwendet weiterhin eigene 5-Sekunden-Fenster.

## Robuste A02-Auswertung

Der A02YYUW sendet UART-Frames mit Prüfsumme. PoolGuard akzeptiert nur gültige Frames und plausible Entfernungen von 30 bis 4500 mm. Für einen normalen Zyklus werden mindestens fünf gültige Frames benötigt.

Für die Entfernung wird der **Median** verwendet. Die Wasserbewegung wird aus dem Bereich zwischen ungefähr dem 10. und 90. Perzentil berechnet, nicht aus einem empfindlichen Roh-Min/Max-Wert. Einzelne Ausreißer oder Spritzer haben dadurch weniger Einfluss.

## Wasserstand, Poolvolumen und Niedrigwasser

Eine einzige bekannte reale Wassertiefe reicht als Referenz. Bei **Set Water Level Reference** speichert PoolGuard gleichzeitig den aktuellen A02-Abstand und die tatsächlich gemessene Wassertiefe. Spätere Änderungen des Sensorabstands werden direkt in eine neue Wassertiefe umgerechnet.

Standardgeometrie:

```yaml
pool_diameter_m: "5.0"
pool_max_depth_cm: "120.0"
low_water_hysteresis_cm: "1.0"
```

`Water Level` ist die aktuelle Tiefe relativ zur konfigurierten Maximaltiefe. `Pool Volume` wird für einen zylindrischen Pool als `π × Radius² × Wassertiefe` geschätzt; bei 5,0 m Durchmesser und 120 cm Tiefe sind das etwa **23,56 m³**.

`Minimum Safe Water Depth` wird in Home Assistant passend zum realen Skimmer festgelegt. Wird dieser Wert erreicht oder unterschritten, wird `Low Water Level` aktiv. Zurückgesetzt wird der Zustand erst bei Mindesttiefe + Hysterese, damit er am Grenzwert nicht flattert.

## Initial Setup Mode

Ein neues oder zurückgesetztes Gerät startet automatisch im **Initial Setup Mode**. Der Zustand `initial_setup_completed` liegt persistent im Flash und übersteht Deep Sleep, Reset und vollständigen Stromverlust. Ein zweiter Firmware-Flash zum Verlassen der Erstinbetriebnahme ist nicht nötig.

Solange das Setup nicht abgeschlossen ist, bleibt PoolGuard wach und erreichbar; der A02YYUW bleibt im Leerlauf aus. Die Erstinbetriebnahme besteht im Wesentlichen aus Wasserstandsreferenz, sicherer Mindestwassertiefe und optional den Bewegungsprofilen. Mit **Finish Initial Setup** wird der Zustand gespeichert und PoolGuard wechselt in den normalen Deep-Sleep-Betrieb. **Reset Initial Setup** muss zur Sicherheit zweimal innerhalb von 10 Sekunden gedrückt werden.

## Geführte Bewegungs-Kalibrierung

Drei Profile können gelernt werden:

1. **Calibrate Quiet Water** – Pumpe aus, niemand im Pool.
2. **Calibrate Pump** – Pumpe an, niemand im Pool.
3. **Calibrate Person** – typische Bade-/Schwimmbewegung.

Standardmäßig werden pro Profil **12 Fenster à 5 Sekunden** ausgewertet. Sind die Profile eindeutig `quiet < pump < person`, berechnet PoolGuard die Pumpen- und Personen-Grenzwerte automatisch zwischen den Profilen. Andernfalls bleiben bisherige oder Fallback-Werte aktiv:

```yaml
pump_motion_threshold_cm: "1.5"
person_motion_threshold_cm: "3.0"
```

Pumpenbewegung muss standardmäßig zweimal hintereinander bestätigt werden. Personen-/Badeaktivität wird schneller eingeschaltet und erst nach zwei unauffälligen Zyklen wieder gelöscht. Während personentypische Bewegung aktiv ist, wird der Pumpenzustand bewusst gehalten.

## Wartungsmodus und OTA

Für Tests, Diagnose und OTA wird in Home Assistant ein persistenter Helper mit exakt dieser Entity-ID angelegt:

```text
input_boolean.poolguard_maintenance_mode
```

PoolGuard aktiviert WLAN **nicht bei jedem Wake nur zum Prüfen dieses Helpers**. Home Assistant speichert den Wunsch; PoolGuard übernimmt ihn bei der nächsten ohnehin stattfindenden API-Verbindung. Im Wartungsmodus bleibt PoolGuard wach und führt fortlaufend Messbursts durch. Beim Ausschalten kehrt er – sofern das Initial Setup abgeschlossen ist – in Deep Sleep zurück.

Für OTA sollte zuerst der Wartungsmodus aktiviert werden, damit PoolGuard während des Updates wach und erreichbar bleibt.

## Akkuüberwachung ohne ADC: Status Heartbeat

PoolGuard misst bewusst **keine Akkuspannung**. Es gibt keinen Spannungsteiler und keinen Battery-Level-Sensor. Für den eigentlichen Zweck – erkennen, dass der Akku leer ist oder das Gerät nicht mehr meldet – ist eine externe Überwachung in Home Assistant robuster.

Dafür stellt die Firmware den Diagnosewert **Status Heartbeat** bereit. Er wird bei einer erfolgreichen WLAN/API-Meldung aktualisiert. Ohne Ereignisse geschieht das spätestens beim regulären Statusbericht, standardmäßig etwa alle 30 Minuten. Zustandsänderungen können zusätzliche Lebenszeichen erzeugen.

Die Benachrichtigung selbst gehört **nicht in ESPHome**, denn ein PoolGuard mit leerem Akku kann keine Warnung mehr senden. Home Assistant überwacht deshalb das Ausbleiben des Heartbeats. Empfohlen ist eine großzügige Schwelle, z. B. **2 Stunden**. Damit führen einzelne WLAN-Aussetzer nicht sofort zu einem Alarm.

Eine robuste HA-Variante verwendet einen Helper `input_datetime.poolguard_last_seen`: Eine Automation aktualisiert ihn immer dann, wenn `sensor.poolguard_status_heartbeat` einen gültigen Wert liefert; eine zweite Automation prüft, ob dieser Zeitstempel älter als zwei Stunden ist und sendet dann eine Benachrichtigung. `unknown` und `unavailable` sollten beim Aktualisieren des Last-Seen-Helpers ignoriert werden, weil PoolGuard im normalen Deep-Sleep-Betrieb zwischen den Verbindungen absichtlich offline ist.

Beispiel für das Aktualisieren des Helpers:

```yaml
alias: PoolGuard - Lebenszeichen speichern
trigger:
  - platform: state
    entity_id: sensor.poolguard_status_heartbeat
condition:
  - condition: template
    value_template: >-
      {{ trigger.to_state.state not in ['unknown', 'unavailable', 'none'] }}
action:
  - service: input_datetime.set_datetime
    target:
      entity_id: input_datetime.poolguard_last_seen
    data:
      timestamp: "{{ now().timestamp() }}"
mode: queued
```

Die eigentliche Warn-Automation kann anschließend z. B. alle 15 Minuten prüfen, ob `poolguard_last_seen` mehr als zwei Stunden zurückliegt. Damit werden leerer Akku, WLAN-Ausfall oder ein hängen gebliebenes Gerät gleichermaßen erkannt.

## Home-Assistant-Entitäten

Wichtige Werte und Zustände sind **Water Temperature**, **Distance to Water**, **Water Depth**, **Water Level**, **Pool Volume**, **Water Surface Motion**, **Pump Detected**, **Person Detected**, **Low Water Level**, **Status Heartbeat**, **WiFi Signal** und **Calibration Status**. Diagnosewerte zeigen außerdem die gelernten Bewegungsprofile und die aktuell verwendeten Grenzwerte.

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