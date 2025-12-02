#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>

OneWire oneWire(GPIO_NUM_15);
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(115200);
  sensors.begin();
}

void loop()
{
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  Serial.println("Temp: " + String(temp) + " °C");
  delay(1000);
}