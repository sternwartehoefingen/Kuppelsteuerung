# Technische Dokumentation – Dome Controller

## Gesamtübersicht

Der Dome Controller ist eine Arduino-basierte Echtzeit-Steuerung für eine astronomische Observatoriumskuppel. Das System erfüllt drei Hauptaufgaben:

1. **Azimut-Messung**: Erfassung der aktuellen Kuppelposition über einen 2452-Impulse/Umdrehung Inkremental-Encoder
2. **Motorsteuerung**: Ansteuerung eines H-Brücken-Moduls für CW/CCW-Drehung mit Watchdog-Überwachung
3. **ASCOM-Schnittstelle**: Synthetisierung eines kompatiblen Encoder-Signals für den SimpleDome ASCOM-Treiber zur automatischen Teleskop-Nachführung

## Systemarchitektur

```
┌─────────────────────────────────────────────────────────────────┐
│                        Arduino Mikrocontroller                   │
│                                                                   │
│  ┌──────────────┐   ┌──────────────┐   ┌──────────────┐        │
│  │ AzEncoder    │   │  AzDrive     │   │  Shutter     │        │
│  │ (Sensor)     │   │  (Motor)     │   │  (Dach)      │        │
│  └──────┬───────┘   └──────┬───────┘   └──────┬───────┘        │
│         │                   │                   │                 │
│         └───────────────────┴───────────────────┘                 │
│                             │                                     │
│                    ┌────────▼────────┐                           │
│                    │  Dome_Controller │                           │
│                    │   (Main Loop)    │                           │
│                    └────────┬─────────┘                           │
│                             │                                     │
│                    ┌────────▼────────┐                           │
│                    │   AzCommand     │                           │
│                    │  (Serial I/O)   │                           │
│                    └─────────────────┘                           │
└─────────────────────────────────────────────────────────────────┘
         │                                    │
         ▼                                    ▼
  Physikalischer                      PC mit ASCOM
     Encoder                        SimpleDome Treiber
  (2452 ppr)                         (Serial 19200 Bd)
```

## Modul-Beschreibungen

### 1. Dome_Controller0.ino (Hauptprogramm)

**Zweck**: Hauptschleife und zentrale Koordination aller Subsysteme.

**setup()-Funktion**:
```
1. Serial-Port initialisieren (19200 Bd)
2. Debug-Pins konfigurieren
3. AzEncoder initialisieren (MOUNTPOSMAX=2452, Auflösung 0,5°)
4. AzDrive initialisieren (Direction-Inversion aktiviert)
5. Shutter initialisieren
6. Watchdog aktivieren (ATmega328: 4s, SAM3X8E: 1,8s)
7. Quadratur-Timer initialisieren (nur Due)
```

**loop()-Funktion** (Echtzeit-Schleife, ~2ms Zykluszeit):
```
1. Encoder-Pins einlesen (AzEncoder_read)
2. Zeitgesteuerte Ausgabe prüfen (TICKTIME=200ms)
3. Bei Änderung oder Request:
   a. Quadratur-Radius auslesen (nur Due)
   b. Encoder-Position aktualisieren
   c. Status seriell ausgeben:
      - Sensor-Zustand ($S...)
      - Encoder-Position (Z...)
      - Synthetisiertes Signal (E...)
      - Azimut-Winkel (A...)
      - Motor-Status (M...)
      - Radius (R..., nur Due)
      - Mount-Position (W...)
4. Motor-Steuerung aktualisieren:
   - DIO-Eingänge lesen (K8055N)
   - Ausgänge setzen (H-Brücke)
5. Serielle Kommandos prüfen
6. Watchdog zurücksetzen
```

**Globale Variablen**:
- `includeDetail`: Vollständige vs. reduzierte Ausgabe
- `doRadiusOutput`: Flag für zeitgesteuerte Radius-Ausgabe
- `enTimeTrig`: Zeitgesteuerte Ausgabe aktiviert
- `enDebugOutput`: Debug-Informationen einblenden
- `enModulo4`: Ausgabe nur bei jedem 4. Encoder-Impuls

---

### 2. AzEncoder (Encoder-Auswertung)

**Zweck**: Auswertung des Quadratur-Encoders und Synthese eines ASCOM-kompatiblen Signals.

