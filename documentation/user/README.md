# Kuppelsteuerung

Arduino-basierte Azimut-Sensorik und Motorsteuerung für die Observatoriumskuppel der Sternwarte Höfingen.

## Projektzweck

Das System misst die Azimutposition der Kuppel über einen Inkremental-Encoder und steuert den Kuppelantrieb. Es kommuniziert mit einem PC über RS-232/USB und ist in die ASCOM-Treiberinfrastruktur (SimpleDome) eingebunden, die eine automatische Nachführung der Kuppel durch den Teleskop-Mount ermöglicht.

**Hardwarekomponenten:**

| Komponente | Beschreibung |
|---|---|
| Arduino Nano / Pro Micro / Due | Mikrocontroller (wählbar per Compiler-Target) |
| Velleman K8055N | USB-Interface-Karte (Motor Enable/Direction) |
| Inkremental-Encoder (Photointerrupter) | Azimut-Messung (2452 Impulse/Umdrehung) |
| H-Brücken-Modul | Kuppelantrieb CW/CCW |
| Dach-Endschalter | Shutter-Positions-Rückmeldung |
| LCD 16×2 HD44780 | Optionale Vor-Ort-Anzeige |

## Projektstruktur

```
Kuppelsteuerung/
├── packages/
│   ├── firmware-main/          # Aktuelle Firmware (Dome_Controller0)
│   │   └── Dome_Controller0/
│   │       ├── Dome_Controller0.ino     # Hauptsketch (setup / loop)
│   │       ├── IO_Defines.h             # Pin-Mapping je MCU-Plattform
│   │       ├── AzEncoder.ino/.h         # Encoder-Auswertung, Signal-Synthese
│   │       ├── AzDrive.ino/.h           # Motorsteuerung (WDT, DIO)
│   │       ├── AzCommand.ino/.h         # Serielle Kommandoschnittstelle
│   │       ├── AzMounts.ino/.h          # Mount-Halter-Positionen
│   │       ├── Dome.ino/.h              # Azimut-Berechnung
│   │       ├── Shutter.ino/.h           # Dach-/Spalt-Steuerung
│   │       ├── Quadratur_TC2.ino/.h     # Hardware-Quadraturdekoder (Due)
│   │       └── Az_Global.h              # Globale Konstanten
│   ├── firmware-legacy/        # Ältere Version (Azimuth_Sensor5)
│   └── pc-drivers/             # K8055D.dll, Demo-Tool
├── documentation/
│   ├── user/                   # Diese Datei, Installationsanleitungen
│   ├── technical/              # TECHNICAL.md (Funktionsanalyse)
│   └── hardware/               # Hardware-Handbücher, PDFs
├── measurements/               # Testdaten aus Inbetriebnahme
└── releases/                   # Archivierte Firmware-Versionen
```

## Installation

### Arduino-Firmware

1. **Arduino IDE** (≥ 1.6) öffnen.
2. Sketch `packages/firmware-main/Dome_Controller0/Dome_Controller0.ino` öffnen.
3. Ziel-Board auswählen:
   - **Arduino Nano** oder **Uno** → `ATmega328P`
   - **Arduino Pro Micro** (SparkFun/Clone) → `ATmega32U4`
   - **Arduino Due** → `SAM3X8E`
4. COM-Port wählen und hochladen.

### K8055N DLL (PC-seitig)

- `K8055D.DLL` aus dem Archiv in `packages/pc-drivers/K8055D.dll.zip` entpacken.
- 32-Bit-Windows: in `C:\Windows\System32` kopieren.
- 64-Bit-Windows: in `C:\Windows\SysWOW64` kopieren.

### ASCOM-Treiber

Reihenfolge der Installation (Details in `documentation/user/Installationsanleitung.txt`):

1. `AdvancedLX200 Setup 6.2.0.9.exe` (FS2-Treiber) installieren.
2. `SimpleDome Setup.exe` **als Administrator** installieren.
3. ASCOM POTH konfigurieren:
   - Dome: **SimpleDome**, Port **COM3** (CH340, ggf. im Geräte-Manager anpassen)
   - Standort: N 48°49,162′ / E 009°00,325′, 410 m ü. NN
   - Geometrie: Scope +N/−S 65 mm, Kuppelradius 1,95 m, GEM-Achsversatz 440 mm
   - Slave Precision: 2°, Slave Frequency: 5 s

