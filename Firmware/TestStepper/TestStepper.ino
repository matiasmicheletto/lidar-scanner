#include "BYJ48.h"

BYJ48 motor(8,9,10,11,false);

void setup(){
  Serial.begin(9600);
}

void loop(){
    for(int step = 0; step < 2048; step++){
        motor.stepCW();
        if(step < 10)
            delay(500);
        else if(step < 30)
            delay(250);
        else if(step < 70)
            delay(100);
        else if(step < 280)
            delay(25);
        else if(step < 500)
            delay(12);
        else if(step < 1000)
            delay(6);
        else 
            delay(3);
    }

    for(int step = 0; step < 2048; step++){
        motor.stepCCW();
        if(step < 10)
            delay(500);
        else if(step < 30)
            delay(250);
        else if(step < 70)
            delay(100);
        else if(step < 280)
            delay(25);
        else if(step < 500)
            delay(12);
        else if(step < 1000)
            delay(6);
        else 
            delay(3);
    }
}
