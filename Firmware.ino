
#include <Wire.h>
#include "BYJ48.h"
#include "SPQueue.h"
#include "VL53L0X.h"

#define ES_PIN1 2 // Pin para endstop de direccion X
#define ES_PIN2 3 // Pin para endstop de direccion Y
#define OK_PIN A0 // Pin para led de encendido
#define ERR_PIN A1 // Pin para led de errores
#define MAXPOSX 14800 // Maxima posicion a desplazarse en X
#define MAXPOSY 14000 // Maxima posicion a desplazarse en Y

// Sensor laser
VL53L0X sensor;

// Motores
BYJ48 motor1(8,9,10,11);
BYJ48 motor2(4,5,6,7);

// Cola de setpoints
SPQueue *queue = 0;

int pos[2] = {MAXPOSX,MAXPOSY}; // Posicion inicial del sensor (se asume)
int* sp; // Setpoint (inicialmente debe ester en 0,0)
int vSweep = 500; // Pasos de separacion entre cada pasada horizontal (divisor de MAXPOSY)


void setup(){
  Serial.begin(9600); // Inicializar interface uart (para bluetooth o usb)

  // Inicializar sensor laser
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500); // Maximo periodo de espera
  sensor.setMeasurementTimingBudget(200000); // Modo high accuracy

  pinMode(ES_PIN1,INPUT);
  pinMode(ES_PIN2,INPUT);
  pinMode(OK_PIN,OUTPUT);
  pinMode(ERR_PIN,OUTPUT);

  // Verificar estado de los interruptores de fin de carrera
  if(digitalRead( ES_PIN1 ) == LOW) pos[0] = 0;
  if(digitalRead( ES_PIN2 ) == LOW) pos[1] = 0;

  goHome(); // Se ingresa como setpoint el origen (0,0)

  noInterrupts();
  // Configuracion del TIMER 2
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
  attachInterrupt(0, es1_activated, FALLING); // Int. para pin 2
  attachInterrupt(1, es2_activated, FALLING); // Int. para pin 3

  // Encender led de status
  digitalWrite(OK_PIN,HIGH);
}

void es1_activated()
// Interrupcion por activacion del endstop de direccion X
{
  pos[0] = 0; // Restablecer posicion horizontal
}

void es2_activated()
// Interrupcion por activacion del endstop de direccion Y
{
  pos[1] = 0; // Restablecer posicion vertical
}

ISR(TIMER2_COMPA_vect)
// Interrupcion por timer 2 (10 ms) - Control de motores
{
  if(queue == 0) return; // No hacer nada si el objeto no esta creado

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
    motor1.stepCCW();
    motor2.stepCW();
    pos[1]++;
    return;
  }
  if(pos[1] > sp[1]){ // Mover abajo
    motor1.stepCW();
    motor2.stepCCW();
    pos[1]--;
    return;
  }
}

void scanningQueue()
// Crear cola de posiciones para barrido izq-der y abajo-arriba
{
  // Borrar y generar lista
  if(queue != 0)
    delete queue;
  queue = new SPQueue(2*round(MAXPOSY/vSweep)+10); // Inicializar cantidad de setpoints +10 por seguridad

  for(int vPos = 0; vPos <= MAXPOSY;) // Para cada pasada (incrementar dentro del loop)
  {
    // Ir de derecha a izquierda
    queue->push(0,vPos);
    queue->push(MAXPOSX,vPos);
    vPos+=vSweep; // Siguiente pasada
    // Volver de izquierda a derecha
    queue->push(MAXPOSX,vPos);
    queue->push(0,vPos);
    vPos+=vSweep; // Siguiente pasada
  }
  sp = queue->pop(); // Asignar el primero para empezar
}

void allStop()
// Detener y borrar cola de setpoints
{
  if(queue != 0)
    delete queue;
  queue = new SPQueue(2); // Se precisa solo un elemento, pero agrego otro por seguridad
  queue->push(pos[0],pos[1]); // Ir al origen
  sp = queue->pop();  
}

void goHome()
// Volver al 0,0
{
  if(queue != 0)
    delete queue;
  queue = new SPQueue(2); // Se precisa solo un elemento, pero agrego otro por seguridad
  queue->push(0,0); // Ir al origen
  sp = queue->pop();
}

void error()
// Si algo sale mal, desactivar todo y encender led de error
{
  if(queue != 0)
    delete queue;
  noInterrupts();
  digitalWrite(OK_PIN,LOW);
  digitalWrite(ERR_PIN,HIGH);
  while(1); // Loop infinito
}

void serialEvent()
// Interrupcion de puerto serie
{
  String arg;
  switch((char) Serial.read()){
   case 'a': // Iniciar escaneo
     scanningQueue();
     break;
   case 'b': // Detener
     allStop();
     break;
   case 'c': // Volver a origen
     goHome();
     break;
   default: 
     break;
  }
}

void loop(){
  // Mostrar posision y lectura por serie
  Serial.print(pos[0]);
  Serial.print(",");
  Serial.print(pos[1]);
  Serial.print(",");
//  if (sensor.timeoutOccurred()) { Serial.println("ST"); }
  Serial.println(sensor.readRangeSingleMillimeters());

  delay(500);
}