#### Funktionsprinzip Quadratur-Dekodierung

Der Inkremental-Encoder liefert zwei um 90° phasenversetzte Rechtecksignale (Phase A, Phase B) und ein Index-Signal (Phase I):

```
Phase A: ──┐   ┌───┐   ┌───┐
           └───┘   └───┘   └───
Phase B: ────┐   ┌───┐   ┌───
             └───┘   └───┘
         CW →→→→→→→→→→→→→→→→
```

**Dekodierungs-Algorithmus** (`update_encoderPos`):

```cpp
if (sensorValA == encoderPinBLast) {
    if (!(encoderPinALast == sensorValB)) {
        encoderPos++;  // Vorwärts
        if (encoderPos >= encoderMaxPos) encoderPos -= encoderMaxPos;
    }
} else {
    if (encoderPinALast == sensorValB) {
        encoderPos--;  // Rückwärts
        if (encoderPos < 0) encoderPos += encoderMaxPos;
    }
}
```

**Logik**: Vergleicht den aktuellen Zustand von Phase A mit dem *letzten* Zustand von Phase B. Damit wird bei jeder Flanke von A oder B die Drehrichtung erkannt und der Zähler entsprechend inkrementiert/dekrementiert.

**Wertebereich**: 0 … 2451 (zyklisch, Modulo 2452)

#### Encoder-Signal-Synthese für ASCOM

ASCOM-Treiber erwarten ein niedrigeres Auflösungs-Signal. Der Controller synthetisiert aus den 2452 nativen Impulsen ein skalierbares Gray-Code-Signal:

**Gray-Code-Tabelle**:
```
Dezimal   Gray   PhA PhB
   0      00      0   0
   1      01      0   1
   2      11      1   1
   3      10      1   0
```

**Synthese-Formel** (Auflösung 0,5°):
```cpp
encGray = Gray[((encoderPos * 4811UL / 8192 + 1) / 2) & 0x03];
```

- Ziel: 720 Flanken/Umdrehung = 0,5° Auflösung
- Skalierung: 2452 → 720 durch Faktor 4811/8192 (Festkomma-Arithmetik)
- Genauigkeit: ±0,003° bzw. ±0,1 mm bei 10 m Kuppelumfang

**Verfügbare Auflösungen**:
| Modus | Flanken/Umdrehung | Winkel-Auflösung | Faktor |
|---|---|---|---|
| `resolutionNative` | 9808 (4× 2452) | ~0,037° | 1:1 |
| `resolution_0d5` | 720 | 0,5° | 4811/8192 |
| `resolution_1d_low` | 360 | 1,0° | 1203/8192 |
| `resolution_1d_high` | 360 | 1,0° | 4811/32768 |

**Hardware-Ausgabe**: Die synthetisierten Gray-Code-Bits werden auf `echoPinA` und `echoPinB` ausgegeben, sodass SimpleDome über die ASCOM-Schnittstelle ein "virtuelles" Encoder-Signal sieht.

---

### 3. AzDrive (Motor-Steuerung)

**Zweck**: Ansteuerung des Kuppel-Motors mit Sicherheits-Watchdog.

#### Eingänge

**Digital Inputs (von Velleman K8055N)**:
- `drivePinEn` (Pin 5): Motor Enable (Low-aktiv)
- `drivePinDir` (Pin 6): Drehrichtung (Low-aktiv)

**Serielle Kommandos**:
- `l`: Links (CCW)
- `r`: Rechts (CW)
- `s`: Stop

#### Ausgänge

- `ctrlPinR` (Pin 7): H-Brücke CW (Rechts)
- `ctrlPinL` (Pin 8): H-Brücke CCW (Links)

#### Steuerlogik

**1. DIO-Eingang lesen** (`AzDrive_CtrlReadDIO`):
```cpp
driveEn  = !digitalRead(drivePinEn);   // negiert
driveDir = !digitalRead(drivePinDir);

int Ctrl = DRV_CW;
if ((bool)driveDir ^ (bool)invDriveDir)
    Ctrl ^= DRVMASK_REV;  // Richtung umkehren

if (!driveEn) Ctrl = DRV_OFF;

driveControl = Ctrl;  // zur Ausgabe weiterleiten
```

