
#include "BYJ48.h"

// Motores
BYJ48 motor1(4,5,6,7);
BYJ48 motor2(8,9,10,11);


int posX = 0, posY = 0, spX = 0, spY = 0; // Posicion y setpoint
boolean ready = true; // Estado

void setup(){

  Serial.begin(9600);

  noInterrupts();
  // TIMER 2
  TCCR2A = 0; // Normal operation
  TCCR2B = 0; // Normal operation
  TCNT2 = 0; // Inicializar en 0
  OCR2A = 16; // Registro de comparacion = 16MHz/1024/1kHz ()
  TCCR2A |= (1 << WGM21); // Modo CTC
  TCCR2B |= (1 << CS20); // 1024 prescaler
  TCCR2B |= (1 << CS21); // 1024 prescaler
  TCCR2B |= (1 << CS22); // 1024 prescaler
  TIMSK2 |= (1 << OCIE2A); // Habilitar int.
  interrupts();
}

ISR(TIMER2_COMPA_vect)
// Interrupcion por timer 2 (10 ms)
{
  // int dX = spX-posX;
  // int dY = spY-posY;
  short r1 = 0;
  short r2 = 0;
  int dM1 = spX-posX+spY-posY; // dX+dY
  int dM2 = spX-posX-spY+posY; // dX-dY

  if(dM1 > 0){
    motor1.stepCW();
    r1++;
  }
  else if(dM1 < 0){
    motor1.stepCCW();
    r1--;
  }
  else // dM1 == 0
    motor1.disable();

  if(dM2 > 0){
    motor2.stepCW();
    r2++;
  }
  else if(dM2 < 0){
    motor2.stepCCW();
    r2--;
  }
  else // dM2 == 0;
    motor2.disable();
    
  posX += (r1+r2)/2;
  posY += (r1-r2)/2;
}

void printAll(){
  Serial.print("Pos = ("); Serial.print(posX);
  Serial.print(","); Serial.print(posY); 
  Serial.print(") -- SP = ("); Serial.print(spX);
  Serial.print(","); Serial.print(spY);  
  Serial.println(")");
  delay(100);
}

void loop(){
  spX = 3000;
  spY = 0;
  while(spX != posX || spY != posY)
    printAll();    
  spX = 3000;
  spY = 3000;
  while(spX != posX || spY != posY)
    printAll();
  spX = 0;
  spY = 3000;
  while(spX != posX || spY != posY)
    printAll();
  spX = 0;
  spY = 0;
  while(spX != posX || spY != posY)
    printAll();
}
