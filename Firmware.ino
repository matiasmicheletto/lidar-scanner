
#include <Wire.h>
#include "BYJ48.h"
#include "SPQueue.h"
#include "VL53L0X.h"

#define ES_PIN1 2 // Pin para endstop de direccion X
#define ES_PIN2 3 // Pin para endstop de direccion Y

#define MAXPOSX 5000 // Maxima posicion a desplazarse en X
#define MAXPOSY 5000 // Maxima posicion a desplazarse en Y

// Sensor laser
VL53L0X sensor;

// Motores
BYJ48 motor1(8,9,10,11);
BYJ48 motor2(4,5,6,7);

// Cola de setpoints
SPQueue *queue;

int pos[2] = {MAXPOSX,MAXPOSY}; // Posicion inicial del sensor (se asume)
int* sp; // Setpoint (inicialmente debe ester en 0,0)

void setup(){
  Serial.begin(9600);
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensor.setMeasurementTimingBudget(200000); // Modo high accuracy
  
  // En la inicializacion se debe setear el setpoint en 0,0 y
  // como posicion actual en MAXPOS. Luego, cuando los endstop se 
  // activen, se setea el origen.
  
  // Prueba lista
  queue = new SPQueue(10);
  queue->push(3000,0);
  queue->push(3000,3000);
  queue->push(3000,2000);
  queue->push(0,0);
  queue->push(1000,3000);
  sp = queue->pop();
  
  pinMode(ES_PIN1,INPUT);
  pinMode(ES_PIN2,INPUT);
  
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
  
  // Interrupciones para endstops
  attachInterrupt(0, es1_activated, RISING);
  attachInterrupt(1, es2_activated, RISING);
}

void es1_activated()
// Interrupcion por activacion del endstop de direccion X
{
  pos[0] = 0;
}

void es2_activated()
// Interrupcion por activacion del endstop de direccion Y
{
  pos[1] = 0;
}

ISR(TIMER2_COMPA_vect)
// Interrupcion por timer 2 (10 ms)
{ 
  if(pos[0] == sp[0] && pos[1] == sp[1]){ // Si se alcanza el setpoint
    if(queue->itemCount() > 0) // Si hay otro setpoint al que ir
      sp = queue->pop(); // Sacar nuevo setpoint de la cola
    else{ // Si no hay mas setpoints, apagar steppers y salir
      motor1.disable();
      motor2.disable();
      return;
    }
  }
  if(pos[0] < sp[0]){ // Mover izquierda
    motor1.stepCW();
    motor2.stepCW();
    pos[0]++;
    return;
  }
  if(pos[0] > sp[0]){ // Mover derecha
    motor1.stepCCW();
    motor2.stepCCW();
    pos[0]--;
    return;
  }
  if(pos[1] < sp[1]){ // Mover arriba
    motor1.stepCW();
    motor2.stepCCW();
    pos[1]++;
    return;
  }
  if(pos[1] > sp[1]){ // Mover abajo
    motor1.stepCCW();
    motor2.stepCW();
    pos[1]--;
    return;
  }
}



void loop(){
  Serial.print("Pos: (");
  Serial.print(pos[0]);
  Serial.print(",");
  Serial.print(pos[1]);
  Serial.println(")");
  Serial.print(sensor.readRangeSingleMillimeters());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();
  delay(500);
}
