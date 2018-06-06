/**
  ******************************************************************************
  *  Name        : Firmware.ino
  *  Author      : Matias Micheletto
  *  Version     : 1.0
  *  Copyright   : GPLv3
  *  Description : Firmware de scanner lidar de cobertura vegetal
  ******************************************************************************
  *
  *  Copyright (c) 2018
  *  Matias Micheletto <matias.micheletto@uns.edu.ar>
  *  Departamento de Ingeniería Eléctrica - Universidad Nacional del Sur
  *  Laboratorio de Sistemas Digitales
  *
  *
  *  This program is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 3 of the License.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

#include <Wire.h>
#include "BYJ48.h"
#include "SPQueue.h"
#include "VL53L0X.h"

#define ES_PIN1 2 // Pin para endstop de direccion X
#define ES_PIN2 3 // Pin para endstop de direccion Y
#define OK_PIN A0 // Pin para led de encendido
#define ERR_PIN A1 // Pin para led de errores
#define MAXPOSX 14900 // Maxima posicion a desplazarse en X
#define MAXPOSY 14400 // Maxima posicion a desplazarse en Y

// Sensor laser
VL53L0X sensor;

// Motores
//BYJ48 motor1(8,9,10,11);
//BYJ48 motor2(4,5,6,7);

BYJ48 motor1(11,10,9,8);
BYJ48 motor2(7,6,5,4);

// Cola de setpoints
SPQueue *queue = 0;

boolean scanning = false;
int pos[2] = {MAXPOSX,MAXPOSY}; // Posicion inicial del sensor (se asume)
int* sp; // Setpoint (inicialmente debe ester en 0,0)
int vSweep = 200; // Pasos de separacion entre cada pasada horizontal (divisor de MAXPOSY)
byte motorSpeed = 12; // Valor del registro de comparacion para ISR valor = 16MHz/1024/freq 
int samplePeriod = 200; // Periodo de muestreo del sensor
unsigned long previousMillis = 0;

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
  OCR2A = motorSpeed; // Registro de comparacion
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

  
  /*
  // Configuracion del modulo
  delay(1000);
  Serial.println("AT");
  delay(200);
  Serial.println("AT+ROLE0"); // Slave
  delay(200);
  Serial.println("AT+UUID0xFFE0"); // UUID
  delay(200);
  Serial.println("AT+CHAR0xFFE1"); // Characteristic
  delay(200);
  Serial.println("AT+NAMEScanner"); // Name
  delay(200);
  */
}

void es1_activated(){ // Interrupcion por activacion del endstop de direccion X
  pos[0] = 0; // Restablecer posicion horizontal
}

void es2_activated(){ // Interrupcion por activacion del endstop de direccion Y
  pos[1] = 0; // Restablecer posicion vertical
}

