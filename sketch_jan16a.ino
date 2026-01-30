#include <TFT_eSPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 21

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite ui = TFT_eSprite(&tft);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress tempInside, tempOutside;

// KOLORY 
#define C_BG        0x10A2  
#define C_PANEL     0x2124  
#define C_BORDER    0xAD45  
#define C_TEXT      0xCE79  
#define C_SHADOW    0x0000  
#define C_COLD      0x4416  
#define C_HOT       0xB9E1  
#define C_WARM      0xEBC0  

unsigned long lastSensorTime = 0;
const int sensorInterval = 2000; // Odczyt co 2 sekundy dla stabilności
float cachedInside = 0;
float cachedOutside = 0;
float animFloat = 0;

// FONT
struct Glyph { char c; uint8_t data[5]; };
const Glyph pixelFont[] = {
  {'0', {0x1F, 0x11, 0x11, 0x11, 0x1F}}, {'1', {0x04, 0x0C, 0x04, 0x04, 0x0E}},
  {'2', {0x1F, 0x01, 0x1F, 0x10, 0x1F}}, {'3', {0x1F, 0x01, 0x0F, 0x01, 0x1F}},
  {'4', {0x11, 0x11, 0x1F, 0x01, 0x01}}, {'5', {0x1F, 0x10, 0x1F, 0x01, 0x1F}},
  {'6', {0x1F, 0x10, 0x1F, 0x11, 0x1F}}, {'7', {0x1F, 0x01, 0x02, 0x04, 0x04}},
  {'8', {0x1F, 0x11, 0x1F, 0x11, 0x1F}}, {'9', {0x1F, 0x11, 0x1F, 0x01, 0x1F}},
  {'I', {0x1F, 0x04, 0x04, 0x04, 0x1F}}, {'N', {0x11, 0x19, 0x15, 0x13, 0x11}},
  {'D', {0x1E, 0x11, 0x11, 0x11, 0x1E}}, {'S', {0x0F, 0x10, 0x0E, 0x01, 0x1E}},
  {'E', {0x1F, 0x10, 0x1E, 0x10, 0x1F}}, {'X', {0x11, 0x0A, 0x04, 0x0A, 0x11}},
  {'T', {0x1F, 0x04, 0x04, 0x04, 0x04}}, {'R', {0x1E, 0x11, 0x1E, 0x14, 0x12}},
  {'A', {0x0E, 0x11, 0x1F, 0x11, 0x11}}, {'L', {0x10, 0x10, 0x10, 0x10, 0x1F}},
  {'C', {0x0F, 0x10, 0x10, 0x10, 0x0F}}, {'O', {0x1F, 0x11, 0x11, 0x11, 0x1F}},
  {'U', {0x11, 0x11, 0x11, 0x11, 0x1F}}, {'°', {0x06, 0x09, 0x09, 0x06, 0x00}},
  {'-', {0x00, 0x00, 0x1F, 0x00, 0x00}} 
};

// Funkcje pomocnicze
void drawChar3D(int x, int y, char ch, uint16_t color, int scale) {
  for (int i = 0; i < sizeof(pixelFont)/sizeof(Glyph); i++) {
    if (pixelFont[i].c == ch) {
      for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
          if (pixelFont[i].data[r] & (1 << (4 - c))) {
            ui.fillRect(x + c*scale + 1, y + r*scale + 1, scale, scale, C_SHADOW);
            ui.fillRect(x + c*scale, y + r*scale, scale, scale, color);
          }
        }
      }
      return; 
    }
  }
}

void drawPixelText(int x, int y, const char* txt, uint16_t color, int scale) {
  int offset = 0;
  while (*txt) {
    drawChar3D(x + offset, y, *txt, color, scale);
    offset += scale * 6;
    txt++;
  }
}

void drawRPGBar(int x, int y, int w, int h, float val, float minVal, float maxVal, uint16_t color) {
  ui.fillRect(x, y, w, h, 0x0000);
  ui.drawRect(x, y, w, h, C_BORDER);
  float constrained = constrain(val, minVal, maxVal);
  int barW = map(constrained * 10, minVal * 10, maxVal * 10, 0, w - 4);
  if (barW > 0) {
    ui.fillRect(x + 2, y + 2, barW, h - 4, color);
    ui.drawFastHLine(x + 2, y + 3, barW, 0xFFFF); 
  }
}

void drawPanel(int x, int y, int w, int h, const char* label) {
  ui.fillRect(x, y, w, h, C_PANEL);
  ui.drawRect(x, y, w, h, C_BORDER);
  drawPixelText(x + 15, y + 8, label, C_BORDER, 2);
  ui.fillRect(x, y, 4, 4, C_BORDER);
  ui.fillRect(x + w - 4, y, 4, 4, C_BORDER);
  ui.fillRect(x, y + h - 4, 4, 4, C_BORDER);
  ui.fillRect(x + w - 4, y + h - 4, 4, 4, C_BORDER);
}

void setup() {
  tft.init();
  tft.setRotation(1);
  ui.setColorDepth(8);
  ui.createSprite(320, 240);

  sensors.begin();
  sensors.requestTemperatures(); // Pierwszy odczyt
  delay(800); // Czekamy na pierwszy wynik, żeby nie było -127
  cachedInside = sensors.getTempCByIndex(0);
  cachedOutside = sensors.getTempCByIndex(1);
}

void loop() {
  unsigned long currentMillis = millis();

  // Odczyt czujników co sensorInterval (bez blokowania animacji)
  if (currentMillis - lastSensorTime >= sensorInterval) {
    lastSensorTime = currentMillis;
    sensors.requestTemperatures();
    float t1 = sensors.getTempCByIndex(0);
    float t2 = sensors.getTempCByIndex(1);
    
    // Filtrujemy błędne odczyty, żeby ekran nie migał na -127
    if (t1 > -50 && t1 < 80) cachedInside = t1;
    if (t2 > -50 && t2 < 80) cachedOutside = t2;
  }

  animFloat += 0.15; // Prędkość animacji
  int fY1 = sin(animFloat) * 3;
  int fY2 = cos(animFloat) * 3;

  ui.fillSprite(C_BG);

  // INDOOR
  drawPanel(10, 10, 300, 100, "INDOOR");
  uint16_t inCol = (cachedInside < 18) ? C_COLD : (cachedInside > 25 ? C_HOT : C_WARM);
  drawPixelText(30, 45 + fY1, String((int)cachedInside).c_str(), inCol, 6);
  drawPixelText(100 + (String((int)cachedInside).length() * 2), 55 + fY1, "°C", C_TEXT, 3);
  drawRPGBar(160, 50, 130, 20, cachedInside, 10, 35, inCol);

  // OUTSIDE
  drawPanel(10, 120, 300, 100, "OUTSIDE"); 
  uint16_t outCol = (cachedOutside < 0) ? C_COLD : (cachedOutside > 20 ? C_HOT : C_TEXT);
  drawPixelText(30, 155 + fY2, String((int)cachedOutside).c_str(), outCol, 6);
  drawPixelText(100 + (String((int)cachedOutside).length() * 2), 165 + fY2, "°C", C_TEXT, 3);
  drawRPGBar(160, 160, 130, 20, cachedOutside, -10, 35, outCol);

  ui.pushSprite(0, 0);
}