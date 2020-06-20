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

int ledState = LOW;             // ledState used to set the LED

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval_nolock = 150;           // interval at which to blink (milliseconds)
long interval_lock = 5;

int count =0;

unsigned long displayCountRX =0;
unsigned long displayCountSats =0;

unsigned long fix_age =0;
 
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

  
   displayCountSats = 404;
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
      //Serial.println(c, HEX);
      if (msg_receiver.process(c))
      {
        if (msg_sender.handleMessage(&msg_receiver))
        {
          digitalWrite(LED_PIN, HIGH);
          mountserial.end();
          delay(100);
          sendmountserial.begin(19200);
          msg_sender.send(&sendmountserial);
          sendmountserial.end();
          pinModeTri(RX_PIN);
          pinModeTri(TX_PIN);
          mountserial.begin(19200);
          digitalWrite(LED_PIN, LOW);
        }
      }
      displayCountRX++;
    }

    if (Serial.available())
      {      
        char c = Serial.read();
        if (gps.encode(c))
        {
          displayCountSats = 200;
        }
      
        display.write(c);
        displayCountRX++;
        if (displayCountRX == 168) // Wrap at 50 25
        {
          display.display();
          display.setCursor(0, 0);     // Start at top-left corner
          display.clearDisplay();
          displayCountRX = 0;
        }
    
        //if (displayCountRX>999) // Wrap at 1000
        //  displayCountRX = 0;
    
        gps.get_datetime(NULL, NULL, &fix_age);

        //displayCountSats = gps.satellites();
        if ((fix_age == gps.GPS_INVALID_AGE) || (fix_age > 5000) || (gps.satellites() == gps.GPS_INVALID_SATELLITES) || (gps.satellites() < 4))
        {
          haveLock = false;
        }
        else
        {
          haveLock = true;
        }
      }


  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);                       // wait for a second
  
  if (haveLock)
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
const char signMessage[] PROGMEM  = {"Sat:                 GPS/RX:              FAge:                I AM PREDATOR,       UNSEEN COMBATANT.    CREATED BY THE USA   Stronger, Faster...  Better than ever"};


char cstr[10];
void drawUnsignedLong(unsigned long a,int x, int y)
{
  // Draw a up to 000 - 999
  // Draw         01k - 99k
  // Draw         .1M - 99m
  // Draw         .1B - 2b+
  // Int overflows at 2b

  display.setCursor(x, y);     // Start at top-left corner
  
  if (a < 1000)
  {
    ltoa(a, cstr, 10);
    display.write(cstr[0]);
    display.write(cstr[1]);
    display.write(cstr[2]);
  }
  else
  {
    ltoa(a, cstr, 10);
    display.write(cstr[0]);
    display.write(cstr[1]);
    display.write(cstr[2]);
    display.write(cstr[3]);
    display.write(cstr[4]);
    display.write(cstr[5]);
    display.write(cstr[6]);
    display.write(cstr[7]);
    display.write(cstr[8]);
    display.write(cstr[9]);
  }
  
}

char myChar;
int k;    // counter variable
void testdrawAll()
{
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // read back a char
  int len = strlen_P(signMessage);
  for (k = 0; k < len; k++)
  {
    myChar =  pgm_read_byte_near(signMessage + k);
    display.write(myChar);
  }

  // Write satellites value:
  drawUnsignedLong(displayCountSats,24,0);
  // Write GPS/RX value:
  drawUnsignedLong(displayCountRX,44,8);
  // Write fix Age:
  drawUnsignedLong(fix_age,34,16);
  
  display.display();
}