**2. Ausgabe mit Totzeit** (`AzDrive_CtrlOutput`):
```cpp
if (driveControlLast != driveControl) {
    int Ctrl = driveControl;
    if (driveControlLast != DRV_OFF)
        Ctrl = DRV_OFF;  // Totzeit: erst beide Relais aus
    
    digitalWrite(ctrlPinR, (Ctrl & DRVMASK_CW));
    digitalWrite(ctrlPinL, (Ctrl & DRVMASK_CCW));
    driveControlLast = Ctrl;
}
```

**Totzeit-Verhalten**: Bei Richtungswechsel wird zuerst der Motor gestoppt, dann in der nächsten Iteration die neue Richtung aktiviert. Dies verhindert Kurzschlüsse in der H-Brücke.

#### Watchdog (Motor-Sicherheits-Timer)

**Zweck**: Stoppt den Motor automatisch, wenn keine Befehle mehr von ASCOM kommen.

```cpp
void DriveWDT(bool DriveOn) {
    if (DriveOn) {
        driveWDTtimeout = millis() + DRV_TIMEOUT_TIMEms;  // 5000 ms
    }
    driveWDTactiv = DriveOn;
}

bool DriveWDTdead(void) {
    bool result = (driveWDTtimeout < millis());
    if (result) {
        driveControl = DRV_OFF;
        driveWDTactiv = false;
    }
    return result;
}
```

- Wird bei jedem `l`, `r`-Kommando zurückgesetzt
- Läuft nach 5 s ohne neues Kommando ab → Motor wird gestoppt
- Verhindert Schäden bei Absturz der ASCOM-Software

---

### 4. AzMounts (Mount-Halter-Positionen)

**Zweck**: Erkennung der 32 radialen Montage-Halter-Positionen für manuelle Teleskop-Ausrichtung.

#### Datenstruktur

```cpp
const int16_t MountPosition[] = {
    51,  123,  209,  282,  354,  439,  512,  584,  669,  741, 
   815,  900,  973, 1061, 1146, 1214, 1280, 1353, 1427, 1511, 
  1583, 1656, 1740, 1813, 1886, 1972, 2044, 2115, 2201, 2274, 
  2346, 2430, 51 + MOUNTPOSMAX
};
```

- 32 Halter-Positionen (gemessen am 2016-01-09)
- Encoder-Werte zwischen 0 und 2451
- Letzter Eintrag ist Wrap-Around (51 + 2452)

#### Erkennungs-Algorithmus

```cpp
mountPosAttr_t CheckMountPosition2(int16_t encoderValue, int16_t fensterbreite) {
    // 1. Offset anwenden und auf [0, MOUNTPOSMAX) normalisieren
    encoderValue -= MountPositionOffset - MountPosition[0];
    if (encoderValue < MountPosition[0])
        encoderValue += MOUNTPOSMAX;
    
    // 2. Binäre Suche durch Vorwärts-/Rückwärts-Iteration
    while (MountPosition[MountIdx] > encoderValue)
        MountIdx--;
    while (MountPosition[MountIdx + 1] <= encoderValue)
        MountIdx++;
    
    // 3. Distanz zu benachbarten Haltern berechnen
    int d1 = encoderValue - MountPosition[MountIdx];
    int d2 = MountPosition[MountIdx + 1] - encoderValue;
    
    if (d2 < d1) MountIdx++;  // näher am nächsten Halter
    
    // 4. Fenster-Check (±5 Impulse = ±3 mm Umfangs-Abstand)
    fensterbreite /= 2;
    bool inVicinity = (d1 <= fensterbreite) || (d2Mod <= fensterbreite);
    
    if (encoderValue == MountPosition[MountIdx])
        return POS_center;   // exakte Position
    else if (inVicinity)
        return POS_vicinity; // in Nähe (*)
    else
        return POS_none;     // zu weit entfernt
}
```

**Ausgabe**:
- `W15`: Halter 15 exakt erreicht
- `*`: In Nähe eines Halters
- ` `: Zwischen Haltern

---

### 5. Dome (Azimut-Berechnung)

**Zweck**: Umrechnung der Encoder-Position in Azimut-Winkel (0–360°).

