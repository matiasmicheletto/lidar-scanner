#include <Servo.h>

#define SERVO_PIN  9

#define CW   2500
#define STOP 1900
#define CCW  1000

Servo myServo;
float t = 0;

void setup()
{
    Serial.begin(9600);
    myServo.attach(SERVO_PIN);
    myServo.writeMicroseconds(STOP);
}

void loop()
{
  t+=0.1;
  int y = (int) 1250*sin(t)+1250;
  myServo.writeMicroseconds(y);
  Serial.print("Vel: ");
  Serial.println(y);
  delay(100);
}
