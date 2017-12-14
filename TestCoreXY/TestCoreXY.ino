#include "BYJ48.h"

BYJ48 motor1(8,9,10,11);
BYJ48 motor2(4,5,6,7);

void setup(){}

void loop(){
  for(int step = 0; step < 4096; step++){
    motor1.stepCW();
    motor2.stepCW();
    delay(1);
  }
  for(int step = 0; step < 4096; step++){
    motor1.stepCW();
    motor2.stepCCW();
    delay(1);
  }
  for(int step = 0; step < 4096; step++){
    motor1.stepCCW();
    motor2.stepCW();
    delay(1);
  }
  for(int step = 0; step < 4096; step++){
    motor1.stepCCW();
    motor2.stepCCW();
    delay(1);
  }
}
