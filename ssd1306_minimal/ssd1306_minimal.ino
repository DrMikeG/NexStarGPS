#include <TinyGPS.h>
#include "NexStarGPS.h"
#include "soss.h"
#include "ross.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Using default pins SDA A4 and SCL A5, no reset pin
Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define RX_PIN 3
#define TX_PIN 5

#define SIGNAL_PIN 9
#define LED_PIN 13

ross mountserial(RX_PIN);
soss sendmountserial(TX_PIN);

TinyGPS gps;

NexstarMessageReceiver msg_receiver;
NexstarMessageSender msg_sender(&gps);

int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval_nolock = 150;           // interval at which to blink (milliseconds)
long interval_lock = 5;

int count =0;
char cstr[16];

boolean haveLock = false;
void setup() {
  // GPS module speed
  Serial.begin(57600);


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, LOW);
  
    
  pinModeTri(RX_PIN);
  pinModeTri(TX_PIN);
  mountserial.begin(19200);
  msg_receiver.reset();

  
  pinMode(LED_BUILTIN, OUTPUT);
}



void loop() {

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  testdrawAll(count++);

}


inline void pinModeTri(int pin)
{
  //digitalWrite(pin, LOW);
  //pinMode(pin, OUTPUT);
  pinMode(pin, INPUT);
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
  itoa(a, cstr, 10);
  drawString(&cstr[0]);
}

void testdrawAll(int satelites)
{
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  drawString("satelites 2:");
  drawInt(satelites);
  display.display();
}
