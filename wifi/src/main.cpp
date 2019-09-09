#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "YameteRodolpheSan";
const char* password =  "E5D4C3B2A1";

const int LED = 2;

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
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