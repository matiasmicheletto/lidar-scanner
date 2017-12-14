
#include "BYJ48.h"

// Motores
BYJ48 motor1(8,9,10,11);
BYJ48 motor2(4,5,6,7);

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
  if(posX == spX && posY == spY){
    motor1.disable();
    motor2.disable();
    ready = true;
    return;
  }
  if(posX < spX){ // Mover izquierda
    motor1.stepCW();
    motor2.stepCW();
    posX++;
    return;
  }
  if(posX > spX){ // Mover derecha
    motor1.stepCCW();
    motor2.stepCCW();
    posX--;
    return;
  }
  if(posY < spY){ // Mover arriba
    motor1.stepCW();
    motor2.stepCCW();
    posY++;
    return;
  }
  if(posY > spY){ // Mover abajo
    motor1.stepCCW();
    motor2.stepCW();
    posY--;
    return;
  }
}

void loop(){
  spX = 3000;
  spY = 0;
  delay(5000);
  spX = 0;
  spY = 0;
  delay(5000);
  spX = 1500;
  spY = 1500;
  delay(5000);
}
