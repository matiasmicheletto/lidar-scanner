#include "BYJ48.h"

BYJ48 motor(8,9,10,11);

void setup(){
  Serial.begin(9600);
}

void loop(){
  for(int step = 0; step < 4096; step++){
    motor.stepCW();
    delay(10);
  }
  for(int step = 0; step < 4096; step++){
    motor.stepCCW();
    delay(1);
  }
}
