#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "APESP32";
const char* password =  "rootroot";

const int LED = 2;

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Establishing connection to WiFi..");
  }

  int blink = 3;
  while (blink-- > 0) {
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  Serial.println("Connected to network");

}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.scanNetworks();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
}

void loop() {}