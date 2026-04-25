int ShutterMotorState=0;
// int driveDir;


void Shutter_setup(void)
{
  pinMode(roofSwitchClosed, INPUT_PULLUP);
  pinMode(roofSwitchOpened, INPUT_PULLUP);
  pinMode(roofDriveClose, OUTPUT);
  pinMode(roofDriveOpen,  OUTPUT);
}

int Shutter_read(void)
{
  return (digitalRead(roofSwitchOpened)<<1) | digitalRead(roofSwitchClosed);
}

void Shutter_ShowState(void)
{
  const char ShutterStateChar[4] = {'-', 'R', 'L', '?'};

  Serial.print("S");
  Serial.print(driveEn);
  Serial.print(ShutterStateChar[1]);
  Serial.print(" ");
}

void ShutterStart(bool Oeffnen)
{
  if (Oeffnen)
  {
    digitalWrite(roofDriveClose, MOTOR_Relay_off);
    digitalWrite(roofDriveOpen,  MOTOR_Relay_on);
    ShutterMotorState = 1; // opening
  }
  else
  {
    digitalWrite(roofDriveOpen,  MOTOR_Relay_off);
    digitalWrite(roofDriveClose, MOTOR_Relay_on);
    ShutterMotorState = 2; // closing
  }
}

void ShutterStop(void)
{
  digitalWrite(roofDriveClose,MOTOR_Relay_off);
  digitalWrite(roofDriveOpen,MOTOR_Relay_off);
  ShutterMotorState=0; // stopped
}

