#include <TinyGPS.h>
#include "NexStarGPS.h"
#include "soss.h"
#include "ross.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

// Using default pins SDA A4 and SCL A5, no reset pin
#define D_WIDTH 128
#define D_HEIGHT 64

Adafruit_SSD1306 display(D_WIDTH, D_HEIGHT, &Wire, -1);

#define RX_PIN 3
#define TX_PIN 5

#define SIGNAL_PIN 9
#define LED_PIN 13

ross mountserial(RX_PIN);
soss sendmountserial(TX_PIN);

TinyGPS gps;

NexstarMessageReceiver msg_receiver;
NexstarMessageSender msg_sender(&gps);

unsigned int displayCountRX =0;
unsigned int displayCountSats =0;
unsigned long gps_time = 0;
unsigned long fix_age =0;
unsigned int displayMountRX =0;
unsigned int displayMountTX =0;
 
boolean haveLock = false;

void setup() {
  // GPS module speed
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;){ // Don't proceed, loop forever
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(200);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(300);                
    }
  }
  delay(2000); // time required for display to init
  display.clearDisplay();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, LOW);
  
    
  pinModeTri(RX_PIN);
  pinModeTri(TX_PIN);
  mountserial.begin(19200);
  msg_receiver.reset();
  
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
}



void loop() {
    
    if (mountserial.available())
    {
      int c = mountserial.read();
      displayMountRX++;
      //Serial.println(c, HEX);
      if (msg_receiver.process(c))
      {
        if (msg_sender.handleMessage(&msg_receiver))
        {
          digitalWrite(LED_PIN, HIGH);
          mountserial.end();
          delay(100);
          sendmountserial.begin(19200);
          msg_sender.send(&sendmountserial,displayMountTX);
          sendmountserial.end();
          pinModeTri(RX_PIN);
          pinModeTri(TX_PIN);
          mountserial.begin(19200);
          digitalWrite(LED_PIN, LOW);
        }
      }
    }

    if (Serial.available())
      {      
        char c = Serial.read();
        
        gps.encode(c);
        
        displayCountRX++;

        if (!haveLock)
        {
          // Save serial character to display buffer...
          display.write(c);
        
          // If we have 168 chars the display them and clear the buffer
          if (displayCountRX == 168) // Wrap at 50 25
          {
            
            display.display(); // Write out whole screen
            display.clearDisplay();
            displayCountRX = 0; // Start count again
            display.setCursor(0, 0);     // Start collecting again at top-left corner            
          }
        }
        else
        {
          // if we have lock we just don't want displayCountRX to overflow...
          if (displayCountRX>999) // Wrap at 1000
          {
            displayCountRX = 0;
          }
        }
        
        // What happens when you call this and encode returned false?
        // You get GPS_INVALID_AGE I think
        // Else this just gets bigger and bigger (time since last good read.)
        gps.get_datetime(NULL, &gps_time, &fix_age);
  

        displayCountSats = gps.satellites();

        if ((fix_age == gps.GPS_INVALID_AGE) || (fix_age > 10000) || (gps.satellites() == gps.GPS_INVALID_SATELLITES) || (gps.satellites() < 4))
        {
          haveLock = false;
        }
        else
        {
          haveLock = true;
        }

      }

      if ( haveLock )
      {
        testdrawAll();
      }
}


inline void pinModeTri(int pin)
{
  //digitalWrite(pin, LOW);
  //pinMode(pin, OUTPUT);
  pinMode(pin, INPUT);
}

// save some chars
/** ----------------------------------|-------------------||-------------------||-------------------||-------------------||-------------------||-------------------||-------------------||-------------------|*/
const char signMessage[] PROGMEM  = {"Sat:                 GPS/RX:              FAge:                Tm:                  Mrx      tx          CREATED BY THE USA   Stronger, Faster...  Better than ever"};



void drawUnsignedInt(unsigned int& a,int x, int y)
{
  display.setCursor(x, y);     // Start at top-left corner
  char cstr[10];
  itoa(a, cstr, 10);
  for (int i=0; i < 10 && cstr[i] != NULL; i++)
  {
    display.write(cstr[i]);
  }
}
void drawUnsignedLong(unsigned long& a,int x, int y)
{
  display.setCursor(x, y);     // Start at top-left corner
  char cstr[10];
  ltoa(a, cstr, 10);
  for (int i=0; i < 10 && cstr[i] != NULL; i++)
  {
    display.write(cstr[i]);
  }
}

void drawTime(unsigned long& time,int x, int y)
{
  display.setCursor(x, y);     // Start at top-left corner
  char cstr[10];
  ltoa(gps_time / 1000000, cstr, 10);
  for (int i=0; i < 10 && cstr[i] != NULL; i++)
  {
    display.write(cstr[i]);
  }
  display.write(':');
  ltoa((gps_time / 10000) % 100, cstr, 10);
  for (int i=0; i < 10 && cstr[i] != NULL; i++)
  {
    display.write(cstr[i]);
  }
  display.write(':');
  ltoa((gps_time / 100) % 100, cstr, 10);
  for (int i=0; i < 10 && cstr[i] != NULL; i++)
  {
    display.write(cstr[i]);
  }
}



void testdrawAll()
{
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  int len = strlen_P(signMessage);
  for (int k = 0; k < len; k++)
  {
    char myChar =  pgm_read_byte_near(signMessage + k);
    display.write(myChar);
  }

  // Write satellites value:
  drawUnsignedInt(displayCountSats,24,0);
  // Write GPS/RX value:
  drawUnsignedInt(displayCountRX,44,8);
  // Write fix Age:
  drawUnsignedLong(fix_age,34,16);
  // Write time:
  drawTime(gps_time,24,24);
  // Write mount RX & TX
  drawUnsignedInt(displayMountRX,22,32);
  drawUnsignedInt(displayMountTX,46,32);

  display.display();
}
