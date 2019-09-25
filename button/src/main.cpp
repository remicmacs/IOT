#include <Arduino.h>

#define LED_OUT 12
#define BUTTON_IN 14

bool light = false;
int previous = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(LED_OUT, OUTPUT);
  pinMode(BUTTON_IN, INPUT_PULLDOWN);

  digitalWrite(LED_OUT, LOW);
}

void toggle() {
  if (light) {
    digitalWrite(LED_OUT, LOW);
  }
  else {
    digitalWrite(LED_OUT, HIGH);
  }
  light = !light;
}

void loop() {
  int state = digitalRead(BUTTON_IN);
  if (previous != state && state == HIGH) {
    toggle();
  }
  previous = state;
  Serial.printf("State : %s\n", (state == HIGH) ? "HIGH" : "LOW");
  delay(100);
}
