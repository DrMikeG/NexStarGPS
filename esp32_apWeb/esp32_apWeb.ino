#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "index.h"  //Web page header file

#include "soss.h"
#include "ross.h"

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "NexStarGPS.h"
/*
  This sample code demonstrates just about every built-in operation of TinyGPS++ (TinyGPSPlus).
  
  I have a 9600 baud serial GPS device (Neo-6m) hooked up to pins D12 and D13.

  I am porting code from https://circuits4you.com/2018/11/20/web-server-on-esp32-how-to-update-and-display-sensor-values/
  to display values on a refreshing webpage
   
*/
static const int RXPin = 13, TXPin = 12;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;


// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// The serial input from the mount
static const int mountRXPin = 14, mountNullPin = 26;
ross mountserial(mountRXPin,mountNullPin); // Only the rx pin will be used?

#define mountTX_PIN 5
soss sendmountserial(mountTX_PIN);


NexstarMessageReceiver msg_receiver;

// For stats that happen every 5 seconds
unsigned long last = 0UL;

WebServer server(80);
 
//Enter your SSID and PASSWORD
const char* ssid = "NexstarGPS";
const char* password = "12345678";
IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,0,254);
IPAddress subnet(255,255,255,0);


String bufferString;
String bufferString2;
 
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleADC() {
  String adcValue = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><serialData><serial1>" + bufferString + "</serial1><serial2>"+bufferString2+"</serial2></serialData>"; 
  server.send(200, "application/xml", adcValue ); 
}


void setup()
{
  
  // Test output
  Serial.begin(115200);

  // GPS input/output
  ss.begin(GPSBaud);


 // Mount serial in
  mountserial.begin(19200);


//ESP32 As access point
  WiFi.mode(WIFI_AP);

  Serial.println(WiFi.softAP(ssid,password) ? "soft-AP setup": "Failed to connect");
  delay(100);
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet)? "Configuring Soft AP" : "Error in Configuration");    
  Serial.println(WiFi.softAPIP());

  //----------------------------------------------------------------
 
  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");

  Serial.println(F("KitchenSink.ino"));
  Serial.println(F("Demonstrating nearly every feature of TinyGPS++"));
  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();

  bufferString = "GPS No lock"; // Clear
  bufferString2 = "No mount data recieved";

  pinModeTri(mountRXPin);
  pinModeTri(mountNullPin);
 
  mountserial.begin(19200);
  msg_receiver.reset();
  
}


template<class T>
void printAndAppend(T param)
{
  bufferString += String(param);
  Serial.print(param);
}

template<class T>
void printAndAppend2(T param)
{
  bufferString2 += String(param);
  Serial.print(param);
}



