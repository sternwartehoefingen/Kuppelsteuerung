// Dome_Controller_ESP32.ino - Main ESP32 Sketch
// ESP32 ASCOM Alpaca Dome Controller with Web Interface
// Hardware: ESP32 DevKit V1, 2452 ppr encoder, H-bridge motor

#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP32Encoder.h>
#include <Preferences.h>

#include "ESP32_Config.h"
#include "IO_Defines_ESP32.h"

// ==================== Global Variables ====================
ESP32Encoder encoder;
AsyncWebServer server(WEB_SERVER_PORT);
Preferences preferences;

// Dome State
volatile int32_t encoderPos = 0;
int32_t encoderMaxPos = 2452;  // One full rotation
float azimuthDeg = 0.0;
float targetAzimuthDeg = 0.0;
bool isSlewing = false;
bool isConnected = false;

// Motor Control
enum MotorState { MOTOR_STOP, MOTOR_CW, MOTOR_CCW };
MotorState motorState = MOTOR_STOP;
unsigned long motorLastUpdate = 0;
const unsigned long motorWatchdogTimeout = 5000; // 5s

// Absolute Position (Reed Contacts)
bool useAbsolutePosition = true;
int reedContact1LastState = HIGH;
int reedContact2LastState = HIGH;
unsigned long lastSyncMillis = 0;

// ASCOM Alpaca State
uint32_t clientTransactionID = 0;
uint32_t serverTransactionID = 0;

// Web Status Data
struct StatusData {
  float azimuth;
  float targetAzimuth;
  bool slewing;
  bool connected;
  String motorState;
  int32_t encoderValue;
  int reedContact1;
  int reedContact2;
  unsigned long uptime;
  int wifiRSSI;
  int syncCount;
} statusData;

// ==================== Helper Functions ====================
float encoderToAzimuth(int32_t pos) {
  // Wrap position to 0..encoderMaxPos
  while (pos < 0) pos += encoderMaxPos;
  while (pos >= encoderMaxPos) pos -= encoderMaxPos;
  
  return (float)pos * 360.0 / (float)encoderMaxPos;
}

int32_t azimuthToEncoder(float azimuth) {
  // Normalize azimuth to 0..360
  while (azimuth < 0) azimuth += 360.0;
  while (azimuth >= 360.0) azimuth -= 360.0;
  
  return (int32_t)(azimuth * (float)encoderMaxPos / 360.0);
}

void setMotor(MotorState state) {
  motorState = state;
  motorLastUpdate = millis();
  
  switch (state) {
    case MOTOR_CW:
      digitalWrite(motorPinCW, HIGH);
      digitalWrite(motorPinCCW, LOW);
      break;
    case MOTOR_CCW:
      digitalWrite(motorPinCW, LOW);
      digitalWrite(motorPinCCW, HIGH);
      break;
    case MOTOR_STOP:
    default:
      digitalWrite(motorPinCW, LOW);
      digitalWrite(motorPinCCW, LOW);
      break;
  }
}

void checkMotorWatchdog() {
  if (motorState != MOTOR_STOP) {
    if (millis() - motorLastUpdate > motorWatchdogTimeout) {
      setMotor(MOTOR_STOP);
      isSlewing = false;
      if (DEBUG_SERIAL) Serial.println("Motor watchdog timeout!");
    }
  }
}

