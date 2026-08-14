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

Die Kabelfarben am verifizierten Prototyp sind: A02 Rot/VCC an Pololu VOUT,
Schwarz/GND an gemeinsame Masse, Weiß/TX an D7/GPIO20 und Gelb/RX-Trigger an
D1/GPIO3. Pololu ON liegt an D2/GPIO4. DS18B20 Data liegt mit einem 4,7-kΩ-
Pull-up nach 3,3 V an D3/GPIO5.

- **D2 / GPIO4 HIGH:** A02YYUW eingeschaltet
- **D2 / GPIO4 LOW oder hochohmig:** A02YYUW ausgeschaltet
- im Deep Sleep bleibt der A02YYUW stromlos

Der mechanische Schiebeschalter des Pololu muss auf „off“ stehen, damit ESPHome
den ON-Pin steuern kann. Die Datenleitung des DS18B20 liegt an D3/GPIO5 und
benötigt einen 4,7-kΩ-Pull-up nach 3,3 V.

Das DYP-UART-Controlled-Protokoll wurde am realen PoolGuard erfolgreich
verifiziert. GPIO3 bleibt normalerweise HIGH. Nachdem der Pololu den Sensor
eingeschaltet hat, sendet der ESP wiederholt fallende Flanken an A02 RX. Nach
jedem Trigger bleibt genügend Zeit für die Antwort `FF Data_H Data_L Checksum`
über A02 TX/GPIO20; zwischen den Triggern liegen mehr als 70 ms. Reale Tests
lieferten ungefähr 50 gültige Frames pro fünfsekündigem Burst. Zwischen den
Bursts und während Deep Sleep trennt der Pololu die komplette A02-Versorgung.

## Messzyklus und Stromsparbetrieb

**Measurement Interval** legt die gewünschte Zeit von einem normalen Wake bis
zum nächsten zwischen 1 und 15 Minuten fest; Standard sind 2 Minuten. PoolGuard
zieht die aktive Laufzeit des aktuellen Wakes vor dem Deep Sleep von dieser
Periode ab. Die Messzeit wird also nicht einfach zum eingestellten Intervall
addiert. Kürzere Intervalle reagieren schneller, benötigen aber mehr Akku.

Bei jedem normalen Wake bleibt WLAN aus. PoolGuard versorgt den A02 nur während
des Controlled-UART-Messbursts, wertet die Samples lokal aus und schaltet ihn
anschließend wieder aus. **Periodic Report Every** legt den regelmäßigen Report
nach 1 bis 120 Wakes fest; Standard sind 30. Damit ergeben 2 Minuten × 30 Wakes
ungefähr einen Report pro Stunde. Änderungen von Pumpen-, Personen-/
Badeaktivitäts- oder Niedrigwasserstatus lösen weiterhin sofort einen Report
aus. WLAN/API werden sonst nur für Erstinbetriebnahme oder Wartung verwendet.

Beide Intervall-Entities verwenden `restore_value` und überleben einen
Akkuwechsel. ESPHome schreibt sie nur, wenn der Benutzer den Wert ändert.
Messwerte, Wake-Zähler und Laufzeitzustände werden nicht bei jedem Wake in den
Flash geschrieben; Zähler und offene Reportzustände verbleiben im RTC-Speicher.

### Home-Assistant-Zustände und Schlafstatus

Vor einem verbundenen Schlafübergang meldet PoolGuard **PoolGuard Status =
Sleeping / Offline** und ruft direkt `deep_sleep.enter` auf. WLAN wird bewusst
nicht vorher abgeschaltet: ESPHome muss die native API als erwarteten Deep-
Sleep-Disconnect sauber schließen. Home Assistant kann dadurch die zuletzt
erfolgreich gemeldeten Messwerte während des Schlafens verfügbar halten.

Wurde PoolGuard ursprünglich zu Home Assistant hinzugefügt, bevor die Firmware
die `deep_sleep`-Komponente enthielt, sollte die ESPHome-Integration bei wachem
Gerät einmal entfernt und neu hinzugefügt werden. Andernfalls kann HA weiterhin
alle nativen ESPHome-Entities als nicht verfügbar markieren. Ein echter
unerwarteter Ausfall oder ein noch nie empfangener Zustand kann weiterhin
`unavailable` ergeben.

