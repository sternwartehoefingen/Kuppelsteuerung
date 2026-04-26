# Absolute Positioning System (Reed Contacts)

## Überblick

Dieses Feature fügt absolute Positions-Referenzierung durch 2 Reed-Kontakte hinzu, die 3 Referenzpositionen erkennen (0°, 120°, 240°).

## Hardware-Setup

### Komponenten
- **2× Reed-Kontakte** (z.B. MK04-1A66B-500W)
- **3× Neodym-Magnete** (10×5mm)
  - Position 1 (0° Nord): 2 Magnete übereinander
  - Position 2 (120°): 1 Magnet
  - Position 3 (240°): 1 Magnet

### Montage

**Reed-Kontakte:**
- An fester Position in der Kuppel montieren
- Vertikal übereinander (Abstand ~2cm)
- Kontakt 1 (MSB) oben, Kontakt 2 (LSB) unten

**Magnete:**
- An der rotierenden Kuppelwand montieren
- Bei 0° (Nord): 2 Magnete übereinander
- Bei 120°: 1 Magnet (oben)
- Bei 240°: 1 Magnet (unten)

### Pin-Belegung

| Arduino Board | Reed 1 | Reed 2 |
|---|---|---|
| Nano/Uno (ATmega328P) | A6 | A7 |
| Pro Micro (ATmega32U4) | 16 | 17 |
| Due (SAM3X8E) | 38 | 40 |

## Funktionsweise

### Zustandscodierung

| Reed 1 | Reed 2 | Position | Encoder-Wert |
|---|---|---|---|
| 1 | 1 | 0° (Nord) | 0 |
| 1 | 0 | 120° | 817 |
| 0 | 1 | 240° | 1634 |
| 0 | 0 | Zwischen Markern | - |

### Synchronisation

1. **Beim Überfahren eines Markers** wird der Encoder-Wert auf den bekannten Referenzwert gesetzt
2. **Toleranzfenster**: ±10 Impulse (~±0,3°)
3. **Drift-Erkennung**: Wenn Abweichung > Toleranz → Warnung

### After Power-Up

- System startet im Zustand `ABS_POS_UNKNOWN`
- Nach Überfahren des ersten Markers → `ABS_POS_SYNCED`
- Maximale "Blind-Fahrt": 120° bis zum nächsten Marker

## Ausgabeformat

```
$S011 Z1848 E01 A2713 M00- R0068 W15 Ref:OK[11](3)
                                     ^^^^^^^^^^^^^^^^
                                     |   |  |   └─ Sync-Zähler
                                     |   |  └───── Reed-Zustand
                                     |   └──────── Status (OK/??/>>)
                                     └──────────── Absolute Positioning
```

**Status-Codes:**
- `?` - Unbekannt (nach Power-Up)
- `OK` - Synchronisiert
- `>>` - Sucht aktiv nach Referenz

**Reed-Zustand:**
- `[11]` - Bei 0° (Nord)
- `[10]` - Bei 120°
- `[01]` - Bei 240°
- `[00]` - Zwischen Markern

## Serielle Kommandos

Keine neuen Kommandos - das System arbeitet vollautomatisch.

## Code-Integration

**Hauptdateien:**
- `AbsolutePosition.h` - Header mit Konstanten und Funktionsdeklarationen
- `AbsolutePosition.ino` - Implementierung
- `IO_Defines.h` - Pin-Definitionen (erweitert)
- `Dome_Controller0.ino` - Integration in setup() und loop()

**Aufrufe:**
```cpp
void setup() {
    // ...
    AbsPos_setup();      // Initialisierung
}

void loop() {
    // ...
    AbsPos_check();      // Prüfung in jeder Iteration
    AbsPos_showState();  // Status-Ausgabe
}
```

## Test ohne Hardware

Für Tests ohne echte Reed-Kontakte kann man in `AbsolutePosition.ino` temporär ändern:

```cpp
uint8_t AbsPos_readReedContacts(void) {
    // Simulator: Gibt feste Werte zurück
    static uint8_t simState = REED_STATE_0;  // Simuliere 0°-Position
    return simState;
}
```

## Vorteile

✅ Kein Homing nach Reset mehr nötig  
✅ Automatische Drift-Korrektur alle 120°  
✅ Encoder-Schlupf wird erkannt  
✅ Redundanz zur reinen Inkremental-Messung  
✅ Günstig (~5€ Material)  

## Zukünftige Erweiterungen

- 3. Reed-Kontakt für 7 Positionen (alle 51°)
- EEPROM: Letzte Position speichern für schnelleres Homing
- Statistik: Drift-Historie loggen
- Warnung bei zu großem Drift (Encoder defekt?)