ISR(TIMER2_COMPA_vect){ // Interrupcion por timer 2 (1 ms) - Control de motores
  if(queue == 0) return; // No hacer nada si el objeto no esta creado

  if(pos[0] == sp[0] && pos[1] == sp[1]){ // Si se alcanza el setpoint
    if(queue->itemCount() > 0) // Si hay otro setpoint al que ir
      sp = queue->pop(); // Sacar nuevo setpoint de la cola
    else{ // Si no hay mas setpoints, apagar steppers y salir
      motor1.disable();
      motor2.disable();
      scanning = false;
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

void scanningQueue(){ // Crear cola de posiciones para barrido izq-der y abajo-arriba
  scanning = false;
  // Borrar y generar lista
  if(queue != 0)
    delete queue;
  queue = new SPQueue(2*round(MAXPOSY/vSweep)+5); // Inicializar cantidad de setpoints +5 por seguridad

  for(int vPos = 0; vPos <= MAXPOSY-vSweep;){ // Para cada pasada (incrementar dentro del loop)
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
  scanning = true;
}

void allStop(){ // Detener y borrar cola de setpoints
  scanning = false;
  if(queue != 0)
    delete queue;
  queue = new SPQueue(2); // Se precisa solo un elemento, pero agrego otro por seguridad
  queue->push(pos[0],pos[1]);
  sp = queue->pop();  
}

void goHome(){ // Volver al 0,0
  scanning = false;
  if(queue != 0)
    delete queue;
  queue = new SPQueue(2); // Se precisa solo un elemento, pero agrego otro por seguridad
  queue->push(0,0); // Ir al origen
  sp = queue->pop();
}



void configureSpeed(byte timerCnt){ // Cambiar la velocidad de los motores
  if(scanning) allStop(); // Si estaba escaneando, detener
  motorSpeed = timerCnt; // Actualizar variable global
  noInterrupts();
  // Configuracion del TIMER 2
  TCCR2A = 0; // Normal operation
  TCCR2B = 0; // Normal operation
  TCNT2 = 0; // Inicializar en 0
  OCR2A = timerCnt; // Registro de comparacion = 16MHz/1024/freq
  TCCR2A |= (1 << WGM21); // Modo CTC
  TCCR2B |= (1 << CS20); // 1024 prescaler
  TCCR2B |= (1 << CS21); // 1024 prescaler
  TCCR2B |= (1 << CS22); // 1024 prescaler
  TIMSK2 |= (1 << OCIE2A); // Habilitar int.
  interrupts();
}

void error(){ // Si algo sale mal, desactivar todo y encender led de error
  if(queue != 0)
    delete queue;
  noInterrupts();
  digitalWrite(OK_PIN,LOW);
  digitalWrite(ERR_PIN,HIGH);
  while(1); // Quedar aca hasta reiniciar
}

void serialEvent(){ // Interrupcion de puerto serie

  String arg;
  switch((char) Serial.read()){
    
    // Orden directa
    case 'a': // Iniciar escaneo     
      scanningQueue();
      Serial.println("a"); // Ack
      break;
    case 'b': // Detener     
      allStop();
      Serial.println("b"); // Ack
      break;
    case 'c': // Volver a origen     
      goHome();
      Serial.println("c"); // Ack
      break;

    // Solicitudes
    case 'd': // Posiciones maximas
      Serial.print("d");
      Serial.print(MAXPOSX);
      Serial.print(",");
      Serial.println(MAXPOSY);
      break;
    case 'e': // Velocidad de los steppers
      Serial.print("e");
      Serial.println(motorSpeed);
      break;
    case 'f': // Periodo de muestreo del sensor
      Serial.print("f");
      Serial.println(samplePeriod);
      break;
    case 'g': // Avance vertical
      Serial.print('g');
      Serial.println(vSweep);
      break;
    case 'h': // Posicion actual
      Serial.print('h');
      Serial.print(pos[0]);
      Serial.print(",");
      Serial.println(pos[1]);
      break;    

    // Comandos con argumento
    case 'i': // Agregar setpoint a la cola
      arg = Serial.readStringUntil('\n'); // Leer argumento
      int sep,x,y;
      sep = arg.indexOf(","); // Separador
      x = arg.substring(0,sep).toInt();
      y = arg.substring(sep+1).toInt();
      boolean res; // Resultado de intentar poner un nuevo setpoint
      if(queue != 0) // Si la cola esta creada
        res = queue->push(x,y);
      if(res){ // Si se pudo hacer la operacion
        if(queue->itemCount() == 1) // Si es el unico set point, ir a ese
          sp = queue->pop();
        Serial.print("i"); // Ack
        Serial.print(x); // Ack
        Serial.print(","); // Ack
        Serial.println(y); // Ack
      }
      break;
    case 'j': // Cambiar velocidad de los steppers
      arg = Serial.readStringUntil('\n'); // Leer argumento
      configureSpeed( (byte) arg.toInt() ); // Pasar argumento
      Serial.println("j"); // Ack
      break;
    case 'k': // Cambiar periodo de muestreo del sensor
      arg = Serial.readStringUntil('\n'); // Leer argumento
      samplePeriod = arg.toInt();
      Serial.println("k"); // Ack
      break;
    case 'l': // Cambiar el avance vertical
      arg = Serial.readStringUntil('\n'); // Leer argumento
      vSweep = arg.toInt();
      Serial.println("l"); // Ack
      break;
    default: 
      break;
   }
 }

 void loop(){
  if (millis() - previousMillis >= samplePeriod) {
    previousMillis = millis();
    if(scanning){
      // Mostrar posision y lectura por serie
      Serial.print("a"); // Prefijo del dato
      Serial.print(pos[0]);
      Serial.print(",");
      Serial.print(pos[1]);
      Serial.print(",");
      //  if (sensor.timeoutOccurred()) { Serial.println("ST"); }
      Serial.println(sensor.readRangeSingleMillimeters());
    }
  }
}
