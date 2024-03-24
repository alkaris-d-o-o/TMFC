void escs_cfg(void){
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
}

void escsFullThrottle(void){
  ledcWrite(ledcCH1, 6553);
  ledcWrite(ledcCH2, 6553);
  ledcWrite(ledcCH3, 6553);
  ledcWrite(ledcCH4, 6553);
}
void escsLowThrottle(void){
  ledcWrite(ledcCH1, 3275);
  ledcWrite(ledcCH2, 3275);
  ledcWrite(ledcCH3, 3275);
  ledcWrite(ledcCH4, 3275);
}