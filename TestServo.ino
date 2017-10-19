#include <Servo.h>

#define SERVO_PIN  9

#define CW   2500
#define STOP 1900
#define CCW  1000

Servo myServo;

char c;

void setup()
{
    Serial.begin(9600);
    myServo.attach(SERVO_PIN);
    myServo.writeMicroseconds(STOP);
}

void loop()
{
    while (Serial.available() > 0) {
        c = Serial.read();

        switch (c) {
            case 'c':
                myServo.writeMicroseconds(CW);
                Serial.println("Derecha");
                break;
            case 'w':
                myServo.writeMicroseconds(CCW);
                Serial.println("Izquierda");
                break;
            case 's':
                myServo.writeMicroseconds(STOP);
                Serial.println("Detenido");
                break;
        }
    }
}
