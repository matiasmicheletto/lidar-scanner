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
#include "VL53L0X.h"

#define ES_PIN1 2 // Pin para endstop de direccion X
#define ES_PIN2 3 // Pin para endstop de direccion Y
//#define OK_PIN A0 // Pin para led de encendido
//#define ERR_PIN A1 // Pin para led de errores
#define MAXPOSX 14600 // Maxima posicion a desplazarse en X (446mm)
#define MAXPOSY 13300 // Maxima posicion a desplazarse en Y (410mm)

// Sensor laser
VL53L0X sensor;

// Motores
BYJ48 motor1(4,5,6,7); // Motor izq.
BYJ48 motor2(8,9,10,11); // Motor der.

boolean scanning = false; // Habilita o deshabilita el muestreo del sensor
boolean moving = false; // Habilita las instrucciones de la ISR del TIMER2 (Para accionar motores)
int pos[2] = {MAXPOSX,MAXPOSY}; // Posicion inicial del sensor (se asume)
int sp[2] = {0,0}; // Setpoint (inicialmente debe ester en 0,0)
int hSweep = 200; // Pasos de separacion entre cada pasada horizontal (divisor de MAXPOSY)
byte motorSpeed = 12; // Valor defeecto del registro de comparacion para ISR valor = 16MHz/1024/freq 
int samplePeriod = 200; // Periodo de muestreo del sensor
unsigned long previousMillis = 0;

void setup(){

  // Inicializar sensor laser
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500); // Maximo periodo de espera
  sensor.setMeasurementTimingBudget(200000); // Modo high accuracy

  pinMode(ES_PIN1,INPUT);
  pinMode(ES_PIN2,INPUT);
  //pinMode(OK_PIN,OUTPUT);
  //pinMode(ERR_PIN,OUTPUT);

  Serial.begin(9600); // Inicializar interface uart (para bluetooth o usb)
  delay(500);

  goHome(); // Se ingresa como setpoint el origen (0,0)

  // Para que no se mueva en el inicio. Si se desea volver al home se puede con el comando correspondiente
  pos[0] = 0;
  pos[1] = 0;

  // Configurar registros de interrupcion
  configureSpeed(motorSpeed);

  // Encender led de status
  //digitalWrite(OK_PIN,HIGH);

  
  /*
  // Configuracion del modulo
  delay(1000);
  Serial.println("AT");
  delay(500);
  Serial.println("AT+ROLE0"); // Slave
  delay(500);
  Serial.println("AT+UUID0xFFE0"); // UUID
  delay(500);
  Serial.println("AT+CHAR0xFFE1"); // Characteristic
  delay(500);
  Serial.println("AT+NAMEScanner"); // Name
  delay(500);
  */
}

void es1_activated(){ // Interrupcion por activacion del endstop de direccion X
  pos[0] = 0; // Restablecer posicion horizontal
}

void es2_activated(){ // Interrupcion por activacion del endstop de direccion Y
  pos[1] = 0; // Restablecer posicion vertical
}

