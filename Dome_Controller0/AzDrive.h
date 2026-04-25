#ifndef _AzDrive_H_
#define _AzDrive_H_

// Drive control 
#define DRV_CW  0b01
#define DRV_CCW (DRV_CW ^ 0b11)
#define DRV_OFF 0b00

#define DRV_TIMEOUT_TIMEms 5000 

extern int driveControl;
extern bool driveWDTactiv;

void AzDrive_setup(bool DriveDirInverse);
void AzDrive_CtrlReadDIO(void);
void AzDrive_CtrlOutput(void);
void AzDrive_showState(bool details);
void DriveWDT(bool DriveOn);
bool DriveWDTdead(void);


#endif  // _AzDrive_H_
