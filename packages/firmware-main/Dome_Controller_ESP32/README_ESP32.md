# ESP32 Dome Controller - ASCOM Alpaca Edition

ESP32-basierte Kuppelsteuerung mit WiFi, ASCOM Alpaca REST API und Web-Interface.

## 🚀 Features

- **WiFi-Konnektivität**: Ersetzt K8055N USB-Interface und serielle Verbindung
- **ASCOM Alpaca API**: Standard REST API für astronomische Geräte
- **Web-Interface**: 
  - Seite 1: Grafische Azimut-Anzeige mit Kompass
  - Seite 2: Technische Debug-Informationen
- **Hardware-Encoder**: ESP32 PCNT-Peripheral für präzise Quadratur-Dekodierung
- **Reed-Kontakte**: Absolute Positionierung mit automatischer Synchronisation
- **OTA-Updates**: Firmware-Updates über WiFi (optional)
- **mDNS**: Zugriff über `http://dome-controller.local`

## 📋 Hardware-Anforderungen

### ESP32 Board
- **Empfohlen**: ESP32 DevKit V1 (38-Pin)
- **CPU**: Dual-Core 240 MHz
- **RAM**: 520 KB SRAM
- **Flash**: 4 MB minimum

### Peripherie
- Quadratur-Encoder: 2452 ppr (kompatibel mit Arduino-Version)
- H-Bridge Motor Driver (z.B. L298N, BTS7960)
- Reed-Kontakte: 2× für absolute Positionierung
- Netzteil: 5V/1A für ESP32, separate Versorgung für Motor

## 🔌 Pin-Belegung

### Encoder (PCNT)
| Signal | GPIO | Beschreibung |
|--------|------|--------------|
| A      | 26   | Quadratur Kanal A |
| B      | 27   | Quadratur Kanal B |
| I      | 25   | Index-Puls (optional) |

### Motor-Steuerung
| Signal | GPIO | Beschreibung |
|--------|------|--------------|
| CW     | 21   | Motor im Uhrzeigersinn |
| CCW    | 22   | Motor gegen Uhrzeigersinn |

### Reed-Kontakte (Absolute Position)
| Signal | GPIO | Beschreibung |
|--------|------|--------------|
| Reed 1 | 34   | Referenzpunkt 0° |
| Reed 2 | 35   | Referenzpunkt 120° |

### Status & Debug
| Signal | GPIO | Beschreibung |
|--------|------|--------------|
| LED    | 2    | Eingebaute Status-LED |
| Debug1 | 16   | Debug-Ausgang 1 |
| Debug2 | 17   | Debug-Ausgang 2 |

**Hinweis**: GPIO 34-39 sind nur Eingänge (kein Pullup verfügbar)

## 📦 Installation

### 1. PlatformIO (empfohlen)

```bash
# Repository klonen
cd packages/firmware-main/Dome_Controller_ESP32

# Projekt öffnen in VS Code mit PlatformIO Extension
code .

# Dependencies installieren
pio pkg install

# Kompilieren
pio run

# Hochladen
pio run --target upload

# Serial Monitor
pio device monitor
```

### 2. Arduino IDE

**Bibliotheken installieren**:
- ESP32Encoder (v0.11.6+)
- ESPAsyncWebServer (v1.2.4+)
- AsyncTCP (v1.1.4+)
- ArduinoJson (v7.0.4+)
- Preferences (v2.1.0+)

**Boards Manager**:
- ESP32 Board Package: `https://dl.espressif.com/dl/package_esp32_index.json`
- Board: "ESP32 Dev Module"
- Upload Speed: 921600

**Kompilieren**: Sketch → Upload

## ⚙️ Konfiguration

### WiFi-Einstellungen

Datei: `ESP32_Config.h`

```cpp
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"
```

**Optional: Statische IP**:
```cpp
#define USE_STATIC_IP
#define STATIC_IP IPAddress(192, 168, 1, 100)
#define GATEWAY IPAddress(192, 168, 1, 1)
#define SUBNET IPAddress(255, 255, 255, 0)
```

### ASCOM Alpaca Port

Standard: `11111` (offizieller ASCOM Alpaca Port)

```cpp
#define ALPACA_PORT 11111
```

### Hostname (mDNS)

```cpp
#define HOSTNAME "dome-controller"
```

Zugriff dann über: `http://dome-controller.local`

## 🌐 Web-Interface

### Seite 1: Azimut-Anzeige
**URL**: `http://dome-controller.local/`

**Features**:
- SVG-Kompass mit Himmelsrichtungen
- Rote Nadel: Aktuelle Azimut-Position
- Grüne gestrichelte Linie: Ziel-Azimut
- Status-Leiste: Verbindung, Motor, Slewing
- Info-Panel: Numerische Werte

