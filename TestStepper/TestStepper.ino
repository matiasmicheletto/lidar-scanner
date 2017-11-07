#include "BYJ48.h"

BYJ48 motor(8,9,10,11);

void setup(){
  Serial.begin(9600);
}

void loop(){
  for(int j = 2000; j>0; j-=100){
    for(int i = 0; i<5000; i++){
      motor.stepCW();
      delayMicroseconds(j);
    }
    Serial.print("Velocidad actual: ");
    Serial.println(j);
  }
}