void updateReedContacts() {
  if (!useAbsolutePosition) return;
  
  int reed1 = digitalRead(reedContact1Pin);
  int reed2 = digitalRead(reedContact2Pin);
  
  // Detect falling edge on reed contact 1 (0° reference)
  if (reedContact1LastState == HIGH && reed1 == LOW) {
    int32_t expectedPos = azimuthToEncoder(0.0);
    int32_t deviation = abs(encoderPos - expectedPos);
    
    if (deviation > 10) {  // Tolerance ±10 counts
      encoder.setCount(expectedPos);
      encoderPos = expectedPos;
      lastSyncMillis = millis();
      statusData.syncCount++;
      if (DEBUG_SERIAL) Serial.printf("Reed sync at 0°: %d\n", encoderPos);
    }
  }
  
  // Detect falling edge on reed contact 2 (120° reference)
  if (reedContact2LastState == HIGH && reed2 == LOW) {
    int32_t expectedPos = azimuthToEncoder(120.0);
    int32_t deviation = abs(encoderPos - expectedPos);
    
    if (deviation > 10) {
      encoder.setCount(expectedPos);
      encoderPos = expectedPos;
      lastSyncMillis = millis();
      statusData.syncCount++;
      if (DEBUG_SERIAL) Serial.printf("Reed sync at 120°: %d\n", encoderPos);
    }
  }
  
  reedContact1LastState = reed1;
  reedContact2LastState = reed2;
}

void updateStatusData() {
  statusData.azimuth = azimuthDeg;
  statusData.targetAzimuth = targetAzimuthDeg;
  statusData.slewing = isSlewing;
  statusData.connected = isConnected;
  statusData.encoderValue = encoderPos;
  statusData.reedContact1 = digitalRead(reedContact1Pin);
  statusData.reedContact2 = digitalRead(reedContact2Pin);
  statusData.uptime = millis() / 1000;
  statusData.wifiRSSI = WiFi.RSSI();
  
  switch (motorState) {
    case MOTOR_CW: statusData.motorState = "CW"; break;
    case MOTOR_CCW: statusData.motorState = "CCW"; break;
    default: statusData.motorState = "STOP"; break;
  }
}

// ==================== ASCOM Alpaca API ====================
String createAlpacaResponse(JsonDocument& doc, uint32_t clientID) {
  doc["ClientTransactionID"] = clientID;
  doc["ServerTransactionID"] = serverTransactionID++;
  doc["ErrorNumber"] = 0;
  doc["ErrorMessage"] = "";
  
  String response;
  serializeJson(doc, response);
  return response;
}

// ==================== Setup ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== ESP32 Dome Controller Starting ===");
  
  // Initialize Pins
  pinMode(motorPinCW, OUTPUT);
  pinMode(motorPinCCW, OUTPUT);
  pinMode(reedContact1Pin, INPUT_PULLUP);
  pinMode(reedContact2Pin, INPUT_PULLUP);
  pinMode(statusLED, OUTPUT);
  setMotor(MOTOR_STOP);
  
  // Initialize Encoder
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachFullQuad(encoderPinA, encoderPinB);
  encoder.setCount(0);
  
  // WiFi Connection
  Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  
  #ifdef USE_STATIC_IP
    WiFi.config(STATIC_IP, GATEWAY, SUBNET, DNS);
  #endif
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    digitalWrite(statusLED, !digitalRead(statusLED));
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
    digitalWrite(statusLED, HIGH);
    
    // Start mDNS
    if (MDNS.begin(HOSTNAME)) {
      Serial.printf("mDNS responder started: http://%s.local\n", HOSTNAME);
      MDNS.addService("http", "tcp", WEB_SERVER_PORT);
      MDNS.addService("alpaca", "tcp", WEB_SERVER_PORT);
    }
  } else {
    Serial.println("\nWiFi connection failed!");
  }
  
  setupWebServer();
  
  Serial.println("=== Setup Complete ===\n");
}

// ==================== Main Loop ====================
void loop() {
  // Update encoder position
  encoderPos = encoder.getCount();
  azimuthDeg = encoderToAzimuth(encoderPos);
  
  // Check reed contacts
  updateReedContacts();
  
  // Motor watchdog
  checkMotorWatchdog();
  
  // Update status
  updateStatusData();
  
  // Blink LED if connected
  if (isConnected) {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 1000) {
      digitalWrite(statusLED, !digitalRead(statusLED));
      lastBlink = millis();
    }
  }
  
  yield();  // Let ESP32 handle WiFi/TCP tasks
}
