# Montage und Kalibrierung

## Projektstatus

PoolGuard befindet sich noch im Prototypenstadium. Zuerst wird ausschließlich die Passform der 5-mm-Schablone geprüft. Der endgültige Body und der Technikdeckel werden erst nach bestätigter Passform veröffentlicht.

## 1. Passschablone testen

1. `cad/stl/PoolGuard_Passschablone_V1.stl` mit geringer Schichthöhe drucken.
2. Stützmaterial ist für die flache Schablone normalerweise nicht erforderlich.
3. Schablone ohne Gewalt in die Fächer 3 und 4 einsetzen.
4. Außenradius, Innenradius und beide geraden Seiten prüfen.
5. Prüfen, ob der Mittelsteg vollständig frei durch die offene Schablone läuft.
6. Abweichungen in Millimetern notieren und möglichst fotografieren.

Noch nichts am Originaldeckel schneiden, bohren oder verkleben.

## 2. Geplanter Einbau des finalen Bodys

Der Body wird später wie ein 60°-Kuchenstück geformt und sitzt direkt in den Fächern 3 und 4. Der Mittelsteg bleibt erhalten. Die Seiten werden mit neutralvernetzendem, für dauerfeuchte Umgebung geeignetem Silikon an den vorhandenen Skimmerstegen befestigt.

Der Body ist nur die dauerhaft eingeklebte Wanne. Der abnehmbare Deckel trägt Batteriehalter, XIAO ESP32-C3 und Kabel. Auf der Wasserseite des Deckels sitzt der A02YYUW.

## 3. Elektronik testen

Vor dem Einbau:

1. ESPHome-Konfiguration mit deaktiviertem Deep Sleep flashen.
2. DS18B20-Adresse und Temperaturwert prüfen.
3. A02-Rohdaten prüfen und mit einem Zollstock vergleichen.
4. Batteriespannung gegen ein Multimeter kalibrieren.
5. A02-Stromabschaltung testen.
6. WLAN-Empfang mit der externen Antenne am realen Einbauort prüfen.

## 4. Wasserstand kalibrieren

PoolGuard misst den Abstand vom Sensor zur Wasseroberfläche. Ein kleinerer Abstand bedeutet einen höheren Wasserstand.

- `distance_empty_cm`: Abstand beim niedrigsten noch zulässigen Wasserstand.
- `distance_full_cm`: Abstand beim höchsten gewünschten Wasserstand.

Die Firmware rechnet den Bereich linear auf 0–100 % um und begrenzt Werte außerhalb des Bereichs.

## 5. Pumpenerkennung kalibrieren

Die Pumpenerkennung ist experimentell. Pro Wachzyklus werden mehrere schnelle Messwerte gesammelt. Die Differenz aus größtem und kleinstem Abstand wird als `Water Surface Motion` veröffentlicht.

Vorgehen:

1. Mehrere Zyklen mit ausgeschalteter Pumpe aufzeichnen.
2. Mehrere Zyklen mit laufender Pumpe aufzeichnen.
3. Einen Schwellwert wählen, der beide Gruppen möglichst zuverlässig trennt.
4. Den Wert `pump_motion_threshold_cm` in der YAML anpassen.

Schaum, Wind, Badebetrieb, Regen oder schwimmende Gegenstände können die Erkennung verfälschen. Der Status darf nicht für sicherheitskritische Steuerungen verwendet werden.

## 6. Abdichtung

- Kabeldurchführungen mit Zugentlastung ausführen.
- Elektronik nicht direkt mit Silikon übergießen.
- Vor dem endgültigen Verschließen einen Kondensationstest durchführen.
- Deckeldichtung regelmäßig kontrollieren.
- Nur kunststoffverträgliche, neutralvernetzende Dichtstoffe verwenden.
