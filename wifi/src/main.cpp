#include <Arduino.h>

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
}

void loop() {
    Serial.println(F("Hello, world!"));
}