# Documentation

Vollständige Projekt-Dokumentation für Entwickler, Anwender und Wartung.

## Struktur

### user/
Installations- und Bedienungsanleitungen für Endanwender

**Dateien:**
- [README.md](user/README.md) – Vollständige Benutzerdokumentation (Kopie des Root-README mit erweiterten Details)
- [Installationsanleitung.txt](user/Installationsanleitung.txt) – ASCOM-Treiber-Setup (POTH, SimpleDome, FS2)
- [Readme.txt](user/Readme.txt) – K8055D.DLL Installation (System32/SysWOW64)

**Themen:**
- Arduino-Firmware flashen
- K8055N-Treiber installieren
- ASCOM SimpleDome konfigurieren
- Standort-Koordinaten und Kuppel-Geometrie eintragen
- Serielle Kommandos nutzen

### technical/
Technische Analyse für Entwickler und Code-Reviewer

**Dateien:**
- [TECHNICAL.md](technical/TECHNICAL.md) – Vollständige Funktionsanalyse mit:
  - Systemarchitektur-Diagramm
  - Modul-Beschreibungen (AzEncoder, AzDrive, Shutter, etc.)
  - Quadratur-Dekodierungs-Algorithmus
  - Gray-Code-Synthese-Formeln
  - Encoder → ASCOM Signal-Kette
  - Hardware-Timer TC2 (Arduino Due)
  - Performance-Charakteristiken
  - Sicherheitsmechanismen
  - Kalibrierungs-Prozedur

**Zielgruppe:**
- Firmware-Entwickler
- Wartungs-Personal
- Code-Reviewer
- System-Integratoren

### hardware/
Hardware-Handbücher, Schaltpläne und Datenblätter

**Dateien:**
- [Assembly_manual_k8055n-uk.pdf](hardware/Assembly_manual_k8055n-uk.pdf) – Velleman K8055N Montage-Anleitung und Pinbelegung

**Themen:**
- K8055N DIO-Schnittstelle
- Arduino Pin-Mapping (IO_Defines.h)
- Encoder-Verkabelung (PhotoInterrupter)
- H-Brücken-Modul-Anschluss
- Endschalter-Verkabelung (Shutter)
- LCD-Display (optional)

**Fehlend (TODO):**
- Gesamtschaltplan
- Platinen-Layout
- Foto-Dokumentation Aufbau

---

## Schnellzugriff

| Thema | Dokument |
|---|---|
| **Installation** | [user/Installationsanleitung.txt](user/Installationsanleitung.txt) |
| **Kommandos** | [../README.md#serielle-kommandos-19200-bd](../README.md#serielle-kommandos-19200-bd) |
| **Algorithmen** | [technical/TECHNICAL.md#funktionsprinzip-quadratur-dekodierung](technical/TECHNICAL.md#funktionsprinzip-quadratur-dekodierung) |
| **Hardware** | [hardware/Assembly_manual_k8055n-uk.pdf](hardware/Assembly_manual_k8055n-uk.pdf) |
| **Versionshistorie** | [../CHANGELOG.md](../CHANGELOG.md) |

---

## Dokumentation pflegen

### Neue Version dokumentieren

```bash
# 1. CHANGELOG.md aktualisieren
vim ../CHANGELOG.md
# → [Unreleased] Abschnitt pflegen (Added/Changed/Fixed/Removed)

# 2. Bei größeren Änderungen: TECHNICAL.md erweitern
vim technical/TECHNICAL.md
# → Neue Algorithmen erklären
# → Datenfluss-Diagramme anpassen

# 3. Bei Hardware-Änderungen: IO_Defines.h dokumentieren
vim ../packages/firmware-main/Dome_Controller0/IO_Defines.h
# → Kommentare aktualisieren

# 4. Bei neuen Features: README.md erweitern
vim ../README.md
# → Schnellstart-Anleitung anpassen
```

### Dokumentations-Standards

- **User-Docs:** Schritt-für-Schritt-Anleitungen, Screenshots, klar verständlich
- **Technical-Docs:** Code-Beispiele, Algorithmen-Erklärung, Formeln, Performance-Daten
- **Hardware-Docs:** Schaltpläne, Pinbelegung, Datenblätter, Fotos

### Sprache

- **Primär:** Deutsch (Projekt-Kontext: Sternwarte Hüfingen)
- **Code/Kommentare:** Englisch (Arduino-Standard)
- **TECHNICAL.md:** Deutsch mit englischen Fachbegriffen
