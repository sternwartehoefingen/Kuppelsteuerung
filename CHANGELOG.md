# Changelog

Alle relevanten Änderungen an diesem Projekt werden hier dokumentiert.  
Format nach [Keep a Changelog](https://keepachangelog.com/de/1.0.0/).

---

## [Unreleased] – Branch: feature/absolute-positioning

### Added
- **Absolute Positionierung mit Reed-Kontakten** (`AbsolutePosition.h/ino`):
  - 2 Reed-Kontakte erkennen 3 Referenzpositionen (0°, 120°, 240°)
  - Automatische Encoder-Synchronisation beim Überfahren der Magnetpositionen
  - Drift-Erkennung mit Warnung (Toleranz ±10 Impulse = ±0,3°)
  - Encoder-Schlupf wird erkannt und gemeldet
  - Status-Anzeige in serieller Ausgabe: `Ref:OK[11](3)`
- Pin-Definitionen für Reed-Kontakte in `IO_Defines.h`:
  - ATmega328P: A6/A7
  - ATmega32U4: Pin 16/17
  - SAM3X8E: Pin 38/40
- [ABSOLUTE_POSITIONING.md](packages/firmware-main/Dome_Controller0/ABSOLUTE_POSITIONING.md): 
  Vollständige Dokumentation mit Hardware-Setup, Montage-Anleitung, Schaltplan

### Changed
- `Dome_Controller0.ino`: Integration von `AbsPos_setup()`, `AbsPos_check()`, `AbsPos_showState()`
- `AzEncoder.h`: `update_encoderPos()` als extern deklariert für Zugriff durch AbsolutePosition

### Hardware-Anforderungen (optional)
- 2× Reed-Kontakte (z.B. MK04-1A66B-500W)
- 3× Neodym-Magnete 10×5mm
- Kosten: ~5€

### Vorteile
- Kein Homing nach Power-Cycle mehr nötig (max. 120° Suchfahrt)
- Automatische Drift-Korrektur alle 120°
- Redundanz zur reinen Inkremental-Messung

---

## [Unreleased] – 2026-04-26 (main branch)

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