## Konfiguration

Relevante Konstanten in den Header-Dateien (in `packages/firmware-main/Dome_Controller0/`):

| Datei | Konstante | Bedeutung |
|---|---|---|
| `Dome.h` | `MOUNTPOSMAX` | Encoder-Impulse pro Umdrehung (2 × 1226 = 2452) |
| `Dome.h` | `MOUNTOFFSET` | Nullpunkt-Offset des Encoders |
| `Dome.h` | `MountPosition[]` | Encoder-Werte der 32 radialen Halter-Positionen |
| `AzMounts.h` | `MountHWidth` | Halbe Fensterbreite für Halter-Erkennung (±5 Impulse) |
| `Az_Global.h` | `TICKTIME` | Zeitgesteuertes Ausgabe-Intervall (200 ms) |
| `Az_Global.h` | `DEADTIME` | Sperrzeit nach manueller Ausgabe (1000 ms) |
| `AzDrive.h` | `DRV_TIMEOUT_TIMEms` | Motor-Watchdog-Timeout (5000 ms) |
| `Dome_Controller0.ino` | `ENCODER_SYNTH` | Aktiviert synthetisches Encoder-Signal für ASCOM |

## Serielle Kommandoschnittstelle

Baudrate: **19200 Bd**

| Taste | Funktion |
|---|---|
| `l` | Motor CW (links) |
| `r` | Motor CCW (rechts) |
| `s` | Motor stopp |
| `0` / `z` | Azimut-Nullpunkt setzen |
| `Z` | Radius-Referenz zurücksetzen (Due) |
| `h` | Hilfe / Versionsinfo |
| `?` | Vollständiger Status-Dump |
| `D` / `d` | Detailausgabe ein / aus |
| `T` / `t` | Zeitgesteuerte Ausgabe ein / aus |
| `4` | Modulo-4-Ausgabe (reduziert) |
| `X` / `x` | Debug-Ausgabe ein / aus |

## Ausgabeformat

Beispielzeile:
```
$S011 Z1848 E01 A2713 M00- R0068
```

| Feld | Bedeutung |
|---|---|
| `$S011` | Sensor-Zustand: PhA=0, PhB=1, Index=1 |
| `Z1848` | Encoder-Zählwert (0 … MOUNTPOSMAX−1) |
| `E01` | Synthetisiertes ASCOM-Encoder-Signal (Gray-Code) |
| `A2713` | Azimut in 0,1°-Schritten (= 271,3°) |
| `M00-` | Motor-Status: En=0, Dir=0, Zustand=– |
| `R0068` | Radius-Messung (Quadraturencoder, nur Due) |

## Entwicklungsablauf

1. Änderungen im Verzeichnis `packages/firmware-main/Dome_Controller0/` vornehmen.
2. Mit Arduino IDE kompilieren und auf das Ziel-Board hochladen.
3. Serielle Konsole (19200 Bd) öffnen, `h` eingeben → Versionsinfo prüfen.
4. `?` eingeben → vollständigen Status prüfen.
5. Für Details siehe `../../technical/TECHNICAL.md`.

## Bekannte Besonderheiten

- **ATmega32U4 (Pro Micro):** Die USB-Serielle Schnittstelle benötigt `while (!Serial)` im Setup (bereits implementiert). Der serielle Monitor funktioniert erst nach Verbindung.
- **Arduino Due:** Hardware-Quadraturdekoder auf TC2 (Pins D4/D5). Watchdog-Reset über `WDT_Restart(WDT)`.
- **Shutter-Steuerung:** Grunder­gerüst vorhanden, aber noch nicht vollständig in die ASCOM-Kommunikation eingebunden.
- **Synthetisches Encoder-Signal:** Über Jumper auf Pin D9/D10 wählbar. Standard: 0,5°-Auflösung (720 Flanken/Umdrehung).
- **Motor-Watchdog:** Stoppt den Motor automatisch nach 5 s ohne erneuerte Freigabe über Serial-Kommando.
