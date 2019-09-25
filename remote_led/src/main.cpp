#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#define LED_OUT 12
#define HTTP_PORT 80

const char* ssid = "ESP32_AP";
const char* password = "rootroot";

WebServer Server(HTTP_PORT);

bool light = false;
int previous = LOW;

void send_status() {
  Serial.println("LED is " + (light) ? "ON" : "OFF");
  Server.send(200, "text/plain", String(light));
}

void toggle() {
  if (light) {
    digitalWrite(LED_OUT, LOW);
  } else {
    digitalWrite(LED_OUT, HIGH);
  }

  light = !light;
  send_status();
}

void led_on() {
  Serial.println("Request to switch on");
  if (!light) {
    toggle();
  }

  send_status();
}

void led_off() {
  Serial.println("Request to switch off");
  if (light) {
    toggle();
  }
}

void setup() {
  Serial.begin(115200);
  // Set the LED pin as output
  pinMode(LED_OUT, OUTPUT);

  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password, true, 1);
  IPAddress ip = WiFi.softAPIP();
  Serial.println("AP IP address: " + ip.toString());
  Server.on("/on", led_on);
  Server.on("/off", led_off);
  Server.on("/toggle", toggle);
  Server.on("/get", send_status);
  Server.begin();
}


void loop() {
  Server.handleClient();
  // IPAddress ip = WiFi.softAPIP();
  // Serial.println("AP IP address: " + ip.toString());
}
