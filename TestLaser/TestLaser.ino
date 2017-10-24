#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup()
{
  Serial.begin(57600);
  Wire.begin();

  sensor.init();
  sensor.setTimeout(300);
  sensor.startContinuous();

  Serial.println("Inicializacion terminada");
}

void loop()
{
  Serial.print(sensor.readRangeContinuousMillimeters());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();
}
