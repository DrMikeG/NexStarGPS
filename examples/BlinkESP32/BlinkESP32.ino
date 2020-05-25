/*
  Blink
  ) Go to Tools > Board, scroll down to the ESP32 section and select the name of your ESP32 board. In my case, it’s the DOIT ESP32 DEVKIT V1 board.
  
*/

// ledPin refers to ESP32 GPIO 23
const int ledPin = 23;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin ledPin as an output.
  pinMode(ledPin, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                  // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                  // wait for a second
}
