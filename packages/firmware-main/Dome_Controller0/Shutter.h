#ifndef Shutter_H
#define Shutter_H

#define MOTOR_Relay_off false
#define MOTOR_Relay_on true

// ShutterStatus      opened,intermediate,closed 
// ShuttermotorStatus opening, stopped, closing

// Ausgabe Ssm
//           ^--- o,s,c   Motorzustand
//          ^---- O,I,C   Endschalterzustand

void Shutter_setup(void);
int  Shutter_read(void);
void Shutter_ShowState(void);

#endif