Die Status-Entity zeigt **Initial Setup**, **Measuring**, **Reporting**,
**Maintenance** oder **Sleeping / Offline**. Bei einem normalen Offline-Wake
kann Home Assistant die kurze Messphase nicht live sehen, weil ein WLAN-Start
nur für diese Anzeige dem Stromsparziel widersprechen würde. Aus HA-Sicht bleibt
der gespeicherte Status daher Sleeping / Offline. Aus demselben Grund gibt es
keinen irreführenden Firmware-Binary-Sensor `Device Awake`. Für die aktuelle
Erreichbarkeit dient die HA-Integrationsverbindung, für den beabsichtigten
Betriebszustand PoolGuard Status.

<p align="center">
  <img src="images/HA.png" alt="PoolGuard-ESPHome-Integration in Home Assistant mit weiterhin verfügbaren Sensorwerten" width="92%"><br>
  <em>PoolGuard in Home Assistant: Auch während <code>Sleeping / Offline</code> bleiben die zuletzt erfolgreich übertragenen Messwerte verfügbar. Der Gerätestatus zeigt dabei bewusst den Deep-Sleep-Zustand an.</em>
</p>

**Status Heartbeat** ändert sich ausschließlich nach einem vollständigen,
erfolgreichen Report. Seine `last_updated`-Metadaten in Home Assistant bilden
damit **Last Successful Report** ab, ohne ESP-Zeitstempel oder Flash-Schreiben.

Die persistente Number-Entity **Water Temperature Offset** reicht von -5,0 bis
+5,0 °C in 0,1-°C-Schritten und wird nur bei Änderungen gespeichert. Zeigt der
DS18B20 beispielsweise 25,6 °C und ein Referenzthermometer 26,8 °C, wird +1,2 °C
eingestellt; **Water Temperature** meldet danach 26,8 °C. Der Offset überlebt
einen Akkuwechsel.

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
sich für die lokalen Messungen weiterhin nicht mit dem WLAN.
Die gespeicherte Anforderung wird erst bei der nächsten ohnehin erforderlichen,
ereignisgesteuerten oder regelmäßigen Verbindung zur Home-Assistant-API
übernommen. Mit den Standardwerten 2 Minuten und 30 Wakes beträgt die maximale
Verzögerung ungefähr eine Stunde; eine Zustandsänderung kann die Aktivierung
früher auslösen. **PoolGuard Status = Maintenance** bestätigt, dass die
Anforderung das Gerät tatsächlich erreicht hat.

Im Wartungsmodus bleibt PoolGuard wach und mit WLAN/API verbunden. Etwa alle
5 Sekunden erfolgen ein eingeschalteter A02-Messburst sowie die Auswertung von
Wasserstand, Pumpen- und Badeaktivität. Die Wassertemperatur wird alle
30 Sekunden aktualisiert. Zwischen den Messbursts wird der A02
ausgeschaltet. Ein dauerhaft eingeschalteter Wartungsmodus verkürzt die
Akkulaufzeit erheblich.
Maintenance ist tatsächlich aktiv, wenn **PoolGuard Status** Maintenance zeigt,
das ESPHome-Live-Log verbunden bleibt und dort fortlaufende A02-Messbursts zu
sehen sind. Dadurch eignet sich der Modus insbesondere für OTA-Arbeiten.

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
der lokalen Messzyklen. OTA steht im Wartungsmodus zur Verfügung. Nach OTA den
Helfer wieder ausschalten; OFF beendet Maintenance und führt PoolGuard zurück
in Deep Sleep und Akkubetrieb.

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
2. Den Wert bei **Reference Water Depth** eintragen.
3. **Measure Now** drücken und den A02-Abstand auf Plausibilität prüfen.
4. Bei unverändertem Wasserstand **Set Water Level Reference** drücken.
5. Bei **Minimum Safe Water Depth** die niedrigste reale Wassertiefe festlegen,
   bei der der Skimmer die Umwälzpumpe noch sicher mit Wasser versorgt.
6. Pumpe AUS, niemand im Wasser, Wasser möglichst ruhig: **Calibrate Quiet
   Water** drücken.
