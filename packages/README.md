# Packages

Dieser Ordner enthält die ausführbaren Komponenten des Projekts.

## Struktur

### firmware-main/
Aktuelle Production-Firmware **Dome_Controller0** (Stand: 2016-08-08)

- Multi-Plattform-Support: Arduino Nano / Pro Micro / Due
- Hardware-Quadratur-Dekoder auf Due (TC2)
- Encoder-Signal-Synthese für ASCOM SimpleDome
- Motor-Watchdog (5s Timeout)
- Shutter-Steuerung (Grundgerüst)

📖 **Technische Details:** [../../documentation/technical/TECHNICAL.md](../../documentation/technical/TECHNICAL.md)

### firmware-legacy/
Ältere Firmware-Version **Azimuth_Sensor5** (Stand: 2016-01-22)

Enthalten im Archiv `Azimuth_Sensor5-160122a_ok.zip`. Diese Version war der Vorgänger von Dome_Controller0, vor der Integration der Shutter-Steuerung.

**Hauptunterschiede zu firmware-main:**
- Keine Shutter-/Dach-Steuerung
- Einfachere Motorsteuerung
- Ursprüngliches IO-Layout

### pc-drivers/
PC-seitige Software für Velleman K8055N USB-Interface

**Dateien:**
- `K8055D.dll` (entpackt aus `.zip`) – Windows-Treiber-DLL
- `K8055D.dll_` – Backup-Kopie
- `K8055N_Demo.exe` – Test-Applikation zur Funktionsprüfung

**Installation:** Siehe [../../documentation/user/Readme.txt](../../documentation/user/Readme.txt)

---

## Entwicklung

### Firmware kompilieren

```bash
# firmware-main öffnen
cd firmware-main/Dome_Controller0/

# Mit Arduino IDE öffnen: Dome_Controller0.ino
# Board auswählen: Tools > Board > Arduino Nano / Pro Micro / Due
# Port auswählen: Tools > Port > COM3 (oder erkannter Port)
# Upload: Sketch > Upload (Ctrl+U)
```

### Serielle Konsole testen

```bash
# Arduino IDE Serial Monitor öffnen (Ctrl+Shift+M)
# Baudrate: 19200
# Eingeben: h  → Versionsinformation
# Eingeben: ?  → Vollständiger Status-Dump
```

### Neue Firmware-Version erstellen

1. Änderungen in `firmware-main/Dome_Controller0/` durchführen
2. Versionsnummer in `Dome_Controller0.ino` anpassen
3. Kompilieren und auf Hardware testen
4. Archiv erstellen: ZIP mit Datums-Suffix
5. In `../../releases/` ablegen
6. `../../CHANGELOG.md` aktualisieren

---

## Plattform-Hinweise

### Arduino Nano (ATmega328P)
- **Flash:** 30 kB (passt knapp)
- **Watchdog:** Software-basiert, `wdt_enable(WDTO_4S)`
- **Encoder:** Software-Dekodierung
- **Serial:** Sofort verfügbar

### Arduino Pro Micro (ATmega32U4)
- **Flash:** 28 kB (sehr knapp, ggf. Features deaktivieren)
- **Watchdog:** Wie Nano
- **Encoder:** Software-Dekodierung
- **Serial:** USB-CDC, benötigt `while (!Serial)` im Setup
- **Shutter-Pins:** Anpassen in `IO_Defines.h` (Platzhalter)

### Arduino Due (SAM3X8E)
- **Flash:** 512 kB (ausreichend)
- **Watchdog:** Hardware-Timer, `WDT_Enable()`
- **Encoder:** Hardware-Quadratur TC2 (Pins D4/D5)
- **Serial:** Native USB, sofort verfügbar
- **Extra:** Radius-Messung über zweiten Quadratur-Encoder
- **Shutter-Pins:** Anpassen in `IO_Defines.h` (Platzhalter)
