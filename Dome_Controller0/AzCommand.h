#ifndef _AzCommand_H_
#define _AzCommand_H_

// single key commands
#define AzCmd_DriveLeft 'l'
#define AzCmd_DriveRight 'r'
#define AzCmd_DriveStop 's'

#define AzCmd_help 'h'
#define AzCmd_zero '0'
#define AzCmd_zero1 'z'

#define Quad_zero 'Z'

#define CmdDetail_On 'D'
#define CmdDetail_Off 'd'

#define CmdTimed_On 'T'
#define CmdTimed_Off 't'

#define CmdDebug_On 'X'
#define CmdDebug_Off 'x'

#define CmdDumpState '?'

#define	CmdModulo4 '4'
/*
#define CmdOutputAutoOn 'A'
#define CmdOutputAutoOff 'a'
*/

#define Char_CR 13
#define Char_Spc 32
#define Char_254 254    // ignore (Arduino Due)

extern bool infoRequest;
extern bool includeDetail_bak;
#endif  // _AzCommand_H_
