// Hardware Quadrature Decoder - TimerCounter TC2
// -----------------------------------------------
/*
   Inkrementalgeber (TC2 Hardware Interface)
   A - D5
   B - D4
   I - D10
*/

#ifdef __SAM3X8E__  // Arduino Due only

bool useSerialDbgISR = false;

void QuadTC2_setup(void)
{
  enable_TC2_CLK();
  set_TC2_quadrature_config();
}

void set_TC2_quadrature_config()
{
  set_QuadTC2_registers();
//  set_QuadTC2_idx_int();  // Index interrupts
}

void enable_TC2_CLK()
{
  // Enable the Clock for all Channels Timer Counter 2 by using the PIO Controller
  pmc_enable_periph_clk(ID_TC6);
  pmc_enable_periph_clk(ID_TC7);
  pmc_enable_periph_clk(ID_TC8);
}

long QuadTC2_read(void)
{
/*  
  long result;
  result = REG_TC2_CV0;  // read TC2 counter value
  return result;
*/
  return REG_TC2_CV0;  // read TC2 counter value
}

void QuadTC2_reset(void)
{
  // Enable the clock (CLKEN=1) and reset the counter (SWTRG=1)
  //  REG_TC2_CCR0 = TC_CCR_CLKEN | TC_CCR_SWTRG; // TC Channel Control Register - CLKEN = TRUE, SWTRG = TRUE --> Reset

  REG_TC2_CCR0 = TC_CCR_SWTRG; //Reset counter
//  Serial.println(" TC2 Reset ");
}

void TC7_Handler() {
  //This won't fire unless the Index interrupt is enabled above
  // Der Interrupt Handler bezieht sich auf den Kanal und nicht auf den Timer--> Timer 0 --> TC1 Handler, Timer 2 --> TC7 Handler
  // TC2, 1, TC7_IRQn  =>  TC7_Handler()
  if (useSerialDbgISR)
  {
    Serial.print("Int-TC2 ");
    Serial.println(REG_TC2_CV0);
  }
  TC_GetStatus(TC2, 1);// otherwise you get infinite interrupts
  //  TPR=REG_TC2_CV2; //Store ticks

#ifdef DBG_ERRORS
  if (!(REG_TC2_CV0 == 2879))    // PrÃƒÂ¼fung KW auf Werte 1439, 2879
  {
    pinOutput(pinDebug3x, HIGH);    // Debug Output Trigger for phasing
    delayMicroseconds(1);
    pinOutput(pinDebug3x, LOW);   // Debug Output Trigger for phasing
    errCount++;
    //    Serial.print("TC2: ");
    //    Serial.println(REG_TC2_CV0);
  }
#endif

  REG_TC2_CCR2 = TC_CCR_SWTRG; //Reset counter, no effect 
}

void set_QuadTC2_registers()
{
  //Resets the count every index. Rem out the line above,and use the line below instead.
//  REG_TC2_CMR0 = TC_CMR_TCCLKS_XC0 | TC_CMR_EEVTEDG_RISING | TC_CMR_ABETRG; // Bit 1,3 --> Set Clock XC0, Bit 8 -> External Trigger Rising Edge, Bit 10 -> TIOA used as external Trigger
  REG_TC2_CMR0 = TC_CMR_TCCLKS_XC0 | TC_CMR_ABETRG; // Bit 1,3 --> Set Clock XC0, Bit 10 -> TIOA used as external Trigger

  REG_TC2_CMR2 = TC_CMR_TCCLKS_TIMER_CLOCK4; // Set to TCLK4

  // activate quadrature encoder and position measure mode, no filters
  REG_TC2_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA; // Bit 8 QDEN - Quadrature Decoder, Bit 9 - POSEN - POsition enabled, Bit 12 EDGPHA - EDGe on PHA count mode logic on this bit seems inverted
  //  else
  //    REG_TC2_BMR = TC_BMR_QDEN|TC_BMR_POSEN|TC_BMR_EDGPHA; // Bit 8 QDEN - Quadrature Decoder, Bit 9 - POSEN - POsition enabled, Bit 12 EDGPHA - EDGe on PHA count mode logic on this bit seems inverted
  REG_TC2_BMR |= TC_BMR_FILTER | TC_BMR_MAXFILT(63); // debouncing of A,B,I
  if (QUADDIR == CCW) REG_TC2_BMR |= TC_BMR_SWAP;   // turning direction CCW (INVerted phA) (alt: TC_BMR_INVA)

  // Enable the clock (CLKEN=1) and reset the counter (SWTRG=1)
  REG_TC2_CCR0 = TC_CCR_CLKEN | TC_CCR_SWTRG; // TC Channel Control Register - CLKEN = TRUE, SWTRG = TRUE --> Reset
  REG_TC2_CCR1 = TC_CCR_CLKEN | TC_CCR_SWTRG;
  REG_TC2_CCR2 = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void set_QuadTC2_idx_int()
{
  //Remark out these 4 lines to disable Index interrupts
  REG_TC2_CMR1 = TC_CMR_ETRGEDG_RISING; // Set rising edge of Z --> Bit 8 - Rising Edge Trigger True Bit 10 FALSE TIOB is Trigger, True TIOA is Trigger
  REG_TC2_IER1 = TC_IER_ETRGS ; // enable interrupt on Z - Enables External Trigger Interrupt on Channel 1
  REG_TC2_IDR1 = 0b01111111; // disable other interrupts
  NVIC_EnableIRQ(TC7_IRQn); // Enables the interrupt on Channel 1 of Counter 2 --> TC7_IRQn
}

#else  // other controller

// return dummy value, Hardware not available on AVR
long QuadTC2_read(void)  { return 0; }
void QuadTC2_reset(void) {}

#endif  // __SAM3X8E__

