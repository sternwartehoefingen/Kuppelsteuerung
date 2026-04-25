#include "./Dome.h"
#include "./AzMounts.h"

int16_t MountPositionOffset = MOUNTOFFSET;
uint8_t MountIdx = 0;
mountPosAttr_t mountPosAttr = POS_none;

// mountPosAttr_t CheckMountPosition (int16_t encoderValue, fensterbreite)
/*
  mountPosAttr_t CheckMountPosition (int16_t encoderValue)
  {
  return CheckMountPosition2 (encoderValue, MountHWidth * 2 + 1);
  }
*/

mountPosAttr_t CheckMountPosition2 (int16_t encoderValue, int16_t fensterbreite)
// return
{
  mountPosAttr_t result = POS_none;
  encoderValue -= MountPositionOffset - MountPosition[0];
  if (encoderValue < MountPosition[0])
    encoderValue += MOUNTPOSMAX;

  /* -----
    Serial.print(" : ");
    Serial.print(MountIdx);
    Serial.print(" ");
    Serial.print(MountPosition[MountIdx]);
    Serial.print("<");
    Serial.print(encoderValue);
    Serial.print("<");
    Serial.print(MountPosition[MountIdx + 1]);
  */

  // Bounds-Checks to prevent buffer overflow
  while (MountIdx > 0 && MountPosition[MountIdx] > encoderValue)
    MountIdx--;

  while (MountIdx < (MOUNTCOUNT - 1) && MountPosition[MountIdx + 1] <= encoderValue)
    MountIdx++;

  int d1 = encoderValue - MountPosition[MountIdx];
  int d2 = MountPosition[MountIdx + 1] - encoderValue;
  int d2Mod = d2;

  if (d2 < d1)
    MountIdx++;

  if ( (fensterbreite % 2) == 0)
    d2Mod++;

  if (enDebugOutput)
  {
    Serial.print("  idx:"); Serial.print(MountIdx);
    Serial.print(" dist:"); Serial.print(d1);
    Serial.print(",");      Serial.print(d2);
    Serial.print("/");      Serial.print(d2Mod);
    Serial.print("  win:"); Serial.print(fensterbreite);
    Serial.print("/");      Serial.print(fensterbreite / 2);
    Serial.print(":");      Serial.print(fensterbreite % 2);
    Serial.print("  ");
  }

  fensterbreite /= 2;   // Umrechnung auf halbe Breite, asymetrisch bei ungerade fensterbreite

  bool inVicinity = (d1 <= fensterbreite);
  inVicinity = inVicinity || (d2Mod <= fensterbreite);

  if (encoderValue == MountPosition[MountIdx])
    result = POS_center;
  else if (inVicinity)
    result = POS_vicinity;

  else
    result = POS_none;

  return result;
  // POS_none, POS_vicinity, POS_center
}

void Mounts_showState(void)
{
  switch (CheckMountPosition2(encoderPos, MountHWidth * 2 + 1))
  {
    case POS_center  :
      Serial.print("W");
      Serial.print(MountIdx + 1);
      break;
    case POS_vicinity  :
      Serial.print("*");
      break;
    default : /* Optional */
      Serial.print(" ");
  }
  Serial.print(" ");
}

void Mounts_showNr(void)
{
  switch (CheckMountPosition2(encoderPos, 4))
  {
    case POS_center  :
    case POS_vicinity  :
      Serial.print("W");
      Serial.print(MountIdx + 1);
      break;
    default : /* Optional */
      Serial.print(" ");
  }
  Serial.print(" ");
}

