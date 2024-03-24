/* Interrupt routines */
void IRAM_ATTR chOneChange()   { if((GPIO.in >> CH1) & 1){ chOneRise   = micros(); } else { chOneTime   = micros() - chOneRise;   if(chOneTime   < 1000){chOneTime   = 1000;} else if(chOneTime   > 2000){chOneTime   = 2000;} } }
void IRAM_ATTR chTwoChange()   { if((GPIO.in >> CH2) & 1){ chTwoRise   = micros(); } else { chTwoTime   = micros() - chTwoRise;   if(chTwoTime   < 1000){chTwoTime   = 1000;} else if(chTwoTime   > 2000){chTwoTime   = 2000;} } }
void IRAM_ATTR chThreeChange() { if((GPIO.in >> CH3) & 1){ chThreeRise = micros(); } else { chThreeTime = micros() - chThreeRise; if(chThreeTime < 1000){chThreeTime = 1000;} else if(chThreeTime > 2000){chThreeTime = 2000;} } }
void IRAM_ATTR chFourChange()  { if((GPIO.in >> CH4) & 1){ chFourRise  = micros(); } else { chFourTime  = micros() - chFourRise;  if(chFourTime  < 1000){chFourTime  = 1000;} else if(chFourTime  > 2000){chFourTime  = 2000;} } }

void receiver_cfg(void){
  pinMode(CH1, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  pinMode(CH2, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  pinMode(CH3, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  pinMode(CH4, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(CH1), chOneChange,   CHANGE); // Configure the interrupt
  attachInterrupt(digitalPinToInterrupt(CH2), chTwoChange,   CHANGE); // Configure the interrupt
  attachInterrupt(digitalPinToInterrupt(CH3), chThreeChange, CHANGE); // Configure the interrupt
  attachInterrupt(digitalPinToInterrupt(CH4), chFourChange,  CHANGE); // Configure the interrupt
}