void printGPSData()
{
  // Dispatch incoming characters
  while (ss.available() > 0)
    gps.encode(ss.read());



  if (gps.location.isUpdated())
  {
    bufferString = ""; // Clear
    printAndAppend(F("LOCATION   Fix Age="));
    printAndAppend(gps.location.age());
    printAndAppend(F("ms Raw Lat="));
    printAndAppend(gps.location.rawLat().negative ? "-" : "+");
    printAndAppend(gps.location.rawLat().deg);
    printAndAppend("[+");
    printAndAppend(gps.location.rawLat().billionths);
    printAndAppend(F(" billionths],  Raw Long="));
    printAndAppend(gps.location.rawLng().negative ? "-" : "+");
    printAndAppend(gps.location.rawLng().deg);
    printAndAppend("[+");
    printAndAppend(gps.location.rawLng().billionths);
    printAndAppend(F(" billionths],  Lat="));
    printAndAppend(gps.location.lat());
    printAndAppend(F(" Long="));
    printAndAppend(gps.location.lng());
    printAndAppend('\n');
  }

  else if (gps.date.isUpdated())
  {
    Serial.print(F("DATE       Fix Age="));
    Serial.print(gps.date.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.date.value());
    Serial.print(F(" Year="));
    Serial.print(gps.date.year());
    Serial.print(F(" Month="));
    Serial.print(gps.date.month());
    Serial.print(F(" Day="));
    Serial.println(gps.date.day());
  }

  else if (gps.time.isUpdated())
  {
    Serial.print(F("TIME       Fix Age="));
    Serial.print(gps.time.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.time.value());
    Serial.print(F(" Hour="));
    Serial.print(gps.time.hour());
    Serial.print(F(" Minute="));
    Serial.print(gps.time.minute());
    Serial.print(F(" Second="));
    Serial.print(gps.time.second());
    Serial.print(F(" Hundredths="));
    Serial.println(gps.time.centisecond());
  }

  else if (gps.speed.isUpdated())
  {
    Serial.print(F("SPEED      Fix Age="));
    Serial.print(gps.speed.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.speed.value());
    Serial.print(F(" Knots="));
    Serial.print(gps.speed.knots());
    Serial.print(F(" MPH="));
    Serial.print(gps.speed.mph());
    Serial.print(F(" m/s="));
    Serial.print(gps.speed.mps());
    Serial.print(F(" km/h="));
    Serial.println(gps.speed.kmph());
  }

  else if (gps.course.isUpdated())
  {
    Serial.print(F("COURSE     Fix Age="));
    Serial.print(gps.course.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.course.value());
    Serial.print(F(" Deg="));
    Serial.println(gps.course.deg());
  }

  else if (gps.altitude.isUpdated())
  {
    Serial.print(F("ALTITUDE   Fix Age="));
    Serial.print(gps.altitude.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.altitude.value());
    Serial.print(F(" Meters="));
    Serial.print(gps.altitude.meters());
    Serial.print(F(" Miles="));
    Serial.print(gps.altitude.miles());
    Serial.print(F(" KM="));
    Serial.print(gps.altitude.kilometers());
    Serial.print(F(" Feet="));
    Serial.println(gps.altitude.feet());
  }

  else if (gps.satellites.isUpdated())
  {
    bufferString = ""; // Clear
    printAndAppend(F("SATELLITES Fix Age="));
    printAndAppend(gps.satellites.age());
    printAndAppend(F("ms Value="));
    printAndAppend(gps.satellites.value());
  }

  else if (gps.hdop.isUpdated())
  {
    Serial.print(F("HDOP       Fix Age="));
    Serial.print(gps.hdop.age());
    Serial.print(F("ms Value="));
    Serial.println(gps.hdop.value());
  }

  else if (millis() - last > 5000)
  {
    Serial.println();
    if (gps.location.isValid())
    {
      static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
      double distanceToLondon =
        TinyGPSPlus::distanceBetween(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON);
      double courseToLondon =
        TinyGPSPlus::courseTo(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON);

      Serial.print(F("LONDON     Distance="));
      Serial.print(distanceToLondon/1000, 6);
      Serial.print(F(" km Course-to="));
      Serial.print(courseToLondon, 6);
      Serial.print(F(" degrees ["));
      Serial.print(TinyGPSPlus::cardinal(courseToLondon));
      Serial.println(F("]"));
    }

    Serial.print(F("DIAGS      Chars="));
    Serial.print(gps.charsProcessed());
    Serial.print(F(" Sentences-with-Fix="));
    Serial.print(gps.sentencesWithFix());
    Serial.print(F(" Failed-checksum="));
    Serial.print(gps.failedChecksum());
    Serial.print(F(" Passed-checksum="));
    Serial.println(gps.passedChecksum());

    if (gps.charsProcessed() < 10)
      Serial.println(F("WARNING: No GPS data.  Check wiring."));

    last = millis();
    Serial.println();
  }
}

void checkForMountSerialInput()
{
if (mountserial.available())
  {
    int c = mountserial.read();
    Serial.println(c, HEX);
    if (msg_receiver.process(c))
    {
      bufferString2 = "";
      nexstar_msg_union* msgin = msg_receiver.getMessage();
      if (msgin->msg.header.to != DEVICE_GPS)
      {
        printAndAppend2("Message header is not intended for GPS device: ");
        printAndAppend2(msgin->msg.header.to);
        printAndAppend2('\n');       
      }
      else
      {
        printAndAppend2("Message header is intended for GPS device ");
        printAndAppend2(msgin->msg.header.to);
        printAndAppend2('\n');
        printAndAppend2("Message from ");
        printAndAppend2(msgin->msg.header.from);
        printAndAppend2('\n');
        printAndAppend2("Message ID ");
        printAndAppend2(msgin->msg.header.messageid);
        printAndAppend2('\n');
      }  
    }
  }
  else
  {
      //Serial.println("mountserial.available() == false");
  }
}


void loop()
{
   server.handleClient();
   checkForMountSerialInput();
   //printGPSData();
}


inline void pinModeTri(int pin)
{
  //digitalWrite(pin, LOW);
  //pinMode(pin, OUTPUT);
  pinMode(pin, INPUT);
}