ISR(TIMER2_COMPA_vect){ // Interrupcion por timer 2 - Control de motores
  if(moving){ // Para deshabilitar el uso de la cola de setpoint en interrupciones
    if(pos[0] == sp[0] && pos[1] == sp[1]){ // Si se alcanza el setpoint
      if(scanning && pos[0] < MAXPOSX){ // Si hay otro setpoint al que ir
        // Poner siguiente sp
        if(pos[1] == MAXPOSY){          
          sp[0] = pos[0]+hSweep;
          sp[1] = 0;
        }else{
          sp[0] = pos[0]+hSweep;
          sp[1] = MAXPOSY;
        }
      }else{ // Si no hay mas setpoints, apagar steppers y salir
        motor1.disable();
        motor2.disable();
	      moving = false; // Deshabilitar el cuerpo de la ISR para no usar cola
        scanning = false; // Deshabilitar el envio de datos
        return;
      }
    }
    if(pos[0] < sp[0]){ // Mover izquierda
      motor1.stepCCW();
      motor2.stepCCW();
      pos[0]++;
      return;
    }
    if(pos[0] > sp[0]){ // Mover derecha
      motor1.stepCW();
      motor2.stepCW();
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
}

void startScanning(){ // Crear cola de posiciones para barrido izq-der y abajo-arriba iniciando desde la pos. vert. actual
  scanning = false; // Deberia estar en false antes
  moving = false; // Deshabilitar cuerpo de ISR

  sp[0] = 0;
  sp[1] = MAXPOSY;

  // Al iniciar escaneo, deshabilitar interrupciones de sensores de fin de carrera por si se desconectan
  detachInterrupt(0);
  detachInterrupt(1);
  scanning = true;
  moving = true;
}

void allStop(){ // Detener y borrar cola de setpoints
  scanning = false; // Detener muestreo de distancia
  moving = false; // Para que no interrumpa durante la operacion sigte.
  motor1.disable();
  motor2.disable();
}

void goHome(){ // Volver al 0,0
  // Deshabilitar medicion de distancia y cuerpo de ISR
  scanning = false;
  moving = false;

  // Reiniciar la posicion por si hubo patinamiento
  pos[0] = MAXPOSX;
  pos[1] = MAXPOSY;

  // Verificar estado de los interruptores de fin de carrera (Tienen logica negativa)
  if(digitalRead( ES_PIN1 ) == LOW) pos[0] = 0;
  if(digitalRead( ES_PIN2 ) == LOW) pos[1] = 0;

  // Habilitar interrupciones para endstops
  attachInterrupt(0, es1_activated, FALLING); // Int. para pin 2
  attachInterrupt(1, es2_activated, FALLING); // Int. para pin 3

  sp[0] = 0; sp[1] = 0;

  moving = true; // Habilitar nuevamente el cuerpo de la ISR
}


void configureSpeed(byte timerCnt){ // Cambiar la velocidad de los motores
  moving = false; // Deshabilitar el cuerpo de la ISR (Aunque luego se deshabilita la ISR completa)
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
  interrupts(); // Habilitar interrupciones nuevamente
  moving = true; // Habilitar ejecucion del cuerpo de la ISR
}

/*
void error(){ // Si algo sale mal, desactivar todo y encender led de error
  if(queue != 0)
    delete queue;
  noInterrupts();
  digitalWrite(OK_PIN,LOW);
  digitalWrite(ERR_PIN,HIGH);
  while(1); // Quedar aca hasta reiniciar
}
*/

void serialEvent(){ // Interrupcion de puerto serie

  String arg;
  switch((char) Serial.read()){
    
    // Orden directa
    case 'a': // Iniciar escaneo     
      startScanning();
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
    case 'n': // Setear home aqui (Igual que mandar m0,0)
      moving = false; // Deshabilitar ISR durante las sgtes instrucciones
      pos[0] = 0; pos[1] = 0; // Setear home
      sp[0] = 0; sp[1] = 0;
      moving = true; // La ISR lo vuelve a false 
      Serial.println("n");
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
      Serial.print("g");
      Serial.println(hSweep);
      break;
    case 'h': // Posicion actual
      Serial.print("h");
      Serial.print(pos[0]);
      Serial.print(",");
      Serial.print(pos[1]);
      Serial.print(",");      
      Serial.println(sensor.readRangeSingleMillimeters());
      break;    

    // Comandos con argumento
    case 'i': // Agregar setpoint a la cola
      arg = Serial.readStringUntil('\n'); // Leer argumento
      if( arg != "" ){ // Si el argumento no esta vacio
        int sep, x, y;
        sep = arg.indexOf(","); // Separador
        x = arg.substring(0,sep).toInt(); // Posicion X
        y = arg.substring(sep+1).toInt(); // Posicion Y
        // Limitar valores al rango de trabajo del escaner
        x = x < 0 ? 0 : x > MAXPOSX ? MAXPOSX : x;
        y = y < 0 ? 0 : y > MAXPOSY ? MAXPOSY : y;        
        sp[0] = x; sp[1] = y;   
        moving = true; // Habilitar movimiento de motores      
        Serial.println("i"); // Ack                
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
      hSweep = arg.toInt();
      Serial.println("l"); // Ack
      break;
    case 'm': // Setear posicion actual manualmente
      arg = Serial.readStringUntil('\n'); // Leer argumento
      if( arg != "" ){ // Si el argumento no esta vacio
        int sep, x, y;
        sep = arg.indexOf(","); // Separador
        x = arg.substring(0,sep).toInt(); // Posicion X
        y = arg.substring(sep+1).toInt(); // Posicion Y
        // Limitar valores al rango de trabajo del escaner
        pos[0] = x < 0 ? 0 : x > MAXPOSX ? MAXPOSX : x;
        pos[1] = y < 0 ? 0 : y > MAXPOSY ? MAXPOSY : y;        
        Serial.println("m"); // Ack        
      }
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
      //Serial.print("a"); // Prefijo del dato
      Serial.print(pos[0]);
      Serial.print(",");
      Serial.print(pos[1]);
      Serial.print(",");
      //  if (sensor.timeoutOccurred()) { Serial.println("ST"); }
      Serial.println(sensor.readRangeSingleMillimeters());
    }
  }
}