```cpp
void Dome_showAzimuth(bool details) {
    Serial.print("A");
    SerialPrint_int4((encoderPos * 10 * ((720UL * 32768 / MOUNTPOSMAX + 1) / 2)) / 32768);
    Serial.print(" ");
}
```

**Berechnung**:
```
Azimut [0,1°] = encoderPos × 10 × (720 × 32768 / 2452 + 1) / 2 / 32768
              = encoderPos × 10 × 9.58...
              ≈ encoderPos × 2.936
```

- Ausgabe in Zehntel-Grad (z. B. `A2713` = 271,3°)
- Festkomma-Arithmetik mit 32768er-Skalierung für hohe Genauigkeit

---

### 6. Shutter (Dach-/Spaltsteuerung)

**Zweck**: Ansteuerung eines motorisierten Dach-/Spalt-Systems mit Endschaltern.

#### Hardware-Pins

| Pin | Funktion |
|---|---|
| `roofDriveClose` | Relais Schliessen (Output) |
| `roofDriveOpen` | Relais Öffnen (Output) |
| `roofSwitchClosed` | Endschalter Geschlossen (Input) |
| `roofSwitchOpened` | Endschalter Geöffnet (Input) |

#### Zustandsautomat

```
          ShutterStart(true)
       ┌──────────────────────┐
       │                      │
       ▼                      │
  ┌─────────┐         ┌───────┴────┐
  │ Opening │         │  Stopped   │
  │ (Mot=1) │         │  (Mot=0)   │
  └────┬────┘         └───────┬────┘
       │ ShutterStop()        │
       └──────────────────────┘
                              │
                              │ ShutterStart(false)
                              ▼
                       ┌─────────┐
                       │ Closing │
                       │ (Mot=2) │
                       └─────────┘
```

**Implementierung**:
```cpp
void ShutterStart(bool Oeffnen) {
    if (Oeffnen) {
        digitalWrite(roofDriveClose, MOTOR_Relay_off);
        digitalWrite(roofDriveOpen,  MOTOR_Relay_on);
        ShutterMotorState = 1; // opening
    } else {
        digitalWrite(roofDriveOpen,  MOTOR_Relay_off);
        digitalWrite(roofDriveClose, MOTOR_Relay_on);
        ShutterMotorState = 2; // closing
    }
}

void ShutterStop(void) {
    digitalWrite(roofDriveClose, MOTOR_Relay_off);
    digitalWrite(roofDriveOpen,  MOTOR_Relay_off);
    ShutterMotorState = 0; // stopped
}
```

**Status**: Grundgerüst vorhanden, aber noch nicht in ASCOM-Kommunikation integriert. Für Betrieb fehlen:
- Automatische Endschalter-Abfrage
- Serielle Kommandos (`o`, `c`, `stop`)
- Timeout-Überwachung

---

### 7. Quadratur_TC2 (Hardware-Decoder, nur Arduino Due)

**Zweck**: Hardwarebasierte Quadratur-Dekodierung für Radius-Messung (Teleskop-Entfernung zur Kuppelwand).

#### Timer/Counter 2 Konfiguration

```cpp
REG_TC2_CMR0 = TC_CMR_TCCLKS_XC0 | TC_CMR_ABETRG;
REG_TC2_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA;
REG_TC2_BMR |= TC_BMR_FILTER | TC_BMR_MAXFILT(63);  // Entprellung
if (QUADDIR == CCW) REG_TC2_BMR |= TC_BMR_SWAP;     // Richtung
```

**Hardware-Features**:
- **QDEN**: Quadratur-Dekoder aktiviert
- **POSEN**: Positions-Modus (Zähler inkrementiert/dekrementiert)
- **EDGPHA**: Flanken-Auswertung auf Phase A
- **FILTER**: Hardware-Entprellung mit 63-Takt-Filter
- **SWAP**: Phasen-Vertauschung für CCW-Richtung

**Vorteil**: Kein CPU-Overhead für Encoder-Auswertung, keine verpassten Impulse bei hoher Geschwindigkeit.

**Auslesen**:
```cpp
long QuadTC2_read(void) {
    return REG_TC2_CV0;  // Counter Value Channel 0
}
```

**Reset**:
```cpp
void QuadTC2_reset(void) {
    REG_TC2_CCR0 = TC_CCR_SWTRG;  // Software Trigger = Reset
}
```

