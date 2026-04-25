const int8_t Gray[4] = {0b00, 0b01, 0b11, 0b10};

bool zeroDetected      = false;
int  encoderPos        = 0;
unsigned int encoderMaxPos = 4;   // default
bool PinsChanged       = false;
bool enEncoderSynthese = false;
int encGray;
resolution_t resolution;

int sensorValA;
int sensorValB;
int sensorValI;

static int encoderPinALast = HIGH;
static int encoderPinBLast = HIGH;

void AzEncoder_setup(unsigned MaxPos, resolution_t selectResolution)
{
  pinMode (encoderPinA, AzEncoderINPUTMODE);  // Input Phase Signals
  pinMode (encoderPinB, AzEncoderINPUTMODE);
  pinMode (encoderPinI, INPUT_PULLUP);

  pinMode(echoPinA, OUTPUT);            // Output Phase Signals
  pinMode(echoPinB, OUTPUT);
  pinMode(echoPinI, OUTPUT);

  encoderMaxPos = MaxPos;

  resolution = selectResolution;

#ifdef ENCODER_SYNTH
  pinMode(cfgPinEnSYNTH, OUTPUT);   // Control level Encoder Synthese (Low=aktiv)
  pinMode(cfgPinSYNTH,  INPUT_PULLUP);  // Low=Encoder Synthese für ASCOM
  digitalWrite(cfgPinEnSYNTH, LOW);    // Synthese Steuerung über HW-Pin aktivieren

  // debug
  digitalWrite(cfgPinEnSYNTH, HIGH);    // Markierung Berechnung
  delay(1);
  digitalWrite(cfgPinEnSYNTH, LOW);    // Markierung Berechnung

#endif
}

void AzEncoder_read(void)
{
  encoderPinALast = sensorValA;         // update last values variables
  encoderPinBLast = sensorValB;

  sensorValA = digitalRead(encoderPinA);
  sensorValB = digitalRead(encoderPinB);
  sensorValI = digitalRead(encoderPinI);

  if (!sensorValI)
    zeroDetected = true;

  PinsChanged = (sensorValA != encoderPinALast) || (sensorValB != encoderPinBLast);
} // AzEncoder_read

void update_encoderPos(void)
{
  if (sensorValA == encoderPinBLast) {
    if ( !(encoderPinALast == sensorValB)) {
      encoderPos++;
      if (encoderPos >= encoderMaxPos)      // Wertebereich auf 0.. MAXPOS-1 beschränken
        encoderPos -= encoderMaxPos;
    }
  }
  else {
    if ( encoderPinALast == sensorValB) {
      encoderPos--;
      if (encoderPos < 0)          // Wertebereich auf 0.. MAXPOS-1 beschränken
        encoderPos += encoderMaxPos;
    }
  }

//  if (zeroDetected & sensorValA & sensorValB) {        // Reset mit Synchronisation
  if (zeroDetected) {        // Reset
    encoderPos = 0;
    zeroDetected = false;
  }
} // update_encoderPos

void showState_AzSensor(bool details)
{
  Serial.print("$");
  if (details) {
    Serial.print("S");
    Serial.print(sensorValA);
    Serial.print(sensorValB);
    Serial.print(sensorValI);
    /*
        Serial.print(" L");               // previous state
        Serial.print(encoderPinALast);
        Serial.print(encoderPinBLast);
    */
    Serial.print(" ");
  }
}

void showState_AzSynthese(bool details)
{
  if (details) {
    Serial.print("E");
    Serial.print(encGray & 0x01);
    Serial.print((encGray & 0x02) / 2);
    Serial.print(" ");
  }
}

void AzSignal_echo(void)
{ // Original Encoder Signal von Motorwelle
  digitalWrite(echoPinA, sensorValA);
  digitalWrite(echoPinB, sensorValB);
  digitalWrite(echoPinI, sensorValI);
}

#ifdef ENCODER_SYNTH
void AzSignal_synthese(void)
{
  //  digitalWrite(cfgPinEnSYNTH, HIGH);   // Markierung Berechnung

  switch (resolution) {
    case resolution_1d_low:
      // 360 Flanken, Auflösung 1°: LesveDomeNet 360/40/10, (17.75 µs)
      encGray = Gray[((encoderPos * 1203UL / 4096 + 1) / 2) & 0x03]; // 360/2452 aprox. 1203 / 8192  (err= 0.08° bzw. 2.2 mm @ U=10000 mm)
      break;
    case resolution_1d_high:
      // 360 Flanken, Auflösung 1°: LesveDomeNet 360/40/10, (18.75 µs)
      encGray = Gray[((encoderPos * 4811UL / 16384 + 1) / 2) & 0x03]; // 360/2452 aprox. 4811 / 32768  (err= 0.003° bzw. 0.1 mm @ U=10000 mm)
      break;
    case resolution_0d5:
      // 720 Flanken, Auflösung 0.5°: LesveDomeNet 360/20/10, (18.75 µs)
      encGray = Gray[((encoderPos * 4811UL / 8192 + 1) / 2) & 0x03]; // 720/2452 aprox. 4811 / 16384  (err= 0.003° bzw. 0.1 mm @ U=10000 mm)
      break;
    default:
      //resolutionNative
      encGray = Gray[encoderPos  & 0x03]; // 9.5 µs
  }
  //  digitalWrite(cfgPinEnSYNTH, LOW);    // Markierung Berechnung Ende

  digitalWrite(echoPinA, (bool) (encGray & 0x02));
  digitalWrite(echoPinB, (bool) (encGray & 0x01));
}
#endif

/* -----------------------------------------------------------------------

   Alternative: kompakte Auswertemethode für AB-Encoder

  #define quadrature_input (PORTA&3)
  // bit 0 und bit 1 sind Quadratureingaenge
  #define MAXCOUNT 6
  static char table[4][4]={{0,1,-1,0},{-1,0,0,1},{1,0,0,-1},{0,-1,1,0}};
  char position, new_quadrature_value, last_quadrature_value;
  void main(void)
  {
  TRISA=1; // alles Eingänge
  TRISB=0; // alles Ausgänge
  position=0;
  last_quadrature_value=quadrature_input
  while(1)
  {
    new_quadrature_value=quadrature_input;
    position=(position+table[last_quadrature_value][new_quadrature_value])%MAXCOUNT;
    PORTB=(unsigned char)1<<position;
    last_quadrature_value=new_quadrature_value;
    _delay_ms(5);
  }
  }
   -----------------------------------------------------------------------
*/
