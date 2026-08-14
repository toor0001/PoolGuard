# A02YYUW / DYP-A02 – Stromversorgung, Pololu und UART-Varianten

Diese Notiz dokumentiert die für PoolGuard relevante Verdrahtung und die Unterscheidung zwischen **UART Auto Output** und **UART Controlled Output** des DYP-A02.

## Verdrahtung / Aderfarben

Für die hier verwendete UART-Kabelvariante gilt:

| Ader | Sensorfunktion | PoolGuard |
|---|---|---|
| Rot | VCC | Pololu VOUT |
| Schwarz | GND | gemeinsames GND |
| Gelb | RX | abhängig von der UART-Variante; siehe unten |
| Weiß | TX | XIAO D7 / GPIO20 (UART RX des ESP32-C3) |

Der Pololu Mini MOSFET Slide Switch LV #2810 wird von PoolGuard über D2 / GPIO4 (`ON`) gesteuert. Seine Aufgabe ist ausschließlich die **harte Stromabschaltung des A02**. Im Deep Sleep bleibt der A02 dadurch vollständig stromlos.

## Warum der Pololu trotz niedrigem Sensor-Standby verwendet wird

Der DYP-A02 ist bereits auf niedrigen Ruhestrom ausgelegt; für die UART-Controlled-Variante wird ein sehr niedriger Standby-Verbrauch spezifiziert. PoolGuard besitzt den Pololu jedoch bereits und verwendet ihn bewusst weiter:

1. Der A02 wird während des ESP32-C3-Deep-Sleep vollständig von der Versorgung getrennt.
2. Es gibt keinen unbekannten Restverbrauch des Sensors zwischen Wake-Zyklen.
3. Das Verhalten ist unabhängig davon, wie zuverlässig eine konkrete A02-Charge ihren internen Sleep-/Standby-Modus erreicht.
4. Die vorhandene Hardware muss nicht umgebaut werden.

Der Pololu ersetzt jedoch **nicht** die UART-Betriebsart des Sensors. Er schaltet nur dessen Versorgung.

## UART Auto Output vs. UART Controlled Output

DYP bietet den A02 in unterschiedlichen, beim Kauf festgelegten Ausgangsvarianten an. Diese Varianten sind nicht nur unterschiedliche Softwaremodi desselben Geräts.

### UART Auto Output

Nach dem Einschalten beginnt der Sensor selbstständig mit Messungen und sendet die Ergebnisse über TX. RX dient bei dieser Variante zur Auswahl des Ausgabeverhaltens (processed/stabiler bzw. real-time, abhängig vom konkreten Datenblatt).

Für PoolGuard bedeutet das:

1. GPIO4 schaltet den Pololu ein.
2. Der A02 erhält Versorgung.
3. Der ESP wartet auf die automatisch eintreffenden UART-Frames an GPIO20.
4. Nach dem Messburst schaltet GPIO4 den Pololu wieder aus.

**Hier ist kein zusätzlicher UART-Trigger erforderlich.**

### UART Controlled Output

Bei der Controlled-Variante reicht das Einschalten der Versorgung **nicht** aus, um eine Entfernungsmessung auszulösen. Eine Messung wird über RX getriggert (laut DYP-Dokumentation durch eine fallende Flanke bzw. geeignete serielle Aktivität). Anschließend gibt TX genau den Messwert aus.

Für PoolGuard bedeutet das:

1. GPIO4 schaltet den Pololu ein.
2. Der A02 erhält Versorgung.
3. PoolGuard muss zusätzlich RX des A02 ansteuern und für jede gewünschte Messung triggern.
4. TX des A02 liefert das Messergebnis an GPIO20.
5. Nach dem Messburst schaltet GPIO4 den Pololu wieder aus.

**Der Pololu macht die Controlled-Ansteuerung also nicht überflüssig.** Power-On und Measurement-Trigger sind zwei verschiedene Vorgänge.

## Welche Variante besitzt der vorhandene Sensor?

Die ursprüngliche Bestellung bezeichnet die Variante lediglich als **“Black UART”**. Daraus lässt sich nicht sicher ableiten, ob es sich um UART Auto oder UART Controlled handelt. Die ursprüngliche Händlerseite ist nicht mehr verfügbar.

Deshalb darf die Firmware nicht allein aufgrund der Kabelfarben auf Controlled oder Auto umgestellt werden.

Praktischer Identifikationstest:

1. Rot an Pololu VOUT, Schwarz an GND und Weiß/TX an GPIO20 anschließen.
2. Gelb/RX zunächst isoliert und zugänglich lassen.
3. A02 über den Pololu einschalten.
4. Prüfen, ob ohne RX-Ansteuerung gültige UART-Distanzframes eintreffen.

Wenn nach dem Einschalten selbstständig fortlaufend gültige Frames eintreffen, spricht das für **UART Auto Output**. Wenn bei sicher korrekter Versorgung und TX-Verbindung keine Messframes eintreffen, muss die Controlled-Variante geprüft und RX als Trigger angeschlossen werden.

Gelb/RX deshalb vor Abschluss dieses Tests **nicht abschneiden**.

## Konsequenz für die aktuelle PoolGuard-Firmware

Die derzeit dokumentierte PoolGuard-Verdrahtung (`A02 RX` offen, `A02 TX -> GPIO20`) entspricht **UART Auto Output**.

Solange der vorhandene Sensor noch nicht eindeutig als UART Controlled identifiziert ist, soll diese Implementierung nicht vorsorglich auf Controlled umgebaut werden.

Falls der Sensor als Controlled identifiziert wird, benötigt PoolGuard einen zusätzlichen freien GPIO für A02 RX/Trigger und die Messroutine muss explizite Triggerimpulse erzeugen. Der Pololu bleibt trotzdem bestehen und schaltet weiterhin die komplette A02-Versorgung zwischen den Wake-Zyklen ab.

## UART-Daten

Der A02 überträgt Distanzdaten als UART-Frame mit Startbyte, High-/Low-Byte der Distanz und Prüfsumme. Die Distanz ergibt sich in Millimetern aus `Data_H * 256 + Data_L`. PoolGuard soll nur Frames mit gültiger Prüfsumme und plausibler Distanz akzeptieren.

Wichtig: Baudrate und exaktes Timing müssen zum Datenblatt der tatsächlich vorhandenen A02-Version passen; unterschiedliche DYP-Dokumentstände und Produktvarianten dürfen nicht ungeprüft miteinander vermischt werden.

## DS18B20 – zur Abgrenzung

Der Wassertemperaturfühler ist unabhängig vom A02 verdrahtet:

- Rot -> 3.3 V
- Schwarz -> GND
- Gelb/DATA -> D3 / GPIO5
- 4.7 kOhm Pull-up zwischen DATA und 3.3 V

Der DS18B20 bleibt versorgt; der Pololu schaltet ausschließlich den A02.