---

### 8. AzCommand (Serielle Kommandoschnittstelle)

**Zweck**: Verarbeitung von Single-Character-Kommandos über die serielle Schnittstelle.

#### Kommando-Liste

| Taste | Funktion | Aktion |
|---|---|---|
| `h` | Help | Versionsinformation ausgeben |
| `?` | Dump State | Vollständiger Status mit Details |
| `l` | Drive Left | Motor CCW, Watchdog aktivieren |
| `r` | Drive Right | Motor CW, Watchdog aktivieren |
| `s` | Drive Stop | Motor aus, Watchdog deaktivieren |
| `0`, `z` | Zero Azimuth | Encoder-Position auf 0 setzen |
| `Z` | Zero Radius | Quadratur-Counter zurücksetzen (Due) |
| `D` / `d` | Detail On/Off | Detaillierte Ausgabe ein/aus |
| `T` / `t` | Timed On/Off | Zeitgesteuerte Ausgabe ein/aus |
| `4` | Modulo 4 | Ausgabe nur bei jedem 4. Impuls |
| `X` / `x` | Debug On/Off | Debug-Informationen ein/aus |

**Implementierung**:
```cpp
void Command_Check(void) {
    if (Serial.available() > 0) {
        int incomingByte = Serial.read();
        switch (incomingByte) {
            case AzCmd_DriveLeft:
                driveControl = DRV_CCW;
                DriveWDT(true);  // Watchdog aktivieren
                Serial.println("Drive left ");
                break;
            // ... weitere Cases
        }
    }
}
```

---

## Datenfluss

### Echtzeit-Ausgabe (loop-Zyklus)

```
1. Encoder lesen (2 µs)
   ├─> PhA, PhB, Index einlesen
   └─> Änderungs-Flag setzen

2. Position berechnen (5 µs)
   ├─> Quadratur-Dekodierung
   ├─> Zyklischer Wertebereich [0, 2451]
   └─> Index-Reset prüfen

3. Serielle Ausgabe (17,75 µs @ 19200 Bd für ~30 Zeichen)
   ├─> $S011        Sensor-Zustand
   ├─> Z1848        Encoder-Zählwert
   ├─> E01          Synthetisiertes Signal (Gray)
   ├─> A2713        Azimut [0,1°]
   ├─> M00-         Motor-Status
   ├─> R0068        Radius (nur Due)
   └─> W15          Mount-Position

4. Motor-Steuerung (50 µs mit Hardware-I/O)
   ├─> DIO-Eingänge lesen
   ├─> Watchdog prüfen
   └─> Ausgänge schalten (mit Totzeit)

5. Watchdog zurücksetzen (10 µs)

Gesamt: ~2 ms pro Zyklus
```

### Encoder → ASCOM Signal-Kette

```
Physikalischer      Arduino           Synthetisiertes      SimpleDome
   Encoder         Quadratur-          Gray-Code-Signal     ASCOM
  (2452 ppr)       Dekodierung         (720 Flanken)        Treiber
      │                  │                    │                │
      ├─> PhA ──────────>│                    │                │
      ├─> PhB ──────────>│                    │                │
      └─> Index ────────>│                    │                │
                          │                    │                │
                    encoderPos                 │                │
                    [0..2451]                  │                │
                          │                    │                │
                   Skalierungs-Formel          │                │
                 (4811/8192 Festkomma)         │                │
                          │                    │                │
                    Gray[index]                │                │
                          │                    │                │
                          ├─> echoPinA ───────>│                │
                          └─> echoPinB ───────>│                │
                                                │                │
                                         SimpleDome liest        │
                                         virtuelles Signal       │
                                                └───────────────>│
```

---

## Plattform-Unterschiede

### ATmega328P (Arduino Nano)
- **Watchdog**: `wdt_enable(WDTO_4S)`, manueller Reset per `wdt_reset()`
- **Serial**: Sofort nach `Serial.begin()` verfügbar
- **Quadratur**: Software-Dekodierung (AzEncoder)
- **Flash**: ~30 kB, ausreichend für alle Features

