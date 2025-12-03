#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

OneWire oneWire(GPIO_NUM_27); // DS18B20 external
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(115200);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Dallas Temp Test");

  sensors.begin();
}

void loop()
{
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);

  Serial.println("Temp: " + String(temp) + " °C");

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);

  if (temp == DEVICE_DISCONNECTED_C)
  {
    tft.println("Error: No DS18B20");
  }
  else
  {
    tft.println("External Temp: " + String(temp) + "C");
  }

  delay(1000);
}