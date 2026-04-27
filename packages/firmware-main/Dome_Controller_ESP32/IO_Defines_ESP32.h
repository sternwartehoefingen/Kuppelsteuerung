// IO_Defines_ESP32.h - ESP32 Pin Assignments
#ifndef _IO_Defines_ESP32_H_
#define _IO_Defines_ESP32_H_

// ==================== ESP32 DevKit V1 Pin Mapping ====================
// ESP32 has 34 GPIOs, some are input-only (34-39), some strapping pins
// Avoid GPIO 0, 2, 12, 15 (boot mode pins)

// Quadrature Encoder (with PCNT peripheral)
#define encoderPinA GPIO_NUM_26   // PCNT Unit 0 Channel 0
#define encoderPinB GPIO_NUM_27   // PCNT Unit 0 Channel 1
#define encoderPinI GPIO_NUM_25   // Index/Home pulse (optional)

// ASCOM Gray-Code Output (synthetisiert)
#define encoderPinOut1 GPIO_NUM_32
#define encoderPinOut2 GPIO_NUM_33

// Motor Driver Control (H-Bridge via GPIO)
// Replaces K8055N Digital Out 1 & 2
#define motorPinCW GPIO_NUM_21    // Motor clockwise
#define motorPinCCW GPIO_NUM_22   // Motor counter-clockwise

// Reed Contacts (Absolute Position)
#define reedContact1Pin GPIO_NUM_34  // Input only, 0° reference
#define reedContact2Pin GPIO_NUM_35  // Input only, 120° reference

// Shutter Control (optional)
#define roofDriveOpen GPIO_NUM_18
#define roofDriveClose GPIO_NUM_19
#define shutterClosedSwitch GPIO_NUM_36  // Input only
#define shutterOpenSwitch GPIO_NUM_39    // Input only

// Rain Sensor (optional)
#define rainSensor GPIO_NUM_23

// Status LED
#define statusLED GPIO_NUM_2  // Built-in LED on most ESP32 boards

// Debug Outputs
#define debugPin1 GPIO_NUM_16
#define debugPin2 GPIO_NUM_17

// I2C for potential sensors (BME280, RTC, etc.)
#define I2C_SDA GPIO_NUM_13
#define I2C_SCL GPIO_NUM_14

// ==================== Hardware Timer Configuration ====================
// ESP32 has 4 hardware timers, can be used for precise encoder timing
#define ENCODER_SAMPLE_TIMER 0     // Timer 0 for encoder sampling
#define MOTOR_CONTROL_TIMER 1      // Timer 1 for motor watchdog

#endif // _IO_Defines_ESP32_H_