### ATmega32U4 (Arduino Pro Micro)
- **Watchdog**: Wie ATmega328P
- **Serial**: USB-CDC, benötigt `while (!Serial)` im Setup
- **Quadratur**: Software-Dekodierung
- **Flash**: ~28 kB (ATmega32U4), knapp ausreichend

### SAM3X8E (Arduino Due)
- **Watchdog**: Hardware-Timer, `WDT_Enable()` mit 1,8s Periode
- **Serial**: Sofort verfügbar (Native USB)
- **Quadratur**: Hardware-Timer TC2 mit Entprellung und automatischer Richtungserkennung
- **Flash**: 512 kB, mehr als ausreichend
- **RAM**: 96 kB (deutlich mehr als AVR)
- **Vorteil**: Zusätzlicher Radius-Encoder über TC2

---

## Performance-Charakteristiken

### Latenz
- Encoder-Auswertung: < 10 µs
- Serielle Ausgabe: 17,75 µs pro Zeile (30 Zeichen @ 19200 Bd)
- Loop-Zyklus: ~2 ms typisch

### Auflösung
- Native Encoder: 0,037° (2452 × 4 Flanken)
- ASCOM-Signal: 0,5° (720 Flanken)
- Azimut-Ausgabe: 0,1° (über Serial)

### Genauigkeit
- Encoder-Synthese: ±0,003° bzw. ±0,1 mm @ 10 m Umfang
- Positions-Wiederholgenauigkeit: ±1 Impuls (±0,147°, ±4 mm)

---

## Sicherheitsmechanismen

1. **Motor-Watchdog**: 5 s Timeout, verhindert Dauer-Lauf bei ASCOM-Absturz
2. **H-Brücken-Totzeit**: Richtungswechsel nur über Stop-Zustand
3. **Watchdog-Timer**: Arduino-Reset bei Firmware-Freeze (4 s AVR, 1,8 s Due)
4. **Encoder-Overflow-Schutz**: Zyklischer Wertebereich verhindert Integer-Überlauf
5. **Serielle Buffer-Prüfung**: `Serial.available()` vor `Serial.read()`

---

## Kalibrierung und Inbetriebnahme

### 1. Mount-Positionen messen
Kuppel an alle 32 Halter-Positionen fahren, Encoder-Werte notieren:
```cpp
// Manuell per '?' Kommando auslesen, in MountPosition[] Array übertragen
const int16_t MountPosition[] = { 51, 123, 209, ... };
```

### 2. Nullpunkt setzen
Kuppel auf Himmels-Nord ausrichten, `0` senden:
```
→ 0
← Reset azimuth
```

### 3. Richtung prüfen
Kuppel CW drehen, Azimut-Wert muss steigen:
```
← A0000   (Nord)
← A0900   (Ost)
← A1800   (Süd)
← A2700   (West)
```
Falls falsch: `invDriveDir = true` in `AzDrive_setup()` setzen.

### 4. ASCOM-Kalibrierung
SimpleDome Wizard durchführen:
- Dome Home Position festlegen
- Shutter Test durchführen (sobald implementiert)
- Slave Mode testen (Teleskop-Nachführung)

---

## Offene Punkte / TODOs

1. **Goto-Funktion** (`$G00` Kommando): Automatische Positionierung zu Zielwinkel
2. **Shutter-ASCOM-Integration**: Vollständige Einbindung in SimpleDome-Protokoll
3. **Endschalter-Überwachung**: Automatischer Stop bei erreichten Endpositionen
4. **Stromsensor**: Überlast-Erkennung für Motor-Schutz
5. **EEPROM-Konfiguration**: Persist MountPosition[], Kalibrierungsdaten
6. **Logging**: SD-Karten-Logging für Diagnose (nur Due mit SD-Shield)

---

## Zusammenfassung

Der Dome Controller ist ein ausgereiftes, echtzeit-fähiges Embedded-System zur präzisen Azimut-Erfassung und Motor-Steuerung einer Observatoriumskuppel. Die Architektur ist modular, plattform-unabhängig und durch Watchdog-Mechanismen abgesichert. Die Encoder-Signal-Synthese ermöglicht eine nahtlose Integration mit ASCOM-Standard-Treibern bei gleichzeitig hoher nativer Auflösung für lokale Positions-Anzeige und Mount-Erkennung.
