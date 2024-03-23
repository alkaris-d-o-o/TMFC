#define pushButton_pin   33
#define CH2 18
#define CH3 19

unsigned long cTime, ch2_rising, ch3_rising;
volatile int ch2_value = 0, ch3_value = 0, receiver_input_channel_3, receiver_input_channel_4;

int rising_or_falling = 0;
uint32_t input;

/* Interrupt routines for CH2 and CH3 */
void IRAM_ATTR CHTwoChange(){ 
  if((GPIO.in >> CH2) & 1){ ch2_rising = micros(); }
  else{ ch2_value = micros() - ch2_rising; }
}
void IRAM_ATTR CHThreeChange(){ 
  if((GPIO.in >> CH3) & 1){ ch3_rising = micros(); }
  else{ ch3_value = micros() - ch3_rising; }
}


void setup(){
  Serial.begin(57600);      // Start the serial connetion @ 57600bps
  pinMode(CH2, INPUT);      // Define CH2 PIN as Input
  pinMode(CH3, INPUT);      // Define CH3 PIN as Input
  attachInterrupt(CH2, CHTwoChange, CHANGE);    // Attach interrupt (On change - Rising or falling edge)
  attachInterrupt(CH3, CHThreeChange, CHANGE);  // Attach interrupt (On change - Rising or falling edge)
} 
void loop(){
  Serial.print("CH2: ");
  Serial.print(ch2_value);
  Serial.print(", CH3: ");
  Serial.println(ch3_value);
  /* if(input){
    Serial.println("HIGH");
  }else Serial.println("LOW"); */
}