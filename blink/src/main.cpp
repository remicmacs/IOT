#include <Arduino.h>

#define LED_OUT 12

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  pinMode(LED_OUT, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_OUT, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println(F("High state"));
  delay(1000);                       // wait for a second
  digitalWrite(LED_OUT, LOW);    // turn the LED off by making the voltage LOW
  Serial.println(F("Low state"));
  delay(500);                       // wait for a second
}
