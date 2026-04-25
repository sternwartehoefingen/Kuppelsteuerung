# Changelog

Alle relevanten Änderungen an diesem Projekt werden hier dokumentiert.  
Format nach [Keep a Changelog](https://keepachangelog.com/de/1.0.0/).

---

## [Unreleased] – 2026-04-25

### Added
- Quellcode aus `Dome_Controller0-160806a.zip` in `packages/firmware-main/Dome_Controller0/` extrahiert, damit alle Dateien per Git versionierbar sind.
- Shutter-Pin-Definitionen für ATmega32U4 (Pro Micro) in `IO_Defines.h` ergänzt (`roofDriveClose`, `roofDriveOpen`, `roofSwitchClosed`, `roofSwitchOpened`). Zuvor fehlten diese Definitionen, was zu Compiler-Fehlern geführt hätte.
- Shutter-Pin-Definitionen für SAM3X8E (Arduino Due) in `IO_Defines.h` ergänzt.
- `void DriveWDT(bool DriveOn)` zur öffentlichen Schnittstelle in `AzDrive.h` hinzugefügt (fehlte trotz Nutzung in `AzCommand.ino`).
- `README.md` vollständig neu erstellt mit Projektzweck, Projektstruktur, Installationsanleitung, Konfigurationsübersicht, Kommandoreferenz, Ausgabeformat-Erklärung und Entwicklungsablauf.

### Fixed
- **Kritischer Bug in `Shutter.ino` (`ShutterStart`):** Der `else`-Zweig (Schliessen) war identisch mit dem `if`-Zweig (Öffnen) – beide schalteten ausschliesslich den Öffnen-Relais ein. Der Schliessen-Zweig schaltet nun korrekt `roofDriveOpen` aus und `roofDriveClose` ein.
- **Header/Implementierungs-Mismatch in `AzEncoder.h`:** `AzSignal_synthese` war mit Parameter `bool details` deklariert, aber in `AzEncoder.ino` ohne Parameter implementiert. Deklaration wurde auf `void AzSignal_synthese(void)` korrigiert.
- **Veraltete Deklarationen in `AzDrive.h`:** `AzDrive_readDIO()` und `AzDrive_update()` existierten nicht mehr in der Implementierung (umbenannt in Version 2016-08). Ersetzt durch die tatsächlich vorhandenen Funktionen `AzDrive_CtrlReadDIO()` und `AzDrive_CtrlOutput()`.
- **Zuweisung in Bedingung in `AzDrive.ino`:** `if (result = driveWDTtimeout < millis())` durch explizite Zuweisung vor der Bedingung ersetzt (`result = (...)`, dann `if (result)`), um unbeabsichtigte Seiteneffekte zu vermeiden.
- **Tippfehler `enDebutOutput` → `enDebugOutput`** in `Dome_Controller0.ino`, `AzCommand.ino` und `AzMounts.ino`. Die Variable steuert die Debug-Ausgabe; „Debut" war ein Tippfehler für „Debug".
- **Falscher Kommentar** im `CmdDebug_On`-Case in `AzCommand.ino`: Kommentar lautete „enable time trigger for output", korrekt ist „enable debug output".

### Changed
- `AzEncoder.h`: `zeroDetected` und `enEncoderSynthese` von `int` auf `bool` geändert (konsistent mit tatsächlicher Verwendung als Boolesche Werte).
- `AzEncoder.h`: Veraltete `extern int dbgResolution`-Deklaration entfernt (Variable wurde nirgends definiert oder verwendet).

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
