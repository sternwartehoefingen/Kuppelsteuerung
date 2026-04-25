/*  Project:
    Task:   Motor control
*/

#include "./AzDrive.h"

int driveEn;
int driveDir;
int driveControlLast = DRV_OFF;
int driveControl;
int driveControlDIO;

bool driveWDTactiv = false;
uint32_t driveWDTtimeout;

bool invDriveDir = false;

void AzDrive_setup(bool DriveDirInverse)
{
  pinMode(drivePinEn,  INPUT_PULLUP);
  pinMode(drivePinDir, INPUT_PULLUP);
  pinMode(ctrlPinR, OUTPUT);
  pinMode(ctrlPinL, OUTPUT);

  delayMicroseconds(50);
#ifdef __AVR_ATmega328P__
  digitalWrite(debugPin1, HIGH);    // Mark
#endif
  digitalWrite(ctrlPinR, LOW);    // switch off
  digitalWrite(ctrlPinL, LOW);    // switch off

#ifdef __AVR_ATmega328P__
  delayMicroseconds(20);
  digitalWrite(debugPin1, LOW);    //debug Mark
#endif

  invDriveDir = DriveDirInverse;
}

void AzDrive_CtrlReadDIO(void)
{
#define DRVMASK_REV (DRV_CW | DRV_CCW)

  static int lastCtrl = DRV_OFF;

  driveEn  = !digitalRead(drivePinEn);   // read drive control Lines, negative
  driveDir = !digitalRead(drivePinDir);

  int Ctrl = DRV_CW;
  if ((bool)driveDir ^ (bool)invDriveDir)
    Ctrl ^= DRVMASK_REV;

  if (!driveEn)
    Ctrl = DRV_OFF;

  if (Ctrl != lastCtrl)
  {
    driveControl = Ctrl;  // forward to output control
    lastCtrl = Ctrl;      // remember
  }
}

void AzDrive_CtrlOutput(void)
{
#define DRVMASK_CW  DRV_CW
#define DRVMASK_CCW DRV_CCW

  digitalWrite(debugPin1, HIGH);    // Mark

  if (driveControlLast != driveControl)
  {

    int Ctrl = driveControl;
    if (driveControlLast != DRV_OFF)
      Ctrl = DRV_OFF;                 // Totzeit zwischen Umschaltungen

    digitalWrite(ctrlPinR, (Ctrl & DRVMASK_CW));
    digitalWrite(ctrlPinL, (Ctrl & DRVMASK_CCW));

    digitalWrite(debugPin1, LOW);    // Mark
    driveControlLast = Ctrl;

    Serial.print(" M: ");
    if (Ctrl & DRVMASK_CW)
      Serial.print("CW");
    if (Ctrl & DRVMASK_CCW)
      Serial.print("CCW");
    Serial.println();
  }
}

void AzDrive_showState(bool details)
{
  const char DriveStateChar[4] = {'-', 'R', 'L', '?'};

  if (details) {
    Serial.print("M");
    Serial.print(driveEn);
    Serial.print(driveDir);
    Serial.print(DriveStateChar[driveControlLast]);
    Serial.print(" ");
  }
}

void DriveWDT(bool DriveOn)
{
  if (DriveOn) {
    driveWDTtimeout = millis() + DRV_TIMEOUT_TIMEms;
  }
  driveWDTactiv = DriveOn;
}

bool DriveWDTdead(void)
{
  bool result = (driveWDTtimeout < millis());
  if (result)
  {
    driveControl  = DRV_OFF;
    driveWDTactiv = false;
  }
  return result;
}

