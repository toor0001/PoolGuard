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
A02YYUW RX  -------- D1 / GPIO3 (Trigger-Ausgang)
```

- **D2 / GPIO4 HIGH:** A02YYUW eingeschaltet
- **D2 / GPIO4 LOW oder hochohmig:** A02YYUW ausgeschaltet
- im Deep Sleep bleibt der A02YYUW stromlos

Der mechanische Schiebeschalter des Pololu muss auf „off“ stehen, damit ESPHome
den ON-Pin steuern kann. Die Datenleitung des DS18B20 liegt an D3/GPIO5 und
benötigt einen 4,7-kΩ-Pull-up nach 3,3 V.

Die aktuelle Firmware unterstützt das DYP-UART-Controlled-Protokoll: GPIO3
bleibt normalerweise HIGH. Nachdem der Pololu den Sensor eingeschaltet hat,
sendet der ESP wiederholt fallende Flanken an A02 RX. Nach jedem Trigger bleibt
genügend Zeit für die Antwort `FF Data_H Data_L Checksum` über A02 TX/GPIO20;
zwischen den Triggern liegen mehr als 70 ms. Außerhalb eines Messbursts trennt
der Pololu weiterhin die komplette A02-Versorgung. Die genaue Sensorvariante
und dieses Controlled-UART-Verhalten müssen am realen Prototyp noch praktisch
verifiziert werden.

## Messzyklus und Stromsparbetrieb

Im Normalbetrieb wacht PoolGuard ungefähr einmal pro Minute mit ausgeschalteter
WLAN-Schnittstelle auf und führt lokal einen kurzen A02YYUW-Messburst aus. Ist
kein Report erforderlich, geht das Gerät ohne Funkstart direkt zurück in Deep
Sleep. WLAN und Home-Assistant-API werden nur bei einer Änderung von Pumpen-,
Badeaktivitäts- oder Niedrigwasserstatus sowie für den regelmäßigen Report nach
30 Wake-Zyklen aktiviert. Ein Report enthält den vollständigen aktuellen
Messdatensatz einschließlich einer frischen korrigierten Wassertemperatur und
der persistenten Kalibrierdiagnose. Vor Deep Sleep wird WLAN wieder deaktiviert.

Home Assistant behält während Deep Sleep die zuletzt erfolgreich gemeldeten
Entity-Werte. Dafür werden nicht bei jedem Wake Messwerte in den ESP-Flash
geschrieben. Wake-Zähler und zuletzt gemeldete Binärzustände liegen im RTC-
Speicher, sodass fehlgeschlagene Reports offen bleiben und erneut versucht
werden. Wurde das Gerät ursprünglich in Home Assistant hinzugefügt, bevor die
Firmware die `deep_sleep`-Komponente enthielt, muss das ESPHome-Gerät einmal
entfernt und neu hinzugefügt werden, damit Home Assistant den erwarteten Deep-
Sleep-Disconnect kennt. Die `last_updated`-Metadaten von **Status Heartbeat**
zeigen, wann Home Assistant zuletzt einen erfolgreichen Report empfangen hat.

Die konfigurierbare Number-Entity **Water Temperature Offset** reicht von -5,0
bis +5,0 °C in 0,1-°C-Schritten und überlebt einen vollständigen Stromverlust.
**Water Temperature** entspricht dem DS18B20-Messwert plus diesem Offset; eine
separate Raw-Temperatur-Entity wird nicht bereitgestellt.

OTA steht zur Verfügung, wenn PoolGuard im Initial Setup, Wartungsmodus oder
während eines Reports ohnehin online ist. Nur für OTA wird keine zusätzliche
WLAN-Verbindung aufgebaut.

## Wartungsmodus

In Home Assistant wird ein dauerhafter Helfer namens **PoolGuard Maintenance
Mode** mit der Entity-ID `input_boolean.poolguard_maintenance_mode` angelegt
(Einstellungen → Geräte & Dienste → Helfer → Helfer erstellen → Schalter). Der
Helfer hält den gewünschten Zustand fest. Ein einfacher ESPHome-Template-
Schalter wäre dafür ungeeignet, weil PoolGuard im Deep Sleep offline ist.

Das Einschalten wirkt während des Schlafens nicht sofort. PoolGuard verbindet
sich für die lokalen Messungen im Minutenabstand weiterhin nicht mit dem WLAN.
Die gespeicherte Anforderung wird erst bei der nächsten ohnehin erforderlichen,
ereignisgesteuerten oder regelmäßigen Verbindung zur Home-Assistant-API
übernommen. Beim voreingestellten Statusintervall von 30 Aufwachzyklen beträgt
die maximale Verzögerung etwas mehr als 30 Minuten; eine Zustandsänderung kann die
Aktivierung früher auslösen.

Im Wartungsmodus bleibt PoolGuard wach und mit WLAN/API verbunden. Etwa alle
5 Sekunden erfolgen ein eingeschalteter A02-Messburst sowie die Auswertung von
Wasserstand, Pumpen- und Badeaktivität. Die Wassertemperatur wird alle
30 Sekunden aktualisiert. Zwischen den Messbursts wird der A02
ausgeschaltet. Ein dauerhaft eingeschalteter Wartungsmodus verkürzt die
Akkulaufzeit erheblich.

Beim Ausschalten des Helfers endet der Wartungsmodus sofort. PoolGuard beendet
die Live-Messungen, schaltet den A02 aus, aktualisiert abschließend die
Temperatur und kehrt ohne Neustart zum normalen Deep-Sleep-Zyklus zurück.
Nach einem unerwarteten Reset wird der lokale Aktivzustand absichtlich nicht
wiederhergestellt: Der stromsparende Betrieb ist die sichere Voreinstellung;
die in Home Assistant gespeicherte Anforderung kann bei einer späteren normalen
API-Verbindung erneut übernommen werden.

Der Wartungsmodus wird zur Laufzeit von Home Assistant gesteuert und benötigt
kein erneutes Flashen. Er bleibt vom Erstinbetriebnahmemodus getrennt; die
Abfrage seines Helfers erzeugt keine zusätzlichen WLAN-Verbindungen während
der lokalen Messzyklen im Minutenabstand.

## Erstinbetriebnahmemodus

Beim ersten Start im Werkszustand wechselt PoolGuard automatisch in den
Erstinbetriebnahmemodus. Das im Flash gespeicherte Flag
`initial_setup_completed` ist zunächst false. Deshalb bleiben WLAN und die
Home-Assistant-API aktiv und Deep Sleep wird verhindert. Im Leerlauf bleibt der
A02 ausgeschaltet und wird nur für eine ausdrücklich gestartete Messung oder
Kalibrierung versorgt. Der DS18B20 bleibt verfügbar.

Für die Inbetriebnahme ist nur ein Firmware-Flash nötig:

1. PoolGuard flashen und zu Home Assistant hinzufügen.
2. Wasserstandsreferenz und sichere Mindestwassertiefe einstellen.
3. Ruhiges Wasser, Pumpe und Person nach Möglichkeit kalibrieren.
4. **Finish Initial Setup** drücken.

PoolGuard akzeptiert den Abschluss erst nach einer gültigen
Wasserstandsreferenz. Eine unvollständige Bewegungskalibrierung ist zulässig;
PoolGuard protokolliert eine Warnung und verwendet weiterhin die
Fallback-Grenzwerte. Danach speichert es das Abschluss-Flag im Flash, schaltet
den A02 aus und beginnt mit dem stromsparenden Normalbetrieb. Jeder spätere
Start, Reset und Deep-Sleep-Aufwachvorgang beginnt direkt im Normalmodus. Ein
zweiter Firmware-Flash ist nicht erforderlich.

Für eine erneute Inbetriebnahme **Reset Initial Setup** innerhalb von 10
Sekunden zweimal drücken. Diese Bestätigung schützt vor versehentlichem
Zurücksetzen. PoolGuard löscht das dauerhafte Flag sofort, bleibt mit WLAN/API
wach und lässt den A02 ausgeschaltet, bis eine Messung oder Kalibrierung
gestartet wird. Vorhandene Kalibrierwerte werden dabei nicht automatisch
gelöscht.

## Wasserstands-Referenzkalibrierung

Für den Wasserstand sind keine getrennten „leer“-/„voll“-Referenzpunkte mehr nötig. **Ein einziger bekannter realer Wasserstand reicht.**

Im automatisch gestarteten Erstinbetriebnahmemodus:

1. Die **aktuelle tatsächliche Wassertiefe** im Pool in Zentimetern messen.
2. Diesen Wert in Home Assistant bei **Reference Water Depth** eintragen.
3. Während der Wasserstand unverändert ist **Set Water Level Reference** drücken.
4. PoolGuard speichert gleichzeitig den aktuellen A02-Abstand und die bekannte reale Wassertiefe.
5. Anschließend bei **Minimum Safe Water Depth** die niedrigste reale Wassertiefe eintragen, bei der der Skimmer die Umwälzpumpe noch zuverlässig mit Wasser versorgt.

Ab diesem Zeitpunkt wird die Wassertiefe aus der Änderung des A02-Abstands berechnet. Der Abstand vom Sensor bis zum Beckenboden muss nicht separat gemessen werden.

In der YAML ist standardmäßig ein runder Pool mit **5,0 m Durchmesser** und **120 cm maximaler Wassertiefe** hinterlegt. `Water Level` wird daraus als Prozentwert berechnet. `Pool Volume` nutzt die aktuelle Wassertiefe und berechnet das Volumen als Zylinder. Bei 5,0 m Durchmesser und 120 cm Tiefe ergeben sich theoretisch etwa **23,56 m³**. Für einen anderen Pool werden `pool_diameter_m` und `pool_max_depth_cm` angepasst. Bei nicht zylindrischer Bodenform ist das Volumen entsprechend nur eine Näherung.

## Geführte Bewegungs-Kalibrierung

Die Erkennung von Pumpe und Badeaktivität hängt stark vom konkreten Skimmer, Pumpendurchfluss, Wasserstand und der Poolgeometrie ab. Deshalb enthält PoolGuard einen geführten Kalibriermodus.

Im Erstinbetriebnahmemodus werden diese drei Buttons nach Möglichkeit nacheinander ausgeführt:

1. **Calibrate Quiet Water** – Pumpe aus, niemand im Pool.
2. **Calibrate Pump** – Umwälzpumpe an, niemand im Pool.
3. **Calibrate Person** – normale Bade-/Schwimmbewegung im Pool.

Jede Phase misst ungefähr 60 Sekunden und speichert den Median der Wasserbewegung. Die Bewegung wird aus einer getrimmten Messwert-Spanne berechnet, damit einzelne Ausreißer oder Spritzer weniger Einfluss haben als bei einem einfachen Min/Max-Wert. Liegen die Profile eindeutig in der Reihenfolge `ruhig < Pumpe < Person`, berechnet PoolGuard automatisch die Grenzwerte. Überlappen sich die Profile, bleiben die bisherigen beziehungsweise Fallback-Grenzwerte aktiv.

Die Bewegungskalibrierung ist für den Abschluss der Erstinbetriebnahme optional.
Falls sie nicht möglich ist, protokolliert PoolGuard eine Warnung und verwendet
die Fallback-Grenzwerte. Nach der Kalibrierung startet **Finish Initial Setup**
den stromsparenden Normalbetrieb.

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
2. `esphome/secrets.example.yaml` als Vorlage für die eigenen ESPHome-Secrets verwenden. PoolGuard verwendet die gerätespezifischen Namen `poolguard_api_encryption_key` und `poolguard_ota_password`; die WLAN-Secrets können gemeinsam genutzt werden.
3. Pins, Poolgeometrie und Kalibrierwerte in `esphome/poolguard.yaml` prüfen.
4. XIAO ESP32-C3 zunächst per USB flashen.
5. PoolGuard beim automatischen ersten Start zu Home Assistant hinzufügen.
6. Reale Wassertiefe messen, bei **Reference Water Depth** eintragen und **Set Water Level Reference** drücken.
7. **Minimum Safe Water Depth** für den realen Skimmer/Pumpenbetrieb festlegen.
8. Kalibrierung für ruhiges Wasser, Pumpe und Badeaktivität nach Möglichkeit durchführen.
9. **Finish Initial Setup** drücken. PoolGuard speichert den Abschluss und startet den stromsparenden Normalbetrieb; ein zweiter Flash ist nicht erforderlich.

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
