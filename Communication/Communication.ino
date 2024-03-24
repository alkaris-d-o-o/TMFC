#define GreenLed 12
#define RedLed 14

#include "helper.h"

/* My custom Classes */
Helper Helper;

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

void setup() {
  Serial.begin(115200);

  /* Configuration of ESCs */
  escs_cfg();

  /* RC 4 channels setup */
  receiver_cfg();

  pinMode(GreenLed, OUTPUT);
  pinMode(RedLed, OUTPUT);
}

void loop(){
  Serial.print("Start: ");
  Serial.print(Helper.armingState(chOneTime, chThreeTime));
  Serial.print(" CH1: ");
  Serial.print(chOneTime);
  Serial.print(" CH2: ");
  Serial.print(chTwoTime);
  Serial.print(" CH3: ");
  Serial.print(chThreeTime);
  Serial.print(" CH4: ");
  Serial.println(chFourTime);
  // while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  // loop_timer = micros();                                               //Reset the loop timer

  // CH1DC = map(chOneTime,   1000, 2000, 3275, 6553);
  // CH2DC = map(chTwoTime,   1000, 2000, 3275, 6553);
  CH3DC = map(chThreeTime, 1000, 2000, 3275, 6553);
  // CH4DC = map(chFourTime,  1000, 2000, 3275, 6553);


  CH1DC = CH3DC;
  CH2DC = CH3DC;
  CH3DC = CH3DC;
  CH4DC = CH3DC;

  ledcWrite(ledcCH1, CH1DC);
  ledcWrite(ledcCH2, CH2DC);
  ledcWrite(ledcCH3, CH3DC);
  ledcWrite(ledcCH4, CH4DC);

}