**Screenshot-Beschreibung**:
```
┌─────────────────────────────────────┐
│  🔭 Sternwarte Hüfingen             │
│     Kuppelsteuerung                 │
│  [Azimut] [Debug]                   │
├─────────────────────────────────────┤
│ Verbindung: ✅  Motor: CW  Slewing: 🔄│
│                                     │
│            N                        │
│        ●---│---●                    │
│      ●     │     ●                  │
│    W ●─────●─────● E               │
│      ●     │     ●                  │
│        ●---│---●                    │
│            S                        │
│                                     │
│    Azimut: 135.2°                   │
│                                     │
│ Aktuell:    135.2°                  │
│ Ziel:       180.0°                  │
│ Encoder:    922                     │
│ Uptime:     1h 23m 45s              │
└─────────────────────────────────────┘
```

### Seite 2: Debug-Informationen
**URL**: `http://dome-controller.local/debug`

**Anzeige**:
- **Verbindung**: ASCOM-Status, WiFi RSSI, Uptime
- **Position**: Azimut, Target, Encoder-Wert
- **Motor**: Status, Slewing, Watchdog
- **Reed-Kontakte**: Status, Sync-Count
- **System-Info**: Firmware-Version, Update-Rate
- **Raw JSON**: Vollständige Status-Daten

**Update-Rate**: 200ms (5 Hz)

## 🔗 ASCOM Alpaca API

### Base URL

```
http://dome-controller.local:80
```

### Management Endpoints

| Endpoint | Methode | Beschreibung |
|----------|---------|--------------|
| `/management/apiversions` | GET | Unterstützte API-Versionen |
| `/management/v1/description` | GET | Server-Beschreibung |
| `/management/v1/configureddevices` | GET | Liste aller Geräte |

### Dome Endpoints (Common)

| Endpoint | Methode | Beschreibung |
|----------|---------|--------------|
| `/api/v1/dome/0/connected` | GET/PUT | Verbindungsstatus |
| `/api/v1/dome/0/description` | GET | Gerätebeschreibung |
| `/api/v1/dome/0/driverinfo` | GET | Treiber-Info |
| `/api/v1/dome/0/driverversion` | GET | Treiber-Version |
| `/api/v1/dome/0/name` | GET | Gerätename |

### Dome-Specific Endpoints

| Endpoint | Methode | Beschreibung |
|----------|---------|--------------|
| `/api/v1/dome/0/azimuth` | GET | Aktuelle Azimut-Position |
| `/api/v1/dome/0/slewing` | GET | Slewing-Status |
| `/api/v1/dome/0/athome` | GET | Home-Position (0°) |
| `/api/v1/dome/0/cansetazimuth` | GET | Kann Azimut setzen? |
| `/api/v1/dome/0/cansyncazimuth` | GET | Kann Azimut synchronisieren? |

### Dome Methods

| Endpoint | Methode | Parameter | Beschreibung |
|----------|---------|-----------|--------------|
| `/api/v1/dome/0/slewtoazimuth` | PUT | `Azimuth` (float) | Zu Azimut fahren |
| `/api/v1/dome/0/abortslew` | PUT | - | Slew abbrechen |
| `/api/v1/dome/0/synctoazimuth` | PUT | `Azimuth` (float) | Position synchronisieren |

### Beispiel-Request (Python)

```python
import requests

BASE_URL = "http://dome-controller.local"

# Verbinden
response = requests.put(f"{BASE_URL}/api/v1/dome/0/connected", 
                        data={"Connected": "true"})
print(response.json())

# Azimut lesen
response = requests.get(f"{BASE_URL}/api/v1/dome/0/azimuth")
data = response.json()
print(f"Azimut: {data['Value']}°")

# Zu 180° fahren
response = requests.put(f"{BASE_URL}/api/v1/dome/0/slewtoazimuth", 
                        data={"Azimuth": "180.0"})

# Slewing-Status prüfen
while True:
    response = requests.get(f"{BASE_URL}/api/v1/dome/0/slewing")
    if not response.json()['Value']:
        break
    time.sleep(0.5)

print("Slew complete!")
```

## 🛠️ Entwicklung & Testing

### Serial Monitor Output

```
=== ESP32 Dome Controller Starting ===
Connecting to WiFi: MyNetwork
..........
WiFi connected! IP: 192.168.1.100
mDNS responder started: http://dome-controller.local
Web server started
=== Setup Complete ===

Reed sync at 0°: 0
ALPACA: Connected = true
ALPACA: Slew to 180.0° (delta: 45.0°)
Motor watchdog timeout!
ALPACA: Slew aborted
```

### Debug-Flags

In `ESP32_Config.h`:

```cpp
#define DEBUG_SERIAL true      // Serial-Ausgaben
#define DEBUG_WIFI true        // WiFi-Debug
#define DEBUG_ALPACA true      // Alpaca API-Debug
```

### Ohne Hardware testen

```cpp
// In Dome_Controller_ESP32.ino setup()
#define SIMULATOR_MODE

#ifdef SIMULATOR_MODE
  // Simuliere Encoder-Bewegung
  static float simAzimuth = 0.0;
  encoderPos = azimuthToEncoder(simAzimuth);
  simAzimuth += 0.5;  // 0.5°/Loop
#endif
```

## 🔧 Fehlerbehebung

