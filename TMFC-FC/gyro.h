#include <Wire.h>
#ifndef gyro_h
#define gyro_h

#define GreenLed 12
#define RedLed 14

class Gyro{
  protected:
    uint8_t address = 0x68;

  public:
    int calItterations = 2000;
    bool auto_level    = true;
    /* Set to false or true; */
    uint8_t use_manual_calibration = false;

    /*
     *  Gyro global variables
     */
    int16_t gyro_roll, gyro_pitch, gyro_yaw, acc_x, acc_y, acc_z;
    // long acc_total_vector;
    int32_t acc_total_vector;
    int32_t gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;
    
    int temperature;
    long gyro_x_cal, gyro_y_cal, gyro_z_cal;

    float angle_pitch = 0, angle_roll = 0;
    bool set_gyro_angles;
    float angle_roll_acc = 0, angle_pitch_acc = 0;
    float angle_pitch_output, angle_roll_output;


    int16_t manual_gyro_pitch_cal_value = 0, manual_gyro_roll_cal_value = 0, manual_gyro_yaw_cal_value = 0;
    int16_t manual_acc_pitch_cal_value = 0,  manual_acc_roll_cal_value = 0;

    float roll_level_adjust, pitch_level_adjust;
    float pid_error_temp;
    float pid_i_mem_roll, pid_roll_setpoint, gyro_roll_input, pid_output_roll, pid_last_roll_d_error;
    float pid_i_mem_pitch, pid_pitch_setpoint, gyro_pitch_input, pid_output_pitch, pid_last_pitch_d_error;
    float pid_i_mem_yaw, pid_yaw_setpoint, gyro_yaw_input, pid_output_yaw, pid_last_yaw_d_error;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //PID gain and limit settings
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    float pid_p_gain_roll = 1.3;               //Gain setting for the pitch and roll P-controller (default = 1.3).
    float pid_i_gain_roll = 0.04;              //Gain setting for the pitch and roll I-controller (default = 0.04).
    float pid_d_gain_roll = 18.0;              //Gain setting for the pitch and roll D-controller (default = 18.0).
    int pid_max_roll = 400;                    //Maximum output of the PID-controller (+/-).

    float pid_p_gain_pitch = pid_p_gain_roll;  //Gain setting for the pitch P-controller.
    float pid_i_gain_pitch = pid_i_gain_roll;  //Gain setting for the pitch I-controller.
    float pid_d_gain_pitch = pid_d_gain_roll;  //Gain setting for the pitch D-controller.
    int pid_max_pitch = pid_max_roll;          //Maximum output of the PID-controller (+/-).

    float pid_p_gain_yaw = 4.0;                //Gain setting for the pitch P-controller (default = 4.0).
    float pid_i_gain_yaw = 0.02;               //Gain setting for the pitch I-controller (default = 0.02).
    float pid_d_gain_yaw = 0.0;                //Gain setting for the pitch D-controller (default = 0.0).
    int pid_max_yaw = 400;                     //Maximum output of the PID-controller (+/-).

    void setupRegisters(void);
    void runCalibration(void);

    void readData(void);
    void calculatePID(void);

    void armDrone(){
      
    }
    void disarmDrone(){
      
    }
};

#endif