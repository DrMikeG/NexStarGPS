#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Using default pins SDA A4 and SCL A5, no reset pin
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
}

int i = 0; 

void loop() {

  
  testdrawAll(i);
  delay(2000);
  i++;
}

void drawString(char* toDraw)
{
  int i=0;
  while(toDraw[i] != NULL)
  {
    display.write((uint8_t)toDraw[i]);
    i++;
  }
}

void drawInt(int a)
{
  String myStr;
  myStr = String(a);   //Converts integer to string)
  drawString(myStr.c_str());
}

void testdrawAll(int satelites)
{
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  drawString("satelites:");
  drawInt(satelites);
  display.display();
}