### WiFi verbindet nicht

**Lösung**:
1. SSID/Passwort in `ESP32_Config.h` prüfen
2. Serial Monitor checken (115200 baud)
3. WiFi-Netzwerk 2.4 GHz? (ESP32 unterstützt kein 5 GHz)
4. Ausreichend Strom? (USB-Kabel/Netzteil)

### Web-Interface lädt nicht

**Lösung**:
1. IP-Adresse im Serial Monitor prüfen
2. Direkter Zugriff via IP statt mDNS testen
3. Firewall-Einstellungen checken
4. Browser-Cache leeren (Strg+Shift+R)

### Encoder-Werte springen

**Lösung**:
1. Pullup-Widerstände an Encoder-Leitungen (4.7kΩ)
2. Abgeschirmtes Kabel verwenden
3. Kurze Leitungen (<1m)
4. `ESP32Encoder::useInternalWeakPullResistors = UP;` aktivieren

### Motor läuft nicht

**Lösung**:
1. Externe Motor-Stromversorgung prüfen
2. H-Bridge Verkabelung checken
3. GPIO-Levels messen (3.3V High)
4. Watchdog-Timeout erhöhen: `motorWatchdogTimeout = 10000`

### Reed-Kontakte synchronisieren nicht

**Lösung**:
1. Magnet-Abstand zum Reed-Kontakt (<5mm)
2. Pullup-Widerstände an Reed-Pins (10kΩ)
3. Toleranz erhöhen: `if (deviation > 50)`
4. Debug-Output aktivieren: `DEBUG_SERIAL true`

## 📊 Performance

- **Encoder-Sampling**: Hardware PCNT, kein CPU-Overhead
- **Web-Update-Rate**: 500ms (Hauptseite), 200ms (Debug)
- **API-Latenz**: <10ms (lokales Netzwerk)
- **Motor-Watchdog**: 5000ms
- **WiFi-Reconnect**: Automatisch

## 🔒 Sicherheit

- **Keine Authentifizierung**: ASCOM Alpaca Standard (lokales Netzwerk)
- **Motor-Watchdog**: Stoppt bei Kommunikationsausfall
- **Bounds-Checking**: Encoder-Werte werden normalisiert
- **Watchdog-Timer**: ESP32 Hardware-Watchdog aktiv

**Produktiv-Einsatz**:
- Separates WLAN für Sternwarte
- Statische IP-Adresse
- Netzwerk-Firewall
- VPN für Remote-Zugriff

## 📝 Migration von Arduino

### Unterschiede zum Arduino-Code

| Feature | Arduino | ESP32 |
|---------|---------|-------|
| Kommunikation | Serial + K8055N | WiFi + REST API |
| Encoder | Software Interrupt | Hardware PCNT |
| Motor | K8055N Digital Out | Direkte GPIO |
| ASCOM | SimpleDome (serial) | Alpaca (REST) |
| Monitoring | Keine | Web-Interface |

### Vorteile ESP32

- ✅ Keine USB-Kabel, WiFi statt K8055N
- ✅ Moderne ASCOM Alpaca API
- ✅ Web-Interface für Monitoring
- ✅ Mehr RAM (520 KB vs. 2 KB)
- ✅ Hardware-Encoder ohne CPU-Last
- ✅ OTA-Firmware-Updates

### Nachteile ESP32

- ❌ Komplexere Konfiguration (WiFi)
- ❌ 3.3V Logic (5V-Level-Shifter nötig?)
- ❌ Höhere Stromaufnahme (~240 mA)

## 🚀 Nächste Schritte

### Geplante Features

- [ ] **OTA-Updates**: ArduinoOTA-Integration
- [ ] **WiFiManager**: SSID/PW über Captive Portal
- [ ] **Shutter-Steuerung**: Dach öffnen/schließen
- [ ] **Regensensor**: Automatisches Schließen
- [ ] **Temperatursensor**: BME280-Integration
- [ ] **RTC**: Echtzeituhr für Zeitstempel
- [ ] **Logging**: SPIFFS/SD-Card für Event-Log
- [ ] **Watchdog-Erweiterung**: Automatisches Reboot

### Anpassungen

1. **WiFi**: SSID/PW in `ESP32_Config.h`
2. **Pins**: `IO_Defines_ESP32.h` anpassen
3. **Encoder**: `encoderMaxPos` auf exakte ppr setzen
4. **Motor**: CW/CCW-Drehrichtung testen
5. **Reed-Positionen**: 0°/120°/240° Winkel messen

## 📚 Referenzen

- [ASCOM Alpaca API](https://ascom-standards.org/api/)
- [ESP32Encoder Library](https://github.com/madhephaestus/ESP32Encoder)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [ArduinoJson](https://arduinojson.org/)
- [ESP32 PCNT](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html)

## 📄 Lizenz

Wie Hauptprojekt: [Siehe Repository-Root](../../../README.md)

## 👤 Autor

Sternwarte Hüfingen  
ESP32-Port: 2025

---

**Status**: 🚧 In Entwicklung (v1.0.0)
