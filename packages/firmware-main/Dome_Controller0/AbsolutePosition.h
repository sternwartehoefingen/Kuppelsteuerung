// AbsolutePosition.h - Absolute positioning via reed contacts
// 2 reed contacts with 3 magnets allow 3 reference positions (0°, 120°, 240°)
// Position encoding:
//   11 (both HIGH) → 0° (North, 2 magnets)
//   10 (Reed1 HIGH, Reed2 LOW) → 120° (1 magnet)
//   01 (Reed1 LOW, Reed2 HIGH) → 240° (1 magnet)
//   00 → Between reference points

#ifndef _AbsolutePosition_H_
#define _AbsolutePosition_H_

// Reference positions in encoder counts
#define REF_POS_0_DEG     0         // North (0°)
#define REF_POS_120_DEG   817       // 120° = MOUNTPOSMAX / 3
#define REF_POS_240_DEG   1634      // 240° = 2 * MOUNTPOSMAX / 3

// Tolerance window for reference position detection (±10 counts = ±0.3°)
#define REF_TOLERANCE     10

// Reed contact state encoding
#define REED_STATE_NONE   0b00      // Between markers
#define REED_STATE_240    0b01      // 240° position
#define REED_STATE_120    0b10      // 120° position
#define REED_STATE_0      0b11      // 0° (North) position

// Absolute position system state
typedef enum {
  ABS_POS_UNKNOWN,        // No reference found yet (after power-up)
  ABS_POS_SYNCED,         // Successfully synced to reference
  ABS_POS_SEARCHING       // Actively searching for reference
} AbsPosState_t;

// Global variables
extern AbsPosState_t absPosState;
extern uint8_t lastReedState;
extern uint32_t lastSyncTime;

// Function declarations
void AbsPos_setup(void);
void AbsPos_check(void);
uint8_t AbsPos_readReedContacts(void);
bool AbsPos_syncToReference(uint8_t reedState, int16_t currentEncoderPos);
int16_t AbsPos_getReferencePosition(uint8_t reedState);
void AbsPos_showState(bool details);

#endif // _AbsolutePosition_H_
