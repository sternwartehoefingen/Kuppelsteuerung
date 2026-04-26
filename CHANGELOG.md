# Changelog

Alle relevanten Änderungen an diesem Projekt werden hier dokumentiert.  
Format nach [Keep a Changelog](https://keepachangelog.com/de/1.0.0/).

---

## [Unreleased] – 2026-04-26

### Added
- **ESP32-Port** (`packages/firmware-main/Dome_Controller_ESP32/`):
  - WiFi-Konnektivität ersetzt K8055N USB-Interface
  - ASCOM Alpaca REST API (moderne Alternative zu SimpleDome Serial)
  - Web-Interface Seite 1: Grafische Azimut-Anzeige mit SVG-Kompass
  - Web-Interface Seite 2: Echtzeit Debug-Informationen (200ms Update)
  - Hardware PCNT für Encoder (ESP32-Peripheral, kein CPU-Overhead)
  - Reed-Kontakte mit automatischer Positionssynchronisation (0°, 120°)
  - mDNS: Zugriff über `http://dome-controller.local`
  - Umfassende Dokumentation in [README_ESP32.md](packages/firmware-main/Dome_Controller_ESP32/README_ESP32.md)
- **WiFi-Credentials-Schutz**: `ESP32_Config.h` wird via `.gitignore` ausgeschlossen, Template-Datei `ESP32_Config.h.template` mit Platzhaltern committed

### Added
- Projekt auf Monorepo-Struktur umgestellt:
  - `packages/firmware-main/` – Aktuelle Firmware (Dome_Controller0)
  - `packages/firmware-legacy/` – Ältere Version (Azimuth_Sensor5)
  - `packages/pc-drivers/` – K8055D.dll, Demo-Tool
  - `documentation/` – Strukturierte Dokumentation (user/technical/hardware)
  - `measurements/` – Testdaten aus Inbetriebnahme
  - `releases/` – Archivierte Firmware-Versionen
- Quellcode aus `Dome_Controller0-160806a.zip` in `packages/firmware-main/` extrahiert, damit alle Dateien per Git versionierbar sind.
- Shutter-Pin-Definitionen für ATmega32U4 (Pro Micro) und SAM3X8E (Arduino Due) in `IO_Defines.h` ergänzt.
- `void DriveWDT(bool DriveOn)` zur öffentlichen Schnittstelle in `AzDrive.h` hinzugefügt.
- [TECHNICAL.md](documentation/technical/TECHNICAL.md) erstellt: Vollständige technische Analyse mit Algorithmen-Erklärung, Datenfluss-Diagrammen, Hardware-Konfiguration.

### Fixed
- **Kritischer Bug in `Shutter.ino` (`ShutterStart`):** Der `else`-Zweig (Schließen) war identisch mit dem `if`-Zweig (Öffnen) – beide schalteten ausschliesslich den Öffnen-Relais ein. Der Schließen-Zweig schaltet nun korrekt `roofDriveOpen` aus und `roofDriveClose` ein.
- **Header/Implementierungs-Mismatch in `AzEncoder.h`:** `AzSignal_synthese` war mit Parameter `bool details` deklariert, aber in `AzEncoder.ino` ohne Parameter implementiert. Deklaration korrigiert.
- **Veraltete Deklarationen in `AzDrive.h`:** `AzDrive_readDIO()` und `AzDrive_update()` existierten nicht mehr in der Implementierung (umbenannt in Version 2016-08). Ersetzt durch die tatsächlich vorhandenen Funktionen `AzDrive_CtrlReadDIO()` und `AzDrive_CtrlOutput()`.
- **Zuweisung in Bedingung in `AzDrive.ino`:** `if (result = driveWDTtimeout < millis())` durch explizite Zuweisung vor der Bedingung ersetzt.
- **Tippfehler `enDebutOutput` → `enDebugOutput`** in `Dome_Controller0.ino`, `AzCommand.ino` und `AzMounts.ino`.
- **Falscher Kommentar** im `CmdDebug_On`-Case: Korrektur zu "enable debug output".

### Changed
- `AzEncoder.h`: `zeroDetected` und `enEncoderSynthese` von `int` auf `bool` geändert.
- `AzEncoder.h`: Veraltete `extern int dbgResolution`-Deklaration entfernt.
- README.md neu strukturiert mit Monorepo-Pfaden und Schnellstart-Anleitung.

---

## [0.1.0] – 2016-08-06  *(Dome_Controller0-160806a)*

### Added
- Integration Shutter-/Dachsteuerung (Grundgerüst, IO-Anpassungen).
- Projekt auf `Dome_Controller0` umbenannt, `AZ_IO.h` → `IO_Defines.h`.

### Changed
- Encoder-Synthese und Motorsteuerung stabilisiert.
- Azimutausgabe in 0,1°-Schritten.
- Zeitgesteuerte Radius-Ausgabe nur bei Änderung.

---

## [Azimuth_Sensor5] – 2016-01-22  *(Archiv-Stand)*

- Basis-Azimut-Sensing mit synthetischem Encoder-Signal.
- Motor-Watchdog für ATmega328P.
- Synchronisation bei Zero-Index-Signal.
