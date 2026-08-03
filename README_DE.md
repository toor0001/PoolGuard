# PoolGuard

**Batteriebetriebener Pool-Skimmer-Sensor für Home Assistant und ESPHome**

Deutsch · [English](README.md)

PoolGuard ist ein DIY-Sensormodul für den Deckel eines **AstralPool-Skimmers mit 17,5 l**. Es überwacht den Abstand zur Wasseroberfläche, die Wassertemperatur und die Batteriespannung. Aus mehreren schnellen Abstandsmessungen kann zusätzlich versuchsweise erkannt werden, ob die Umwälzpumpe läuft und die Wasseroberfläche dadurch unruhiger ist.

> **Projektstatus:** Prototyp / in Entwicklung. Aktuell wird zuerst die Passschablone für die Fächer 3 und 4 getestet. Den Originaldeckel noch nicht bohren, schneiden oder dauerhaft verkleben, bevor die aktuelle CAD-Version geprüft wurde.

## Funktionen

- Wasserstand mit wasserdichtem **DFRobot A02YYUW**
- Wassertemperatur mit wasserdichtem **DS18B20**
- **Seeed Studio XIAO ESP32-C3** mit externer 2,4-GHz-Antenne
- eine geschützte 18650-Zelle
- Deep Sleep für lange Batterielaufzeit
- Batteriespannungsmessung über Spannungsteiler
- experimentelle Pumpenerkennung anhand der kurzfristigen Wasserunruhe
- lokale Einbindung in ESPHome und Home Assistant
- 3D-gedruckter Einsatz ausschließlich für dieses Skimmer-Projekt

## Verzeichnisstruktur

```text
cad/                 Parametrische Quellen und Testdrucke
esphome/             ESPHome-Konfiguration
hardware/            Verdrahtung und Stückliste
docs/                Montage und Kalibrierung
```

## Mechanisches Konzept

Der Body sitzt in den Skimmerfächern 3 und 4 und wird später seitlich mit geeignetem neutralvernetzendem Silikon an den Rippen fixiert. Der originale Mittelsteg bleibt vollständig erhalten. Der lösbare Deckel trägt die Technik:

- Batteriehalter und ESP auf der trockenen Innenseite;
- A02YYUW außen in Richtung Wasser;
- Durchführung für das DS18B20-Kabel;
- externe WLAN-Antenne möglichst nah am Kunststoffdeckel.

## Schnellstart

1. Passschablone aus `cad/stl/` drucken und die Kontur prüfen.
2. `esphome/secrets.example.yaml` als Vorlage für die eigenen ESPHome-Secrets verwenden.
3. Pins und Kalibrierwerte in `esphome/poolguard.yaml` prüfen.
4. XIAO ESP32-C3 zunächst per USB flashen.
5. `distance_empty_cm` und `distance_full_cm` im eingebauten Zustand kalibrieren.
6. Vor Aktivierung der Pumpenerkennung mehrere Messzyklen mit Pumpe an und aus vergleichen.

## Sicherheit

- Nur eine geschützte, qualitativ gute 18650-Zelle verwenden.
- Akku nicht kurzschließen, quetschen, verpolen oder unbeaufsichtigt laden.
- Elektronik vor Kondenswasser und Spritzwasser schützen.
- Den A02YYUW über MOSFET oder Load-Switch im Deep Sleep vollständig abschalten.
- Pumpenerkennung nur als Hinweis verwenden, niemals als sicherheitsrelevante Abschaltung.

## Unterstützung

<a href="https://buymeacoffee.com/toor0001"><img src="https://img.buymeacoffee.com/button-api/?text=Spendiere%20mir%20einen%20Kaffee&emoji=%E2%98%95&slug=toor0001&button_colour=FFDD00&font_colour=000000&font_family=Lato&outline_colour=000000&coffee_colour=ffffff" alt="Spendiere mir einen Kaffee"></a>

## Lizenz

Software und Dokumentation stehen unter der MIT-Lizenz. Die CAD-Dateien sind aktuell noch Prototypen; vor dem ersten stabilen Release kann dafür eine eigene Hardware-Lizenz ergänzt werden.
