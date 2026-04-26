#ifndef _AzEncoder_H_
#define _AzEncoder_H_

//#define AzEncoderINPUTMODE INPUT
#define AzEncoderINPUTMODE INPUT_PULLUP

typedef enum {resolutionNative, resolution_0d5, resolution_1d_low, resolution_1d_high} resolution_t;

extern bool zeroDetected;
extern int  encoderPos;
extern bool PinsChanged;
extern bool enEncoderSynthese;

void AzEncoder_setup(unsigned int MaxPos, resolution_t selectResolution);
void AzEncoder_read(void);
void update_encoderPos(void);
void AzSignal_echo(void);

#ifdef ENCODER_SYNTH
void AzSignal_synthese(void);
#endif

#endif // _AzEncoder_H_
