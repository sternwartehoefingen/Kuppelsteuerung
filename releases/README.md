# Releases

Archivierte Firmware-Versionen und Build-Artefakte für Rollback und Versionsverwaltung.

## Struktur

```
releases/
├── Dome_Controller0-160806a.zip    # Version 2016-08-06 (aktuell)
├── (zukünftige Releases)
└── README.md                       # Diese Datei
```

## Release-Naming-Convention

```
<Projektname>-<Version>.zip
```

**Beispiele:**
- `Dome_Controller0-160806a.zip` – Dome Controller, 2016-08-06, Revision A
- `Azimuth_Sensor5-160122a.zip` – Azimuth Sensor, 2016-01-22, Revision A

**Format:**
- Datum: `YYMMDD` (Jahr, Monat, Tag)
- Revision: `a`, `b`, `c`, ... (bei mehreren Releases am selben Tag)

## Aktuelle Version

**Dome_Controller0-160806a.zip** (2016-08-08)

**Enthalten:**
- Alle `.ino` und `.h` Dateien
- Vollständiger Quellcode inkl. IO_Defines.h
- Kompatibel mit Arduino Nano / Pro Micro / Due

**Status:** Extrahiert nach `packages/firmware-main/Dome_Controller0/`

**Changelog:** Siehe [../CHANGELOG.md](../CHANGELOG.md#010--2016-08-06--dome_controller0-160806a)

## Release-Prozess

### 1. Version vorbereiten

```bash
cd packages/firmware-main/Dome_Controller0/

# Versionsnummer in Dome_Controller0.ino aktualisieren
#define Version 20260425
```

### 2. Tests durchführen

- [ ] Kompiliert auf allen Plattformen (Nano/Pro Micro/Due)
- [ ] Serielle Kommandos funktionieren
- [ ] Encoder-Auswertung korrekt
- [ ] Motor-Steuerung (CW/CCW/Stop)
- [ ] Watchdog stoppt Motor nach 5s
- [ ] ASCOM SimpleDome erkennt Encoder-Signal
- [ ] (Optional) Shutter-Funktion

### 3. CHANGELOG.md aktualisieren

```bash
vim ../CHANGELOG.md

# Unreleased-Abschnitt in versionierten Release umbenennen:
## [0.2.0] – 2026-04-25
```

### 4. ZIP-Archiv erstellen

```bash
cd packages/firmware-main/
zip -r ../../releases/Dome_Controller0-$(date +%y%m%d)a.zip Dome_Controller0/
```

**Alternativ (Windows PowerShell):**
```powershell
Compress-Archive -Path packages/firmware-main/Dome_Controller0 -DestinationPath releases/Dome_Controller0-$(Get-Date -Format yyMMdd)a.zip
```

### 5. Git-Tag setzen

```bash
git tag -a v0.2.0 -m "Release 0.2.0 - Bugfixes und Struktur-Refactoring"
git push origin v0.2.0
```

### 6. Release-Notes erstellen (optional)

```markdown
# Release v0.2.0 (2026-04-25)

## Highlights
- Kritischer Shutter-Bug behoben
- Monorepo-Struktur eingeführt
- TECHNICAL.md mit vollständiger Analyse

## Downloads
- [Dome_Controller0-260425a.zip](releases/Dome_Controller0-260425a.zip)

## Breaking Changes
- Keine

## Bekannte Probleme
- Shutter-ASCOM-Integration noch nicht vollständig
```

## Rollback zu älterer Version

```bash
# 1. Gewünschtes Release entpacken
unzip releases/Dome_Controller0-160806a.zip -d packages/firmware-main/

# 2. Mit Arduino IDE öffnen und hochladen
```

## Archiv-Verwaltung

### Aufbewahrung
- **Letzte 3 Versionen:** Immer behalten
- **Major-Releases:** Dauerhaft archivieren
- **Bugfix-Releases:** Nach 6 Monaten optional löschen

### Externe Backups
- Wichtige Releases zusätzlich auf externem Medium (USB-Stick, Cloud)
- Mindestens 1x pro Jahr vollständiges Projekt-Backup

### Größenbeschränkung
- Wenn `.zip`-Dateien >10 MB: In Git-LFS oder externe Archivierung auslagern
- Git-Repository sollte <100 MB für gute Performance bleiben

## Versions-Historie

| Version | Datum | Beschreibung | Größe |
|---|---|---|---|
| `Dome_Controller0-160806a.zip` | 2016-08-08 | Initial Release mit Shutter-Grundgerüst | 28 KB |
| *(zukünftige Versionen)* | - | - | - |
