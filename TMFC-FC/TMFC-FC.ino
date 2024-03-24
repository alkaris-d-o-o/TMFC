#include "gyro.h"
#include "helper.h"

int error = 0;

/* My custom Classes */
Helper Helper;
Gyro Gyro;

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

long loop_timer;
int throttle;

void setup() {
  Serial.begin(115200);
  Wire.begin(); 

  /* Configuration of ESCs */
  escs_cfg();

  /* RC 4 channels setup */
  receiver_cfg();

  pinMode(GreenLed, OUTPUT);
  pinMode(RedLed, OUTPUT);

  /* Calibrate ESCS before usage .. */
  // escsFullThrottle();

  /*
   *  Setup Gyro 
   *    1. Setup registers
   *    2. Run Calibration
   */
  Gyro.setupRegisters();
  Gyro.runCalibration();

  /* Return ESCs to OFF */
  // escsLowThrottle();

  /*
   *  Wait until the receiver is active.
   */
  while (chOneTime < 990 || chTwoTime < 990 || chThreeTime < 990 || chFourTime < 990)  {
    error = 3; // Set the error status to 3.
    delay(4);
  }
  error = 0;   //Reset the error status to 0.

  /*
   *  Wait until the receiver is active.
   */
  while (chThreeTime < 990 || chThreeTime > 1050)  {
    error = 4; //Set the error status to 4.
    delay(4);
  }
  error = 0;   //Reset the error status to 0.

  /* Reset the loop timer */
  loop_timer = micros();
}

