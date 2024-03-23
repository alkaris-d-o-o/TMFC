/* Four motors for quad */
#define M1 26
#define M2 25
#define M3 33
#define M4 32

const int ledcCH1 = 0, ledcCH2 = 1, ledcCH3 = 2, ledcCH4 = 3;
const int ledcResolution = 16;   // LEDC resolution (bits) for motor 1
const int ledcFrequency  = 50;   // LEDC frequency (Hz) for motor 1
// Duty Cycle of the PWM signal generated
u_int16_t CH1DC = 0, CH2DC = 0, CH3DC = 0, CH4DC = 0;

/* RC 4 channels at the moment */
#define CH1 5
#define CH2 18
#define CH3 19
#define CH4 23

long chOneRise = 0, chTwoRise = 0, chThreeRise = 0, chFourRise = 0;
volatile long chOneTime = 0, chTwoTime = 0, chThreeTime = 0, chFourTime = 0;

/* Interrupt routines */
void IRAM_ATTR chOneChange()   { if((GPIO.in >> CH1) & 1){ chOneRise   = micros(); } else { chOneTime   = micros() - chOneRise; } }
void IRAM_ATTR chTwoChange()   { if((GPIO.in >> CH2) & 1){ chTwoRise   = micros(); } else { chTwoTime   = micros() - chTwoRise; } }
void IRAM_ATTR chThreeChange() { if((GPIO.in >> CH3) & 1){ chThreeRise = micros(); } else { chThreeTime = micros() - chThreeRise; } }
void IRAM_ATTR chFourChange()  { if((GPIO.in >> CH4) & 1){ chFourRise  = micros(); } else { chFourTime  = micros() - chFourRise; } }

void setup() {
  Serial.begin(57600);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);

  // Configure LEDC timers for motor 1, motor 2, motor 3 and motor 4
  ledcSetup(ledcCH1, ledcFrequency, ledcResolution);
  ledcSetup(ledcCH2, ledcFrequency, ledcResolution);
  ledcSetup(ledcCH3, ledcFrequency, ledcResolution);
  ledcSetup(ledcCH4, ledcFrequency, ledcResolution);

  // Attach motor pins to the respective LEDC channels
  ledcAttachPin(M1, ledcCH1);
  ledcAttachPin(M2, ledcCH2);
  ledcAttachPin(M3, ledcCH3);
  ledcAttachPin(M4, ledcCH4);

  /* RC 4 channels setup */
  pinMode(CH1, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  pinMode(CH2, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  pinMode(CH3, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  pinMode(CH4, INPUT_PULLUP);                                         // Configure the pin as an input with internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(CH1), chOneChange,   CHANGE); // Configure the interrupt
  attachInterrupt(digitalPinToInterrupt(CH2), chTwoChange,   CHANGE); // Configure the interrupt
  attachInterrupt(digitalPinToInterrupt(CH3), chThreeChange, CHANGE); // Configure the interrupt
  attachInterrupt(digitalPinToInterrupt(CH4), chFourChange,  CHANGE); // Configure the interrupt
}

void loop(){
  Serial.print("CH1: ");
  Serial.print(chOneTime);
  Serial.print(" CH2: ");
  Serial.print(chTwoTime);
  Serial.print(" CH3: ");
  Serial.print(chThreeTime);
  Serial.print(" CH4: ");
  Serial.println(chFourTime);
  // while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  // loop_timer = micros();                                               //Reset the loop timer

  CH1DC = map(chThreeTime, 1000, 2000, 3275, 6553);
  CH2DC = CH1DC;
  CH3DC = CH1DC;
  CH4DC = CH1DC;

  ledcWrite(ledcCH1, CH1DC);
  ledcWrite(ledcCH2, CH2DC);
  ledcWrite(ledcCH3, CH3DC);
  ledcWrite(ledcCH4, CH4DC);
}












