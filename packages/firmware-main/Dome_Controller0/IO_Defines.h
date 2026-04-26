#ifndef _Az_IO_H_
#define _Az_IO_H_

#if defined(__AVR_ATtiny85__)
#error mcu ATtiny85 not supported
#elif defined (__AVR_ATtiny13__)
#error mcu ATtiny13 not supported

#elif defined (__AVR_ATmega328P__)
#warning mcu ATmega328P selected

#define BOARDINFO "Arduino Nano/Micro"
#include <avr/wdt.h>

#define roofDriveClose  A0  // Output: Ansteuerung Motor Dach/Spalt 
#define roofDriveOpen   A1  // Output: Ansteuerung Motor Dach/Spalt

#define encoderPinA 	A3  // Input: Photointerrupter A
#define encoderPinB 	A4  // Input: Photointerrupter B
#define encoderPinI 	A5  // Input: Zero/Index
#define echoPinA    	 2  // Output: Azimut Encoder A CW/CCW ?
#define echoPinB    	 3  // Output: Azimut Encoder B CW/CCW ?
#define echoPinI    	 4  // Output: Azimut Index Mark

#define drivePinEn  	 5  // Input: Motor On/Off from Velleman K8055N Out1
#define drivePinDir 	 6  // Input: Motor direction from Velleman K8055N Out2
#define ctrlPinR    	 7  // Output: Motor CW run
#define ctrlPinL    	 8  // Output: Motor CCW run

#define cfgPinSYNTH  	 9  // Input: Low=Encoder Synthese für ASCOM
#define cfgPinEnSYNTH   10  // Output: Control level Encoder Synthese (Low=aktiv)

#define roofSwitchClosed  11  // Input: Rückmeldung Endpositon Dach/Spalt
#define roofSwitchOpened   12  // Input: Rückmeldung Endpositon Dach/Spalt

// Absolute positioning reed contacts (2 contacts for 3 reference positions)
#define reedContact1Pin   A6  // Input: Reed contact 1 (MSB)
#define reedContact2Pin   A7  // Input: Reed contact 2 (LSB)

#define debugPin1 13    // 12 in 2016-04...
#define debugPin2 13
//--------------------------------------------------------------------

#elif defined  (__AVR_ATmega32U4__)
#warning mcu ATmega32U4 selected

#define BOARDINFO "Arduino Pro Micro"

#define encoderPinA 	A0  // Input: Photointerrupter A
#define encoderPinB 	A1  // Input: Photointerrupter B
#define encoderPinI 	A2  // Input: Zero/Index
#define echoPinA    	 2  // Output: Azimut Encoder A CW/CCW ?
#define echoPinB    	 3  // Output: Azimut Encoder B CW/CCW ?
#define echoPinI    	 4  // Output: Azimut Index Mark

#define drivePinEn  	 5  // Input: Motor On/Off from Velleman K8055N Out1
#define drivePinDir 	 6  // Input: Motor direction from Velleman K8055N Out2
#define ctrlPinR    	 7  // Output: Motor CW run
#define ctrlPinL    	 8  // Output: Motor CCW run

#define cfgPinSYNTH      9  // Input: Low=Encoder Synthese für ASCOM
#define cfgPinEnSYNTH   10  // Output: Control level Encoder Synthese (Low=aktiv)

// Shutter / roof drive (adapt to actual wiring on Pro Micro)
#define roofDriveClose  A3  // Output: Ansteuerung Motor Dach/Spalt schliessen
#define roofDriveOpen   A4  // Output: Ansteuerung Motor Dach/Spalt oeffnen
#define roofSwitchClosed 14  // Input: Endposition geschlossen
#define roofSwitchOpened 15  // Input: Endposition geoeffnet

// Absolute positioning reed contacts
#define reedContact1Pin  16  // Input: Reed contact 1 (MSB)
#define reedContact2Pin  17  // Input: Reed contact 2 (LSB)

#define debugPin1 12
#define debugPin2 13
//--------------------------------------------------------------------

#elif defined  (__SAM3X8E__)
#warning mcu SAM3X8E selected

#define BOARDINFO "Arduino Due"

/*
 * Inkrementalgeber (Hardware Interface)
 *      1       (2)
 * A - D5       D2 
 * B - D4       D13
 * I - D10      A6
 */

#define encoderPinA   53  // Jumper Kabel f-f wegen geänderter Stiftleistenbelegung
#define encoderPinB   51
#define encoderPinI   49  // Input: Zero/Index
#define echoPinA      50  // Output: Azimut Encoder A CW/CCW ?
#define echoPinB      52  // Output: Azimut Encoder B CW/CCW ?
#define echoPinI      48  // Output: Azimut Index Mark

#define drivePinEn    39  // Input: Motor On/Off from Velleman K8055N Out1
#define drivePinDir   41  // Input: Motor direction from Velleman K8055N Out2
#define ctrlPinR      43  // Output: Motor CW run
#define ctrlPinL      45  // Output: Motor CCW run

//#define encoderPinA  6
//#define encoderPinB  7

#define cfgPinSYNTH     23  // Input: Low=Encoder Synthese für ASCOM
#define cfgPinEnSYNTH   25  // Output: Control level Encoder Synthese (Low=aktiv)

// Shutter / roof drive (adapt to actual wiring on Due)
#define roofDriveClose  30  // Output: Ansteuerung Motor Dach/Spalt schliessen
#define roofDriveOpen   32  // Output: Ansteuerung Motor Dach/Spalt oeffnen
#define roofSwitchClosed 34  // Input: Endposition geschlossen
#define roofSwitchOpened 36  // Input: Endposition geoeffnet

// Absolute positioning reed contacts
#define reedContact1Pin  38  // Input: Reed contact 1 (MSB)
#define reedContact2Pin  40  // Input: Reed contact 2 (LSB)

#define debugPin1 14
#define debugPin2 13

#else
#error unknown mcu selected
#endif

#endif /* _Az_IO_H_ */
