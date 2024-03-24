#include "gyro.h"
#include "helper.h"

void Gyro::setupRegisters(void){
  Wire.beginTransmission(this->address);                       //Start communication with the MPU-6050.
  Wire.write(0x6B);                                            //We want to write to the PWR_MGMT_1 register (6B hex).
  Wire.write(0x00);                                            //Set the register bits as 00000000 to activate the gyro.
  Wire.endTransmission();                                      //End the transmission with the gyro.

  Wire.beginTransmission(this->address);                       //Start communication with the MPU-6050.
  Wire.write(0x1B);                                            //We want to write to the GYRO_CONFIG register (1B hex).
  Wire.write(0x08);                                            //Set the register bits as 00001000 (500dps full scale).
  Wire.endTransmission();                                      //End the transmission with the gyro.

  Wire.beginTransmission(this->address);                       //Start communication with the MPU-6050.
  Wire.write(0x1C);                                            //We want to write to the ACCEL_CONFIG register (1A hex).
  Wire.write(0x10);                                            //Set the register bits as 00010000 (+/- 8g full scale range).
  Wire.endTransmission();                                      //End the transmission with the gyro.

  Wire.beginTransmission(this->address);                       //Start communication with the MPU-6050.
  Wire.write(0x1A);                                            //We want to write to the CONFIG register (1A hex).
  Wire.write(0x03);                                            //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz).
  Wire.endTransmission();                                      //End the transmission with the gyro.
}

/* Run calibration for Gyro */
void Gyro::runCalibration(void){
  if (this->use_manual_calibration) this->calItterations = 2000;                      //If manual calibration is used set cal_int to 2000 to skip the calibration.
  else {
    this->calItterations = 0;                                                                      //If manual calibration is not used.
    this->manual_gyro_pitch_cal_value = 0;                                                  //Set the manual pitch calibration variable to 0.
    this->manual_gyro_roll_cal_value = 0;                                                   //Set the manual roll calibration variable to 0.
    this->manual_gyro_yaw_cal_value = 0;                                                    //Set the manual yaw calibration variable to 0.
  }

  if (this->calItterations != 2000) {
    // Let's take multiple gyro data samples so we can determine the average gyro offset (calibration).
    // Take 2000 readings for calibration.
    for (this->calItterations = 0; this->calItterations < 2000 ; this->calItterations ++) {
      if (this->calItterations % 25 == 0) digitalWrite(RedLed, !digitalRead(RedLed));           // Change the led status every 125 readings to indicate calibration.
      this->readData();                                                                         // Read the gyro output.
      this->gyro_roll_cal += gyro_roll;                                                         // Ad roll value to gyro_roll_cal.
      this->gyro_pitch_cal += gyro_pitch;                                                       // Ad pitch value to gyro_pitch_cal.
      this->gyro_yaw_cal += gyro_yaw;                                                           // Ad yaw value to gyro_yaw_cal.
      delay(4);                                                                                 // Small delay to simulate a 250Hz loop during calibration.
    }

    //Now that we have 2000 measures, we need to devide by 2000 to get the average gyro offset.
    this->gyro_roll_cal  /= 2000;                                                               // Divide the roll total by 2000.
    this->gyro_pitch_cal /= 2000;                                                               // Divide the pitch total by 2000.
    this->gyro_yaw_cal   /= 2000;                                                               // Divide the yaw total by 2000.
    this->manual_gyro_pitch_cal_value = this->gyro_pitch_cal;                                   // Set the manual pitch calibration variable to the detected value.
    this->manual_gyro_roll_cal_value  = this->gyro_roll_cal;                                    // Set the manual roll calibration variable to the detected value.
    this->manual_gyro_yaw_cal_value   = this->gyro_yaw_cal;                                     // Set the manual yaw calibration variable to the detected value.
  }


  /* for (int cal_int = 0; cal_int < 2000 ; cal_int ++){                  // Run this code 2000 times
    if(cal_int % 125 == 0) digitalWrite(RedLed, !digitalRead(RedLed)); // Print a dot on the LCD every 125 readings
    this->readData();                                                  // Read the raw acc and gyro data from the MPU-6050
    this->gyro_x_cal += this->gyro_roll;                               // Add the gyro x-axis offset to the gyro_x_cal variable
    this->gyro_y_cal += this->gyro_pitch;                              // Add the gyro y-axis offset to the gyro_y_cal variable
    this->gyro_z_cal += this->gyro_yaw;                                // Add the gyro z-axis offset to the gyro_z_cal variable
    delay(3);                                                          // Delay 3us to simulate the 250Hz program loop
  }
  // Serial.println();
  this->gyro_x_cal /= 2000;                                            // Divide the gyro_x_cal variable by 2000 to get the avarage offset
  this->gyro_y_cal /= 2000;                                            // Divide the gyro_y_cal variable by 2000 to get the avarage offset
  this->gyro_z_cal /= 2000;                                            // Divide the gyro_z_cal variable by 2000 to get the avarage offset */
}

