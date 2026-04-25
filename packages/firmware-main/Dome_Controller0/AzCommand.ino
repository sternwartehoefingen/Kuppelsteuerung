#include "./AzCommand.h"
#include "./AzDrive.h"
#include "./Quadratur_TC2.h"

bool infoRequest = false;
bool includeDetail_bak = false;

void Command_Check(void)
{
  // Command interface
  // Motor: l, s, r
  // Encoder: 0, z
  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    //               switch
    //              zeroDetected = true;

    switch (incomingByte) {

      case AzCmd_help:            // "h" help
        sayHello();
        Serial.println(" help h Z 0 l,s,r D/d, T/t, 4, X/x ?");
        break;

      case CmdDumpState:          // "?" dump state including details
        includeDetail_bak = includeDetail;
        includeDetail = true;     // temporary
        infoRequest = true;       // force output
        break;

      case AzCmd_DriveLeft:       // "l" motor left
        driveControl = DRV_CCW;   // forward to output control
        DriveWDT(true);
        Serial.println("Drive left ");
        break;

      case AzCmd_DriveStop:       // "s" motor stop
        driveControl = DRV_OFF;   // forward to output control
        DriveWDT(false);
        Serial.println("Drive stop ");
        break;

      case AzCmd_DriveRight:      // "r" motor right
        driveControl = DRV_CW;    // forward to output control
        DriveWDT(true);
        Serial.println("Drive right ");
        break;

      case CmdDetail_On:      // "D" include details in output
        includeDetail = ON;
        enModulo4 = OFF;
        Serial.println("Detail: on ");
        break;

      case CmdDetail_Off:      // "d" exclude details from output
        includeDetail = OFF;
        enModulo4 = OFF;
        Serial.println("Detail: off ");
        break;

      case CmdTimed_On:      // "T" enable time trigger for output
        enTimeTrig = ON;
        Serial.println("Timed output: on ");
        break;

      case CmdTimed_Off:      // "t" disable time trigger for output
        enTimeTrig = OFF;
        Serial.println("Timed output: off ");
        break;

      case CmdDebug_On:      // "X" enable debug output
        enDebugOutput = ON;
        Serial.println("Debug output: on ");
        break;

      case CmdDebug_Off:      // "x" disable debug output
        enDebugOutput = OFF;
        Serial.println("Debug output: off ");
        break;

      case CmdModulo4:      // "4" reduce output to 1/4
        enModulo4 = ON;
        Serial.println("Modulo 4 output: on ");
        break;

      /*
            case CmdOutputAutoOn:     // "A"
              AutoOutEn = ON;
              Serial.println("Auto Output : on ");

            case CmdOutputAutoOff:     // "a"
              AutoOutEn = OFF;
              Serial.println("Auto Output : off ");
      */

      case AzCmd_zero:  // "0","z" zero azimuth
      case AzCmd_zero1:
        Serial.println("Reset azimuth");
        zeroDetected = true;
        break;

      case Quad_zero:
        QuadTC2_reset();
        Serial.println("Reset radius reference");
        break;

      case Char_CR:  // ignore cr
      case Char_Spc:
      case Char_254:
        break;

      default :
        Serial.print(" rcvd: ");
        Serial.println(incomingByte, DEC);
    }
  }
}
