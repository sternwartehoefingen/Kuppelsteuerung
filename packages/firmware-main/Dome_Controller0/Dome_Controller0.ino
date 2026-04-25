/*
  Drehgeberauswertung
  Prozessor/Board: Arduino Due,
                 Micro (Leonardo)
                 Pro Micro (USB Nano, Select: Nano, ATmega328)

  Bedienung: Drehgeber
  Anzeige:   LCD 16x2 HD44780 (optional)

  2014-07-13 Rainer Willkomm

  2015-09-08 RW Grundversion (Fork von Dimmer)
  2015-09-10 RW Anpassung an Encoder mit Endlosdrehung (Counts zyklisch MAXPOS),
              Serielle Ausgabe nur bei Änderungen
  2015-09-12 RW Array mit Positionen der Fixpunkte für Anzeige
  2015-09-15 RW Anzeige der Fixpunkteannäherung
  2015-09-23 RW Auswertung/Anzeige HW-Inkrementalencoder hinzugefügt
  2015-09-25 RW Code für HW-Encoder wird nur bei Arduino Due (SAM3X8E) aktiviert
  2015-09-26 RW Reset Eingang
  2015-10-18 RW Drive control pins for transcoding En/Dir to L/R (echo only yet)
  2015-10-19 RW Watch dog for ATmega328
  2015-10-25 RW release Pin-out, FU interface ok, YE=CW(Rechts, FU Rev), GN=CCW(Links,FU Fwd)
  2015-10-27 RW Nullstellung über Index-Signal
  2015-10-28 RW Kalibrierung Drehrichtung mit LesveDome ASCOM Treiber
  2015-11-01 RW Synthetisiertes Encodersignal für ASCOM (define ENCODER_SYNTH, Jumper D9-D10), Faktor 1:2
  2015-11-02 RW Synthetisiertes Encodersignal für ASCOM (define ENCODER_SYNTH, Jumper D9-D10), Faktor 360/2452, i.e. 1° Schritte
  2015-11-02 RW 20151208 Synthetisiertes Encodersignal für ASCOM  usint 0.5 deg resolution
  2015-12-10 RW 20151210 Mount holder identification corrected
  2015-12-16 RW Neustrukturierung Quellcode
  2015-12-17 RW Azimuthanzeige
  2015-12-18 RW Motorsteuerung DIO
  2015-12-19 RW 20151219 Radiusmessung mit Quadraturencoder auf Arduino Due
  2015-12-20 RW 20151220 Motorsteuerung über Kommando und DIO
  2015-12-20 RW 20151221 Anzeige Details an-/abschaltbar
  2015-12-22 RW 20151222 Ausgabe mit nachfolgendem Leerzeichen
  2016-01-09 RW 20160109 Input Pullup für AzEncoder AB konfigurierbar (AzEncoderINPUTMODE)
  2016-01-17 RW 20160117 Synchronisation bei Zero entfernt, keine automatische Az-Ausgabe
  2016-01-22 RW 20160122 Watchdog für Motor (DRV_TIMEOUT_TIMEms)
  2016-04-03 RW 20160403 Flag für reduzierte Ausgabe (Modulo 4, enModulo4)
  2016-04-24 RW 20160424 Modulo 4 Ausgabe mit Winkelpositionen, zeitgesteuerte Radiusausgabe
  2016-04-29 RW 20160429 zeitgesteuerte Radiusausgabe nur bei Änderung
  2016-08-08 RW Integration Shutter-/Dachsteuerung (Grundgerüst,IO-Anpassungen)
  2016-08-08 RW Änderung Projekt auf Dome_Control0, AZ_IO.h -> IO_Defines.h

  2Do Goto Kommando implementieren ($G00)

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
  LCD RS pin to digital pin 12
  LCD Enable pin to digital pin 11
  LCD D4 pin to digital pin 5
  LCD D5 pin to digital pin 4
  LCD D6 pin to digital pin 3
  LCD D7 pin to digital pin 2
  LCD R/W pin to ground
  10K resistor:
  ends to +5V and ground
  wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

#define PROJECT "Dome Drive Interface (Azimuth Sensor)"
#define Version 20160808

#define LEADINGZEROS
#define ENCODER_SYNTH

#define OUT_NONE 0x00
#define OUT_AZ   0b00000001
#define OUT_R    0b00000010

#define INVALID_POSITION 9999L  // Sentinel value for uninitialized position

#include "./Az_Global.h"
#include "./IO_Defines.h"
#include "./AzEncoder.h"
#include "./AzMounts.h"
#include "./AzDrive.h"
#include "./AzCommand.h"
#include "./Dome.h"
#include "./Shutter.h"
#include "./Quadratur_TC2.h"

char buf[10];
bool includeDetail = true;
bool doRadiusOutput = false;
bool enTimeTrig = false;
bool enDebugOutput = false;
bool enModulo4 = false;
unsigned long nextTick = 0;
unsigned long tick;

void setup() {

  Serial.begin (19200);

#ifdef __AVR_ATmega32U4__
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
#endif

  sayHello();

  //-- debug_setup ----------
  pinMode (debugPin1, OUTPUT);
  pinMode (debugPin2, OUTPUT);
  digitalWrite(debugPin1, LOW);
  digitalWrite(debugPin2, LOW);
  //-- debug_setup end -------

  markIO();
  AzEncoder_setup(MOUNTPOSMAX, resolution_0d5);
  markIO();
  AzDrive_setup(true);
  markIO();
  Shutter_setup();
  markIO();

  AzEncoder_read();
  PinsChanged = true;

#ifdef __AVR_ATmega328P__
  wdt_enable(WDTO_4S);
#endif

#ifdef __SAM3X8E__  // Arduino Due only
  QuadTC2_setup();
  wdt_initialize();
#endif
} // -- setup end -----------------------------

void loop() {

  long Quad_Position = 0;  // Initialize to prevent undefined behavior
  static long Quad_PositionLast = INVALID_POSITION;

  digitalWrite(debugPin2, HIGH);
  AzEncoder_read();

  if (enTimeTrig) {
    tick = millis();
    if (tick > nextTick) {
      nextTick = tick + TICKTIME;
      doRadiusOutput = true;
    }
  }

  if (PinsChanged || infoRequest || doRadiusOutput)      // Ausgabe automatisch
    //  if (infoRequest)                  // Ausgabe nur auf Anforderung
  {
    bool doOutput = !doRadiusOutput;                  // default

    Quad_Position = QuadTC2_read();
    update_encoderPos();

    //    OutputMode = OUT_NONE;

    if (enModulo4 && !doRadiusOutput)
      doOutput = (encoderPos & 0x0003) == 0;

    if (doOutput) {
      showState_AzSensor(includeDetail);

      if (includeDetail) {
        Serial.print("Z");
        SerialPrint_int4(encoderPos);
        Serial.print(" ");
      }
    }
#ifdef ENCODER_SYNTH
    enEncoderSynthese = digitalRead(cfgPinSYNTH);
    if (enEncoderSynthese)
    {
      AzSignal_synthese();
      if (doOutput) showState_AzSynthese(includeDetail);
    }
    else
      AzSignal_echo();
#else
    AzSignal_echo();
#endif

    if (doOutput) {
      Dome_showAzimuth(true);
      AzDrive_showState(includeDetail);
      Shutter_ShowState();
    }

#ifdef __SAM3X8E__  // Arduino Due only
    if ((doOutput && includeDetail) || (doRadiusOutput && (Quad_Position != Quad_PositionLast))) {
      Serial.print("R");
      SerialPrint_int4(Quad_Position);
      Serial.print(" ");
      Quad_PositionLast = Quad_Position;
    }
#endif

    if (doOutput && includeDetail) {
      if (enModulo4)
        Mounts_showNr();
      else
        Mounts_showState();
    }

    if (doOutput || (doRadiusOutput && (Quad_Position != Quad_PositionLast)) ) {
      Serial.println();
    }

    if (doOutput) {
      nextTick = millis() + DEADTIME;
    }

    if (infoRequest) {
      includeDetail = includeDetail_bak;  // restore
      infoRequest = false;    // done
    }
    doRadiusOutput = false; // done
  }

  // Drive
  // controlled by driveControl via serial command and digital input
  AzDrive_CtrlReadDIO();
  AzDrive_CtrlOutput();

  Command_Check();
  digitalWrite(debugPin2, LOW);
  delay(2);
  /*
    delay(20);
    Serial.print("x_WDT:");Serial.println( (driveWDTactiv ? "On" : "Off"));
  */
  if (driveWDTactiv)
  {
    if (DriveWDTdead())
      Serial.println("stopped by Drive WDT");
  }

