// AbsolutePosition.ino - Absolute positioning implementation
// Uses 2 reed contacts to detect 3 reference positions for encoder synchronization

#include "./AbsolutePosition.h"
#include "./IO_Defines.h"
#include "./Dome.h"

// Global state variables
AbsPosState_t absPosState = ABS_POS_UNKNOWN;
uint8_t lastReedState = REED_STATE_NONE;
uint32_t lastSyncTime = 0;

// Statistics
uint32_t syncCount = 0;
uint32_t syncErrorCount = 0;

void AbsPos_setup(void)
{
  // Configure reed contact pins as inputs with pullup
  pinMode(reedContact1Pin, INPUT_PULLUP);
  pinMode(reedContact2Pin, INPUT_PULLUP);
  
  absPosState = ABS_POS_UNKNOWN;
  lastReedState = REED_STATE_NONE;
  lastSyncTime = 0;
  syncCount = 0;
  syncErrorCount = 0;
  
  Serial.println("Absolute positioning: initialized");
  Serial.println("Waiting for reference marker...");
}

uint8_t AbsPos_readReedContacts(void)
{
  // Read reed contacts (active LOW due to INPUT_PULLUP)
  // Invert so that triggered = HIGH (1)
  bool reed1 = !digitalRead(reedContact1Pin);
  bool reed2 = !digitalRead(reedContact2Pin);
  
  return (reed1 << 1) | reed2;
}

int16_t AbsPos_getReferencePosition(uint8_t reedState)
{
  switch (reedState) {
    case REED_STATE_0:
      return REF_POS_0_DEG;     // 0° (North)
    case REED_STATE_120:
      return REF_POS_120_DEG;   // 120°
    case REED_STATE_240:
      return REF_POS_240_DEG;   // 240°
    default:
      return -1;  // Invalid state
  }
}

bool AbsPos_syncToReference(uint8_t reedState, int16_t currentEncoderPos)
{
  int16_t refPos = AbsPos_getReferencePosition(reedState);
  
  if (refPos < 0) {
    return false;  // Invalid reed state
  }
  
  // Calculate difference
  int16_t diff = currentEncoderPos - refPos;
  if (diff < 0) diff = -diff;  // Absolute value
  
  // Handle wrap-around at 0/MOUNTPOSMAX boundary
  int16_t wrapDiff = MOUNTPOSMAX - diff;
  if (wrapDiff < diff) diff = wrapDiff;
  
  // Check if we're within tolerance
  if (diff <= REF_TOLERANCE) {
    // Already in sync, no correction needed
    return true;
  }
  
  // Correction needed
  if (absPosState == ABS_POS_UNKNOWN || diff > 100) {
    // Large difference or first sync - apply correction
    encoderPos = refPos;
    absPosState = ABS_POS_SYNCED;
    syncCount++;
    lastSyncTime = millis();
    
    Serial.print("Synced to reference: ");
    Serial.print(refPos);
    Serial.print(" (was: ");
    Serial.print(currentEncoderPos);
    Serial.print(", diff: ");
    Serial.print(diff);
    Serial.println(")");
    
    return true;
  }
  else if (diff > REF_TOLERANCE) {
    // Small drift detected
    syncErrorCount++;
    Serial.print("Position drift detected: ");
    Serial.print(diff);
    Serial.print(" counts at ref ");
    Serial.println(refPos);
    
    // Optional: Apply soft correction (average)
    // encoderPos = (encoderPos + refPos) / 2;
    
    return false;
  }
  
  return true;
}

void AbsPos_check(void)
{
  uint8_t currentReedState = AbsPos_readReedContacts();
  
  // Detect state change (entering a reference marker)
  if (currentReedState != lastReedState && currentReedState != REED_STATE_NONE) {
    // Entering a reference position
    AbsPos_syncToReference(currentReedState, encoderPos);
  }
  
  lastReedState = currentReedState;
}

void AbsPos_showState(bool details)
{
  if (!details) return;
  
  uint8_t reedState = AbsPos_readReedContacts();
  
  Serial.print(" Ref:");
  
  switch (absPosState) {
    case ABS_POS_UNKNOWN:
      Serial.print("?");
      break;
    case ABS_POS_SYNCED:
      Serial.print("OK");
      break;
    case ABS_POS_SEARCHING:
      Serial.print(">>");
      break;
  }
  
  // Show reed contact state
  Serial.print("[");
  Serial.print((reedState & 0b10) ? '1' : '0');
  Serial.print((reedState & 0b01) ? '1' : '0');
  Serial.print("]");
  
  // Show sync count
  if (syncCount > 0) {
    Serial.print("(");
    Serial.print(syncCount);
    Serial.print(")");
  }
  
  Serial.print(" ");
}