/* Read data in real-time */
void Gyro::readData(void){
  Wire.beginTransmission(this->address);                                     //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                          //Send the requested starting register
  Wire.endTransmission();                                                    //End the transmission
  Wire.requestFrom(0x68, 14);                                                //Request 14 bytes from the MPU-6050

  while(Wire.available() < 14);                                              //Wait until all the bytes are received

  // Wire.read()<<8|Wire.read() => That means, use first 8 bits and shift it to the left, then add another 8 bits
  this->acc_y = Wire.read() << 8 | Wire.read();                              // Add the low and high byte to the acc_x variable
  this->acc_x = Wire.read() << 8 | Wire.read();                              // Add the low and high byte to the acc_y variable
  this->acc_z = Wire.read() << 8 | Wire.read();                              // Add the low and high byte to the acc_z variable

  this->temperature = Wire.read() <<8 | Wire.read();                         // Add the low and high byte to the temperature variable
  
  this->gyro_roll  = Wire.read() << 8 | Wire.read();                         // Add the low and high byte to the gyro_x variable
  this->gyro_pitch = Wire.read() << 8 | Wire.read();                         // Add the low and high byte to the gyro_y variable
  this->gyro_yaw   = Wire.read() << 8 | Wire.read();                         // Add the low and high byte to the gyro_z variable

  // this->gyro_pitch *= -1;                                                    // Invert the direction of the axis.
  this->gyro_yaw   *= -1;                                                    // Invert the direction of the axis.

  this->acc_y -= this->manual_acc_pitch_cal_value;                           // Substact the manual accelerometer pitch calibration value.
  this->acc_x -= this->manual_acc_roll_cal_value;                            // Substact the manual accelerometer roll calibration value.
  this->gyro_roll  -= this->manual_gyro_roll_cal_value;                      // Substact the manual gyro roll calibration value.
  this->gyro_pitch -= this->manual_gyro_pitch_cal_value;                     // Substact the manual gyro pitch calibration value.
  this->gyro_yaw   -= this->manual_gyro_yaw_cal_value;                       // Substact the manual gyro yaw calibration value.
}

/* Calculate PID */
void Gyro::calculatePID(void){
  //Roll calculations
  this->pid_error_temp = this->gyro_roll_input - this->pid_roll_setpoint;
  this->pid_i_mem_roll += this->pid_i_gain_roll * this->pid_error_temp;
  if(this->pid_i_mem_roll > this->pid_max_roll) this->pid_i_mem_roll = this->pid_max_roll;
  else if(this->pid_i_mem_roll < this->pid_max_roll * -1) this->pid_i_mem_roll = this->pid_max_roll * -1;

  this->pid_output_roll = this->pid_p_gain_roll * this->pid_error_temp + this->pid_i_mem_roll + this->pid_d_gain_roll * (this->pid_error_temp - this->pid_last_roll_d_error);
  if(this->pid_output_roll > this->pid_max_roll) this->pid_output_roll = this->pid_max_roll;
  else if(this->pid_output_roll < this->pid_max_roll * -1) this->pid_output_roll = this->pid_max_roll * -1;

  this->pid_last_roll_d_error = this->pid_error_temp;

  //Pitch calculations
  this->pid_error_temp = this->gyro_pitch_input - this->pid_pitch_setpoint;
  this->pid_i_mem_pitch += this->pid_i_gain_pitch * this->pid_error_temp;
  if(this->pid_i_mem_pitch > this->pid_max_pitch) this->pid_i_mem_pitch = this->pid_max_pitch;
  else if(this->pid_i_mem_pitch < this->pid_max_pitch * -1) this->pid_i_mem_pitch = this->pid_max_pitch * -1;

  this->pid_output_pitch = this->pid_p_gain_pitch * this->pid_error_temp + this->pid_i_mem_pitch + this->pid_d_gain_pitch * (this->pid_error_temp - this->pid_last_pitch_d_error);
  if(this->pid_output_pitch > this->pid_max_pitch) this->pid_output_pitch = this->pid_max_pitch;
  else if(this->pid_output_pitch < this->pid_max_pitch * -1) this->pid_output_pitch = this->pid_max_pitch * -1;

  this->pid_last_pitch_d_error = this->pid_error_temp;

  //Yaw calculations
  this->pid_error_temp =  this->gyro_yaw_input - this->pid_yaw_setpoint;
  this->pid_i_mem_yaw  += this->pid_i_gain_yaw * this->pid_error_temp;
  if(this->pid_i_mem_yaw > this->pid_max_yaw)pid_i_mem_yaw = this->pid_max_yaw;
  else if(this->pid_i_mem_yaw < this->pid_max_yaw * -1) this->pid_i_mem_yaw = this->pid_max_yaw * -1;

  this->pid_output_yaw = this->pid_p_gain_yaw * this->pid_error_temp + this->pid_i_mem_yaw + this->pid_d_gain_yaw * (this->pid_error_temp - this->pid_last_yaw_d_error);
  if(this->pid_output_yaw > this->pid_max_yaw) this->pid_output_yaw = this->pid_max_yaw;
  else if(this->pid_output_yaw < this->pid_max_yaw * -1) this->pid_output_yaw = this->pid_max_yaw * -1;

  this->pid_last_yaw_d_error = this->pid_error_temp;
}