#ifdef __AVR_ATmega328P__
  wdt_reset();
#endif

#ifdef __SAM3X8E__  // Arduino Due only
  WDT_Restart( WDT );           // Kicking the Dog
#endif

} // -- loop end -----------------------------

void markIO (void)
{
  digitalWrite(debugPin1, HIGH);    // debug Mark
  delayMicroseconds(200);
  digitalWrite(debugPin1, LOW);
}

void sayHello(void)
{
  Serial.println(PROJECT);
  Serial.print(BOARDINFO);
  Serial.print(" ");
  Serial.println(Version);
  Serial.println();
}

void SerialPrint_int4 (int value)
{
#ifdef LEADINGZEROS
  //-- uses 1550 byte of flash --------------
  snprintf (buf, sizeof(buf), "%04d", value);   // int mit insgesamt 4 Stellen mit führenden Nullen ; sonst "%04d" ändern
  Serial.print(buf);
  //-----------------------------------------
#else
  Serial.print(value);
#endif
}

#ifdef __SAM3X8E__  // Arduino Due only
// Änderung Arduino Bibliothek in Arduino IDE
// http://forum.arduino.cc/index.php?topic=233175.0

void wdt_initialize(void)
{
  //  wdt_enable(WDTO_1S);
  // Variable wdp_ms hold the periode in 256 th of seconds of watchdog
  // It must be greater than 3 et less or equal to 4096
  // The following value set a periode of 1,8 seconds (256 x 1,8 = 461)
  uint32_t wdp_ms = 461 ;
  WDT_Enable( WDT, 0x2000 | wdp_ms | ( wdp_ms << 16 ));
}
#endif