void loop(){
  Gyro.readData();                                                                 // Read the gyro and accelerometer data.

  //65.5 = 1 deg/sec (check the datasheet of the MPU-6050 for more information).
  Gyro.gyro_roll_input  = (Gyro.gyro_roll_input  * 0.7) + (((float)Gyro.gyro_roll  / 65.5) * 0.3);      // Gyro pid input is deg/sec.
  Gyro.gyro_pitch_input = (Gyro.gyro_pitch_input * 0.7) + (((float)Gyro.gyro_pitch / 65.5) * 0.3);      // Gyro pid input is deg/sec.
  Gyro.gyro_yaw_input   = (Gyro.gyro_yaw_input   * 0.7) + (((float)Gyro.gyro_yaw   / 65.5) * 0.3);      // Gyro pid input is deg/sec.


  // Gyro angle calculations
  // 0.0000611 = 1 / (250Hz / 65.5)
  Gyro.angle_pitch += (float)Gyro.gyro_pitch * 0.0000611;                                               // Calculate the traveled pitch angle and add this to the angle_pitch variable.
  Gyro.angle_roll  += (float)Gyro.gyro_roll  * 0.0000611;                                               // Calculate the traveled roll angle and add this to the angle_roll variable.

  // 0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians and not degrees.
  Gyro.angle_pitch -= Gyro.angle_roll  * sin((float)Gyro.gyro_yaw * 0.000001066);                       // If the IMU has yawed transfer the roll angle to the pitch angle
  Gyro.angle_roll  += Gyro.angle_pitch * sin((float)Gyro.gyro_yaw * 0.000001066);                       // If the IMU has yawed transfer the pitch angle to the roll angle
  
  // Accelerometer angle calculations
  Gyro.acc_total_vector = sqrt((Gyro.acc_x * Gyro.acc_x) + (Gyro.acc_y * Gyro.acc_y) + (Gyro.acc_z * Gyro.acc_z));    //Calculate the total accelerometer vector.
  
  if (abs(Gyro.acc_y) < Gyro.acc_total_vector) {                                                        // Prevent the asin function to produce a NaN.
    Gyro.angle_pitch_acc = asin((float)Gyro.acc_y / Gyro.acc_total_vector) * 57.296;                    // Calculate the pitch angle.
  }
  if (abs(Gyro.acc_x) < Gyro.acc_total_vector) {                                                        // Prevent the asin function to produce a NaN.
    Gyro.angle_roll_acc  = asin((float)Gyro.acc_x / Gyro.acc_total_vector) * 57.296;                    // Calculate the roll angle.
  }
  
  Gyro.angle_pitch = Gyro.angle_pitch * 0.9996 + Gyro.angle_pitch_acc * 0.0004;                   // Correct the drift of the gyro pitch angle with the accelerometer pitch angle.
  Gyro.angle_roll = Gyro.angle_roll * 0.9996 + Gyro.angle_roll_acc * 0.0004;                      // Correct the drift of the gyro roll angle with the accelerometer roll angle.

  Gyro.pitch_level_adjust = Gyro.angle_pitch * 15;                                                // Calculate the pitch angle correction.
  Gyro.roll_level_adjust  = Gyro.angle_roll * 15;                                                 // Calculate the roll angle correction.

  if (!Gyro.auto_level) {                                                                         // If the quadcopter is not in auto-level mode
    Gyro.pitch_level_adjust = 0;                                                                  // Set the pitch angle correction to zero.
    Gyro.roll_level_adjust = 0;                                                                   // Set the roll angle correcion to zero.
  }
  
  // Serial.print("Roll: ");
  // Serial.print(Gyro.angle_roll);
  // Serial.print(" Pitch: ");
  // Serial.println(Gyro.angle_pitch);


  if(Helper.armingState(chOneTime, chThreeTime) == 2){
    Gyro.angle_pitch = Gyro.angle_pitch_acc;                                                  // Set the gyro pitch angle equal to the accelerometer pitch angle when the quadcopter is started.
    Gyro.angle_roll  = Gyro.angle_roll_acc;                                                   // Set the gyro roll angle equal to the accelerometer roll angle when the quadcopter is started.

    //Reset the PID controllers for a bumpless start.
    Gyro.pid_i_mem_roll = 0;
    Gyro.pid_last_roll_d_error = 0;
    Gyro.pid_i_mem_pitch = 0;
    Gyro.pid_last_pitch_d_error = 0;
    Gyro.pid_i_mem_yaw = 0;
    Gyro.pid_last_yaw_d_error = 0;
  }

  // The PID set point in degrees per second is determined by the roll receiver input.
  // In the case of deviding by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  Gyro.pid_roll_setpoint = 0;
  // We need a little dead band of 16us for better results.
  if (chFourTime > 1508) Gyro.pid_roll_setpoint = chFourTime - 1508;
  else if (chFourTime < 1492) Gyro.pid_roll_setpoint = chFourTime - 1492;

  Gyro.pid_roll_setpoint -= Gyro.roll_level_adjust;                                          // Subtract the angle correction from the standardized receiver roll input value.
  Gyro.pid_roll_setpoint /= 3.0;                                                             // Divide the setpoint for the PID roll controller by 3 to get angles in degrees.

  // The PID set point in degrees per second is determined by the pitch receiver input.
  // In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  Gyro.pid_pitch_setpoint = 0;
  // We need a little dead band of 16us for better results.
  if (chTwoTime > 1508) Gyro.pid_pitch_setpoint = chTwoTime - 1508;
  else if (chTwoTime < 1492) Gyro.pid_pitch_setpoint = chTwoTime - 1492;

  Gyro.pid_pitch_setpoint -= Gyro.pitch_level_adjust;                                        // Subtract the angle correction from the standardized receiver pitch input value.
  Gyro.pid_pitch_setpoint /= 3.0;                                                            // Divide the setpoint for the PID pitch controller by 3 to get angles in degrees.

  // The PID set point in degrees per second is determined by the yaw receiver input.
  // In the case of deviding by 3 the max yaw rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  Gyro.pid_yaw_setpoint = 0;
  // We need a little dead band of 16us for better results.
  if (chThreeTime > 1050) { // Do not yaw when turning off the motors.
    if (chOneTime > 1508) Gyro.pid_yaw_setpoint = (chOneTime - 1508) / 3.0;
    else if (chOneTime < 1492) Gyro.pid_yaw_setpoint = (chOneTime - 1492) / 3.0;
  }

  /* Calculate PID data */
  Gyro.calculatePID();

  throttle = chThreeTime;                                                            //We need the throttle signal as a base signal.

  if(Helper.isArmed()){
    digitalWrite(GreenLed, HIGH);
    digitalWrite(RedLed,   LOW);

   //  if (throttle > 1800) throttle = 1800;                                          //We need some room to keep full control at full throttle.
    CH1DC = throttle - Gyro.pid_output_pitch + Gyro.pid_output_roll - Gyro.pid_output_yaw;        //Calculate the pulse for esc 1 (front-right - CCW).
    CH2DC = throttle + Gyro.pid_output_pitch + Gyro.pid_output_roll + Gyro.pid_output_yaw;        //Calculate the pulse for esc 2 (rear-right - CW).
    CH3DC = throttle + Gyro.pid_output_pitch - Gyro.pid_output_roll - Gyro.pid_output_yaw;        //Calculate the pulse for esc 3 (rear-left - CCW).
    CH4DC = throttle - Gyro.pid_output_pitch - Gyro.pid_output_roll + Gyro.pid_output_yaw;        //Calculate the pulse for esc 4 (front-left - CW).

    if (CH1DC < 1100) CH1DC = 1100;                                                //Keep the motors running.
    if (CH2DC < 1100) CH2DC = 1100;                                                //Keep the motors running.
    if (CH3DC < 1100) CH3DC = 1100;                                                //Keep the motors running.
    if (CH4DC < 1100) CH4DC = 1100;                                                //Keep the motors running.

    if (CH1DC > 2000) CH1DC = 2000;                                                 //Limit the esc-1 pulse to 2000us.
    if (CH2DC > 2000) CH2DC = 2000;                                                 //Limit the esc-2 pulse to 2000us.
    if (CH3DC > 2000) CH3DC = 2000;                                                 //Limit the esc-3 pulse to 2000us.
    if (CH4DC > 2000) CH4DC = 2000;                                                 //Limit the esc-4 pulse to 2000us.
  }else{
    digitalWrite(GreenLed, LOW);
    digitalWrite(RedLed,   HIGH);

    CH1DC = 1000;
    CH2DC = 1000;
    CH3DC = 1000;
    CH4DC = 1000;
  }

  /* Serial.print("Start: ");
  Serial.print(Helper.armingState(chOneTime, chThreeTime));
  Serial.print(" CH1: ");
  Serial.print(CH1DC);
  Serial.print(" CH2: ");
  Serial.print(CH2DC);
  Serial.print(" CH3: ");
  Serial.print(CH3DC);
  Serial.print(" CH4: "); 
  Serial.println(CH4DC); */


  CH1DC = map(CH1DC, 1000, 2000, 3275, 6553);
  CH2DC = map(CH2DC, 1000, 2000, 3275, 6553);
  CH3DC = map(CH3DC, 1000, 2000, 3275, 6553);
  CH4DC = map(CH4DC, 1000, 2000, 3275, 6553);

  ledcWrite(ledcCH1, CH1DC);
  ledcWrite(ledcCH2, CH2DC);
  ledcWrite(ledcCH3, CH3DC);
  ledcWrite(ledcCH4, CH4DC);

 /*
  *  Because of the angle calculation the loop time is getting very important. If the loop time is
  *  longer or shorter than 4000us the angle calculation is off. If you modify the code make sure
  *  that the loop time is still 4000us and no longer! More information can be found on
  */
  while (micros() - loop_timer < 4000);                                            // We wait until 4000us are passed.
  loop_timer = micros();                                                           // Set the timer for the next loop.
}