7. Pumpe AN, niemand im Wasser: **Calibrate Pump** drücken.
8. Typische Bade-/Schwimmbewegung erzeugen: **Calibrate Person** drücken.
9. Die drei Motion-Profile und automatisch gelernten Thresholds prüfen.
10. **Finish Initial Setup** drücken.

Ab diesem Zeitpunkt wird die Wassertiefe aus der Änderung des A02-Abstands berechnet. Der Abstand vom Sensor bis zum Beckenboden muss nicht separat gemessen werden.

Die Berechnung wurde am realen Pool getestet: Eine Referenz von 105,5 cm ergab
anschließend ungefähr 105,4 cm. Wasserreferenz, sichere Mindesttiefe, alle drei
Motion-Profile, gelernte Thresholds und Temperatur-Offset liegen persistent im
Flash und überleben einen Akkuwechsel.

In der YAML ist standardmäßig ein runder Pool mit **5,0 m Durchmesser** und **120 cm maximaler Wassertiefe** hinterlegt. `Water Level` wird daraus als Prozentwert berechnet. `Pool Volume` nutzt die aktuelle Wassertiefe und berechnet das Volumen als Zylinder. Bei 5,0 m Durchmesser und 120 cm Tiefe ergeben sich theoretisch etwa **23,56 m³**. Für einen anderen Pool werden `pool_diameter_m` und `pool_max_depth_cm` angepasst. Bei nicht zylindrischer Bodenform ist das Volumen entsprechend nur eine Näherung.

## Geführte Bewegungs-Kalibrierung

Die Erkennung von Pumpe und Badeaktivität hängt stark vom konkreten Skimmer, Pumpendurchfluss, Wasserstand und der Poolgeometrie ab. Deshalb enthält PoolGuard einen geführten Kalibriermodus.

Im Erstinbetriebnahmemodus werden diese drei Buttons nach Möglichkeit nacheinander ausgeführt:

1. **Calibrate Quiet Water** – Pumpe aus, niemand im Pool.
2. **Calibrate Pump** – Umwälzpumpe an, niemand im Pool.
3. **Calibrate Person** – normale Bade-/Schwimmbewegung im Pool.

Jede Phase misst ungefähr 60 Sekunden und speichert den Median der Wasserbewegung. Die Bewegung wird aus einer getrimmten Messwert-Spanne berechnet, damit einzelne Ausreißer oder Spritzer weniger Einfluss haben als bei einem einfachen Min/Max-Wert. Liegen die Profile eindeutig in der Reihenfolge `ruhig < Pumpe < Person`, berechnet PoolGuard automatisch die Grenzwerte. Überlappen sich die Profile, bleiben die bisherigen beziehungsweise Fallback-Grenzwerte aktiv.

Die drei Werte beschreiben zunehmende Bewegung der Wasseroberfläche: Quiet
Motion ist das Grundrauschen, Pump Motion die normale Umwälzung und Person
Motion typische Badeaktivität. Beispielsweise sind Quiet 0,50 cm, Pump 0,90 cm
und Person 1,45 cm korrekt geordnet. PoolGuard setzt den gelernten Pump-
Threshold in die Mitte zwischen Quiet und Pump (0,70 cm) und den Person-
Threshold in die Mitte zwischen Pump und Person (etwa 1,18 cm).

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
  <img src="images/PG.jpg" alt="Mechanisches PoolGuard-Gehäusekonzept" width="88%"><br>
  <em>PoolGuard-Gehäusekonzept für Skimmerdeckel und lösbaren Elektronikträger.</em>
</p>

## Fertiger Aufbau

<table>
  <tr>
    <td width="50%" align="center"><img src="images/Final1.JPG" alt="PoolGuard-Elektronikträger mit Akku, Controller und Sensoren" width="100%"></td>
    <td width="50%" align="center"><img src="images/Final2.JPG" alt="Am Skimmerdeckel montierter PoolGuard-Sensorträger" width="100%"></td>
  </tr>
  <tr>
    <td align="center"><em>Verdrahteter Elektronikträger mit 18650-Akku, XIAO-Controller, A02 und DS18B20.</em></td>
    <td align="center"><em>Sensorseite des fertigen Trägers am originalen Skimmerdeckel.</em></td>
  </tr>
</table>

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
