# Measurements

Messdaten, Kalibrierungs-Logs und Testprotokolle aus der Inbetriebnahme und dem Betrieb der Kuppelsteuerung.

## Struktur

Daten sind nach Datum organisiert:

```
measurements/
├── 2016-04-30/         # Initiale Kalibrierungs-Messungen
│   ├── output_2016-04-30_17-55-32.log
│   └── output_2016-04-30_18-20-03.log
└── 2016-05-07/         # Radiusreferenz-Tests
    ├── 2016-05-07.txt
    └── output_2016-05-07_18-01-08.log
```

## Log-Format

### Encoder-Status-Logs (`output_*.log`)

Serielle Ausgaben der Firmware während Test-Fahrten:

```
R0065 
R0066 
$S011 Z1848 E01 A2713 M00- R0068   
```

**Struktur:**
- `R` – Radius-Wert (Quadratur-Encoder, nur Due)
- `$S011` – Sensor-Status (PhaseA=0, PhaseB=1, Index=1)
- `Z1848` – Encoder-Zählwert (0–2451)
- `E01` – Synthetisiertes Gray-Code-Signal für ASCOM
- `A2713` – Azimut in 0,1° (271,3°)
- `M00-` – Motor-Status (Enable=0, Dir=0, State=off)
- `W15` – Mount-Halter-Position (wenn in Nähe)

### Test-Protokolle (`*.txt`)

Manuelle Notizen zu Messungen:

```
Radiusreferenz auf 15, 20 Skt = 3 mm
Armlänge R-Taster: 90 mm
schwergängig bei 1 am Antrieb

Manuelle zügige Drehung
File: output_2016-05-07_18-01-08.log
Manuell CW
Status
```

## Verwendungszweck

### Kalibrierung Mount-Positionen

Die Logs aus 2016-04-30 wurden verwendet, um die 32 radialen Halter-Positionen zu vermessen:

```cpp
// In Dome.h
const int16_t MountPosition[] = {
    51,  123,  209,  282,  354,  439,  512,  584,  669,  741, 
   815,  900,  973, 1061, 1146, 1214, 1280, 1353, 1427, 1511, 
  1583, 1656, 1740, 1813, 1886, 1972, 2044, 2115, 2201, 2274, 
  2346, 2430, 51 + MOUNTPOSMAX
};
```

**Prozedur:**
1. Kuppel manuell zu jedem Halter drehen
2. Encoder-Wert via `?` Kommando auslesen
3. Wert in Array übertragen

### Radius-Kalibrierung (nur Arduino Due)

Die Logs aus 2016-05-07 dokumentieren die Kalibrierung des zweiten Quadratur-Encoders, der den radialen Abstand des Teleskops zur Kuppelwand misst.

**Parameter:**
- Armlänge des Radius-Tasters: 90 mm
- Referenzpunkt: 15 bzw. 20 Skalenteilstriche = 3 mm

## Neue Messungen anlegen

### Log-Datei während Firmware-Test speichern

```bash
# Arduino Serial Monitor Ausgabe in Datei umleiten
# (Manuell oder via Screen/PuTTY)

# Alternativ: Python-Script
python -m serial.tools.miniterm COM3 19200 > measurements/$(date +%Y-%m-%d)/output_$(date +%Y-%m-%d_%H-%M-%S).log
```

### Ordner anlegen

```bash
mkdir measurements/YYYY-MM-DD
echo "Beschreibung des Tests" > measurements/YYYY-MM-DD/README.txt
```

### Was dokumentieren?

- **Testziel** – Was wurde untersucht?
- **Hardware-Konfiguration** – Welches Arduino-Board? K8055N angeschlossen?
- **Firmware-Version** – `#define Version` aus `Dome_Controller0.ino`
- **Auffälligkeiten** – Fehler, unerwartetes Verhalten, schwergängige Stellen
- **Ergebnisse** – Kalibrierwerte, erreichte Genauigkeit

## Datenschutz / Archivierung

- **Sensible Daten:** Keine personenbezogenen Daten in Logs
- **Größe:** Log-Dateien können groß werden (`.gitignore` beachten für >10 MB)
- **Backup:** Wichtige Kalibrierungs-Daten zusätzlich extern sichern
- **Retention:** Alte Logs nach 2 Jahren archivieren (ZIP in `releases/`)
